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
static struct task_struct *task;
static int running = 1;
static int target;
static const char *dev_name = "taskmonitor";
static int dev_major = 0;
char taskmon_buf[TASKMON_BUFSIZ];

struct task_sample {
	cputime_t utime;
	cputime_t stime;
} sample;


MODULE_PARM_DESC(target,"Process ID of the target task to monitor");
module_param(target,int,0644);
static struct task_monitor {
	struct pid *pid;
} task_monitor;

bool get_sample(struct task_monitor *tm, struct task_sample *sample)
{
	if (pid_alive(get_pid_task(tm->pid, PIDTYPE_PID)))
		return 0;
	sample->utime = task->utime;
	sample->stime = task->stime;
	return 1;
}

ssize_t taskmonitor_show(struct kobject *kobj, struct kobj_attribute *attr,
		char *buf)
{
	ssize_t ret;
	if(get_sample(&task_monitor,&sample))
		return -ENOTTY;
	ret = strlen(taskmon_buf);
	snprintf(taskmon_buf,TASKMON_BUFSIZ,"pid %d usr %ld sys %ld\n", target,
			sample.utime, sample.stime);
	strncpy(buf,taskmon_buf,ret);
	return ret;
}

static struct kobj_attribute taskmonitor = __ATTR_RO(taskmonitor);

int monitor_pid(pid_t target)
{
	task_monitor.pid = find_get_pid(target);
	if (!task_monitor.pid)
		return -1;
	return 0;
}

int monitor_fn(void *unused)
{
	while (running)
	{
		char buf[32];
		if (get_sample(&task_monitor,&sample)) {
			running = 0;
			break;
		}
		snprintf(buf,32,"pid %d usr %ld sys %ld\n", target,
				sample.utime, sample.stime);
		pr_info("[monitor_fn] %s\n", buf);
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(HZ);
	}
	return 0;
}

long taskmonitor_unlocked_ioctl(struct file* file, unsigned int req,
		unsigned long param)
{
	switch(req) {
		case TASKMON_STOP :
			running = 0;
			if (!running_thread) {
				pr_warn("no task to stop\n");
				return -ENOTTY;
			}
			kthread_stop(running_thread);
			break;
		case TASKMON_START :
			monitor_pid(target);
			if(!task_monitor.pid) {
				pr_warn("no task to start\n");
				return -ENOTTY;
			}
			running_thread = kthread_run(monitor_fn, NULL, "monitor_fn");
			break;
		default :
			pr_info("[helloioctl]\tundefined request\n");
			break;
	}
	return 0;
}

struct file_operations taskmonitor_ops = {
	.unlocked_ioctl = taskmonitor_unlocked_ioctl};

static int __init hello_init(void)
{
	pr_info("Hello, world!\n");
	monitor_pid(target);
	if(!task_monitor.pid) {
		pr_info("[task_monitor] monitor failed : cannot find process id %d\n", target);
		return -1;
	}
	pr_info("[task_monitor] monitor successful : found process id %d\n", target);
	running_thread = kthread_run(monitor_fn, NULL, "monitor_fn");
	task = get_pid_task(task_monitor.pid, PIDTYPE_PID);
	pr_warn("Monitoring thread created\n");
	sysfs_create_file(kernel_kobj,&(taskmonitor.attr));
	pr_warn("Kobj created\n");
	dev_major = register_chrdev(dev_major,dev_name,&taskmonitor_ops);
	return 0;
}
module_init(hello_init);

static void __exit hello_exit(void)
{
	running = 0;
	if (running_thread)
		kthread_stop(running_thread);
	pr_warn("Monitoring thread unloaded\n");
	put_pid(task_monitor.pid);
	pr_warn("struct pid released\n");
	put_task_struct(task);
	pr_warn("struct task_struct released\n");
	sysfs_remove_file(kernel_kobj,&(taskmonitor.attr));
	unregister_chrdev(dev_major,dev_name);
	pr_info("Goodbye, cruel world\n");
}
module_exit(hello_exit);

