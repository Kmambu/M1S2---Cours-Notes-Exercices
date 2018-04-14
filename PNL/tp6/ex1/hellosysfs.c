#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/string.h>

#define HELLO_BUFSIZ 64
MODULE_DESCRIPTION("Module \"hello word\" dans le sysfs");
MODULE_AUTHOR("Kevin Mambu");
MODULE_LICENSE("GPL");

char hello_buf[HELLO_BUFSIZ] = "Hello sysfs!\n";

ssize_t hello_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	ssize_t ret = strlen(hello_buf);
	strncpy(buf,hello_buf,ret);
	return ret;
}

ssize_t hello_store(struct kobject *kobj, struct kobj_attribute *attr,
		const char *buf, size_t count)
{
	ssize_t ret;
	strncpy(hello_buf,buf,HELLO_BUFSIZ);
	ret = strlen(hello_buf);
	return ret;
}

static struct kobj_attribute hello = __ATTR_RW(hello);

static int __init hello_init(void)
{
	sysfs_create_file(kernel_kobj, &(hello.attr));
	return 0;
}
module_init(hello_init);

static void __exit hello_exit(void)
{
	sysfs_remove_file(kernel_kobj,&(hello.attr));
	pr_info("Goodbye, cruel world\n");
}
module_exit(hello_exit);

