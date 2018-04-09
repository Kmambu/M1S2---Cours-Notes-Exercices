#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pid.h>
#include <linux/moduleparam.h>
#include <linux/kthread.h>

#define BUFSIZE 64

MODULE_DESCRIPTION("Monitors a task");
MODULE_AUTHOR("Kevin Mambu");
MODULE_LICENSE("GPL");

static int target;
module_param(target,int,0644);
MODULE_PARM_DESC(target, "Process ID of monitored task");

static struct task_monitor
{
	struct pid *pid;
} tm;

struct task_sample {
	struct mutex mutex;
	struct list_head list;
	cputime_t utime;
	cputime_t stime;
} sample;

/******************************/
/***** sampling functions *****/
/******************************/
ssize_t format_string(struct task_sample *sample,char *buf, ssize_t max)
{
	strcpy(buf,"");
	snprintf(buf,max,"pid %d usr %ld sys %ld\n",
			target, sample->utime, sample->stime);
	return strlen(buf);
}

bool get_sample(struct task_monitor *tm, struct task_sample *sample,
		char *buf, int *size)
{
	struct task_struct *t;
	if(!(t = get_pid_task(tm->pid,PIDTYPE_PID)))
	{
		pr_warn("[taskmon] Sampling of %d failed unexpectedly\n",
				target);
		return 0;
	}
	sample->utime = t->utime;
	sample->stime = t->stime;
	(*size) = format_string(sample,buf,(*size));
	put_task_struct(t);
	return 1;
}

/****************************************/
/***** kthread monitoring functions *****/
/****************************************/
static struct task_struct *kthread;
static bool is_kthread_running = 0;
int monitor_fn(void *unused)
{
	int size;
	char buf[BUFSIZE];
	while(!kthread_should_stop())
	{
		size = BUFSIZE;
		if(get_sample(&tm, &sample, buf, &size)) {
			pr_warn("%s",buf);
			set_current_state(TASK_UNINTERRUPTIBLE);
			schedule_timeout(HZ);
		}
	}
	return 0;
}

int monitor_pid(pid_t pid)
{
	tm.pid = find_get_pid(target);
	if (tm.pid == NULL) {
		pr_warn("[taskmon] Process %d not found \n", target);
		return -1;
	}
	pr_warn("[taskmon] Process %d found\n", target);
	sample.list = LIST_HEAD_INIT(sample.list);
	kthread = kthread_run(monitor_fn, NULL, "monitor_fn");
	is_kthread_running = 1;
	return 0;
}

/***************************/
/***** sysfs functions *****/
/***************************/
ssize_t taskmon_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	int ret = BUFSIZE;
	char src[BUFSIZE];
	if (!(get_sample(&tm, &sample,src,&ret)))
		return 0;
	strncpy(buf,src,BUFSIZE);
	return ret;
}

ssize_t taskmon_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	char dest[BUFSIZE];
	strncpy(dest,buf,BUFSIZE);
	if (strcmp(dest,"start\n") == 0) {
		pr_warn("[taskmon] Relaunching a monitoring of %d\n", target);
		if(is_kthread_running)
			kthread_stop(kthread);
		monitor_pid(target);
		return 1;
	} else if (strcmp(dest,"stop\n") == 0) {
		pr_warn("[taskmon] Monitoring of %d halted\n",target);
		kthread_stop(kthread);
		is_kthread_running = 0;
		return 2;
	}
	return 3;
}

static struct kobj_attribute taskmon = __ATTR_RW(taskmon);

/******************************************************/
/***** module initialization and cleanup methodes *****/
/******************************************************/
int init_module(void)
{
	sysfs_create_file(kernel_kobj,&(taskmon.attr));
	pr_warn("[taskmon] Initializing Task Monitor\n");
	return monitor_pid(target);
}

void cleanup_module(void)
{
	kthread_stop(kthread);
	sysfs_remove_file(kernel_kobj,&(taskmon.attr));
	pr_warn("[taskmon] Unloading Task Monitor\n");
}
