#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <uapi/asm-generic/errno-base.h>
#include <uapi/asm-generic/ioctl.h>

#define MAGICN 'N'
#define HELLO	_IOR(MAGICN, 0, char *)
#define WHO	_IOW(MAGICN, 0, char *)

#define BUFSIZE 64

MODULE_DESCRIPTION("Module \"hello word\" pour noyau linux");
MODULE_AUTHOR("Julien Sopena, LIP6");
MODULE_LICENSE("GPL");

static const char* dev_name = "hello";
static int dev_major = 0;
static char hello_buf[BUFSIZE] = "Hello ioctl!\n";

long hello_unlocked_ioctl(struct file* file, unsigned int req,
		unsigned long param)
{
	int siz = strlen(hello_buf);
	char *buf = (char *) param;
	switch(req) {
		case HELLO :
			if (copy_to_user(buf,hello_buf,siz) >= siz)
			{
				pr_info("[helloioctl]\tcopy_to_user failed\n");
				return -ENOTTY;
			}
			break;
		case WHO :
			if (copy_from_user(hello_buf,buf,siz) >= siz)
			{
				pr_info("[helloioctl]\tcopy_from_user failed\n");
				return -ENOTTY;
			}
			break;
		default :
			pr_info("[helloioctl]\tundefined request\n");
			break;
	}
	return 0;
}

struct file_operations hello_ops = {
	.unlocked_ioctl = hello_unlocked_ioctl};

static int __init hello_init(void)
{
	dev_major = register_chrdev(dev_major,dev_name,&hello_ops);
	pr_info("Hello, world\n");
	return 0;
}
module_init(hello_init);

static void __exit hello_exit(void)
{
	unregister_chrdev(dev_major,dev_name);
	pr_info("Goodbye, cruel world\n");
}
module_exit(hello_exit);

