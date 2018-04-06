#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/pid.h>
#include <linux/moduleparam.h>
#include <linux/kthread.h>

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
static struct task_struct *kthread;

int monitor_fn(void *unused)
{
	struct task_struct *t;
	while(!kthread_should_stop())
	{
		if(!(t = get_pid_task(tm.pid, PIDTYPE_PID))) {
			pr_warn("[taskmon] Monitoring of %d stopped\n",target);
			return -1;
		}
		pr_warn("pid %d usr %ld sys %ld\n",target, t->utime, t->stime);
		set_current_state(TASK_UNINTERRUPTIBLE);
		schedule_timeout(HZ);
		put_task_struct(t);
	}
	put_pid(tm.pid);
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
	kthread = kthread_run(monitor_fn, NULL, "monitor_fn");
	return 0;
}

int init_module(void)
{
	pr_warn("[taskmon] Initializing Task Monitor\n");
	return monitor_pid(target);
}

void cleanup_module(void)
{
	if (kthread)
		kthread_stop(kthread);
	pr_warn("[taskmon] Unloading Task Monitor\n");
}
