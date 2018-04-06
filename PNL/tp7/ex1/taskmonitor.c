#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/timer.h>
#include <linux/kernel_stat.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <uapi/asm-generic/errno-base.h>
#include <uapi/asm-generic/ioctl.h>

#define TASKMON_BUFSIZ 64
#define TASKMON_STOP _IOR('N',0,void*)
#define TASKMON_START _IOR('M',0,void*)
#define TASKMON_SET_PID _IOR('V',0,void*)

MODULE_DESCRIPTION("Monitors a task...");
MODULE_AUTHOR("Kevin Mambu");
MODULE_LICENSE("GPL");

static struct task_struct *running_thread;
static int running = 1;
char taskmon_buf[TASKMON_BUFSIZ];

static int target;
MODULE_PARM_DESC(target,"Process ID of the target task to monitor");
module_param(target,int,0644);

struct task_sample {
	struct list_head list;
	cputime_t utime;
	cputime_t stime;
};

static struct task_monitor {
	struct pid *pid;
	struct mutex mutex;
	struct task_sample samples;
	struct kref kref;
} task_monitor;

bool get_sample(struct task_monitor *tm, struct task_sample *sample)
{
	struct pid *pid = tm->pid;
	struct task_struct *task = get_pid_task(pid, PIDTYPE_PID);
	if (pid_alive(task))
		return 0;
	task = get_pid_task(pid,PIDTYPE_PID);
	sample->utime = task->utime;
	sample->stime = task->stime;
	put_task_struct(task);
	return 1;
}


int monitor_pid(pid_t target)
{
	task_monitor.pid = find_get_pid(target);
	if (!task_monitor.pid)
		return -1;
	(task_monitor.samples).list.prev = &(task_monitor.samples.list);
	(task_monitor.samples).list.next = &(task_monitor.samples.list);
	mutex_init(&(task_monitor.mutex));
	kref_init(&(task_monitor.kref));
	return 0;
}
void taskmon_release(struct kref* kref) {
	put_pid(task_monitor.pid);
	mutex_destroy(&(task_monitor.mutex));
	pr_warn("taskmon_release executed\n");
}
void save_sample(void)
{
	struct task_sample *sample;
	sample = (struct task_sample *)kmalloc
		(sizeof(struct task_sample),GFP_KERNEL);
	if(get_sample(&task_monitor,sample)) {
		pr_warn("no process to monitor\n");
		kfree(sample);
		return;
	}
	mutex_lock(&(task_monitor.mutex));
	list_add(&(sample->list),&(task_monitor.samples.list));
	mutex_unlock(&(task_monitor.mutex));
	pr_warn("save_sample : success\n");
	return;
}

ssize_t format_sample(struct task_sample *sample,char *buf)
{
	snprintf(buf,TASKMON_BUFSIZ,"pid %d usr %ld sys %ld\n", target,
			sample->utime, sample->stime);
	return strlen(taskmon_buf);
}

ssize_t taskmonitor_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	ssize_t ret = 0;
	ssize_t size = 0;
	char b[TASKMON_BUFSIZ];
	struct task_sample pos;
	struct task_sample *posp = &pos;
	strcpy(buf,"");
	list_for_each_entry_reverse(posp,&(task_monitor.samples.list),list)
	{
		mutex_lock(&(task_monitor.mutex));
		size = format_sample(posp,b);
		mutex_unlock(&(task_monitor.mutex));
		if(ret + size >= PAGE_SIZE)
			break;
		ret += size;
		copy_to_user(buf,b,size);
		pr_warn("%s",b);
	}
	return ret;
}

static struct kobj_attribute taskmonitor = __ATTR_RO(taskmonitor);

int monitor_fn(void *unused)
{
	while (running)
	{
		save_sample();
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(HZ);
	}
	return 0;
}

static int __init hello_init(void)
{
	if(monitor_pid(target) == -1) {
		pr_info("[task_monitor] monitor failed : cannot find process id %d\n", target);
		return -1;
	}
	pr_info("[task_monitor] monitor successful : found process id %d\n", target);
	running_thread = kthread_run(monitor_fn, NULL, "monitor_fn");
	sysfs_create_file(kernel_kobj,&(taskmonitor.attr));
	return 0;
}
module_init(hello_init);

static void __exit hello_exit(void)
{
	struct task_sample *pos;
	running = 0;
	if (running_thread)
		kthread_stop(running_thread);
	sysfs_remove_file(kernel_kobj,&(taskmonitor.attr));
	while(!kref_put(&(task_monitor.kref),taskmon_release)) {
		pos = list_first_entry(&(task_monitor.samples.list),
				struct task_sample,list);
		list_del(&(pos->list));
		kfree(pos);
		pr_warn("sample deleted\n");
	}
	pr_info("Goodbye, cruel world\n");
}
module_exit(hello_exit);

