#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/timer.h>
#include <linux/kernel_stat.h>
#include <linux/kthread.h>

MODULE_DESCRIPTION("Monitors a task...");
MODULE_AUTHOR("Kevin Mambu");
MODULE_LICENSE("GPL");

static struct task_struct *running_thread;
static struct task_struct *task;
static int running = 1;
static int target;
MODULE_PARM_DESC(target,"Process ID of the target task to monitor");
module_param(target,int,0644);

static struct task_monitor {
	struct pid *pid;
} task_monitor;

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
		schedule_timeout(1*HZ);
		if (!pid_alive((const struct task_struct *) task)) {
			running = 0;
			break;
		}
		snprintf(buf,32,"pid %d usr %ld sys %ld", target,
				task->utime, task->stime);
	}
	return 0;
}

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
	pr_info("Goodbye, cruel world\n");
}
module_exit(hello_exit);

