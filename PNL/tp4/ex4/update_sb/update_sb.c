#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/err.h>
#include <linux/string.h>
#include <linux/time.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <uapi/asm-generic/errno-base.h>
#include <asm/uaccess.h>
#include <asm/segment.h>
#include <asm/processor.h>

MODULE_DESCRIPTION("Prints to the Kernel message console a list of the superblocks of a given type.\nThe date displayed next to the superbloc entry is the elapsed time since its last display through the loading of update_sb.\nSince update_sb requires persistent memory to remember dates of display (at least until the halt of the OS execution), side effects includes files containing date structures being created in the tmpfs pseudo filesystem.\nPlease note we are well aware that the Linux Programmer etiquette indicates to prevent as much as possible accessing files from the kernelspace.");
MODULE_AUTHOR("Kevin Mambu");
MODULE_LICENSE("GPL");
static char* type = "ext4";
module_param(type, charp, 0);
MODULE_PARM_DESC(type, "The filesystem type from which the superblocks are displayed");
static void print_sb_time(struct super_block *sb, void* type)
{
	u8 *uuid = sb->s_uuid;	// UUID of the superbloc
	char s_uuid[64];
	char path[64];		// filename associated to the above-mentioned
	struct timespec old_ts, ts, dlay;
	struct file *file;
	char *buf = (char *)kmalloc(64, GFP_KERNEL);
	loff_t pos = 0;
	ssize_t w_ret;
	mm_segment_t old_fs = get_fs();
	set_fs(KERNEL_DS);
	getnstimeofday(&ts);
	sprintf(s_uuid,"%.2x%.2x%.2x%.2x-%.2x%.2x-%.2x%.2x-%.2x%.2x-%.2x%.2x%.2x%.2x%.2x%.2x",
			uuid[0],uuid[1],uuid[2],uuid[3],
			uuid[4],uuid[5],
			uuid[6],uuid[7],
			uuid[8],uuid[9],
			uuid[10],uuid[11],uuid[12],uuid[13],uuid[14],uuid[15]);
	sprintf(path,"/tmp/%s.%s.date", s_uuid, (char *) type);
	file = filp_open(path,O_RDWR,0644);
	if (IS_ERR(file)) {
		pr_info("%s not found, creating it\n", path);
		file = filp_open(path,O_CREAT,0666);
		filp_close(file,NULL);
		file = filp_open(path,O_RDWR,0666);
		old_ts.tv_sec = ts.tv_sec;
		old_ts.tv_nsec = ts.tv_nsec;
	} else {
		w_ret = vfs_read(file,(void __user *)buf,sizeof(buf),&pos);
		sscanf(buf,"%ld.%ld",&old_ts.tv_sec,&old_ts.tv_nsec);
		pr_info("read buf=%s\n",buf);
	}
	pos = 0;
	pr_info("old_ts=[%ld.%.5ld] ts=[%ld.%.5ld]\n",
			old_ts.tv_sec, old_ts.tv_nsec,
			ts.tv_sec, ts.tv_nsec);
	sprintf(buf,"%ld.%ld",old_ts.tv_sec,old_ts.tv_nsec);
	pr_info("written buf=%s\n",buf);
	w_ret = vfs_write(file,(void __user *)buf,sizeof(buf),&pos);
	dlay.tv_nsec = ts.tv_nsec - old_ts.tv_nsec;
	dlay.tv_sec = ts.tv_sec - old_ts.tv_sec;
	pr_info("uuid=%s type=%s time=[%ld.%.5ld]\n",
			s_uuid,
			(sb->s_type)->name,
			dlay.tv_sec, dlay.tv_nsec);
	filp_close(file,NULL);
	set_fs(old_fs);
	kfree(buf);
}

static int __init hello_init(void)
{
	struct file_system_type *fs_type = get_fs_type(type);
	pr_info("[ PR ] *** update_sb init***\n");
	iterate_supers_type(fs_type,print_sb_time, type);
	return 0;
}
module_init(hello_init);

static void __exit hello_exit(void)
{
	pr_info("[ PR ]*** update_sb exit***\n");
}
module_exit(hello_exit);

