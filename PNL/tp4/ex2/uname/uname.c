#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/utsname.h>

MODULE_DESCRIPTION("Changes th name of the kernel for the one passed \
		in parameter");
MODULE_AUTHOR("Kevin Mambu");
MODULE_LICENSE("GPL");

extern struct uts_namespace init_uts_ns;
static char *name = "Linux-Foo";
static char prev[__NEW_UTS_LEN + 1];
static int i;
module_param(name, charp, 0);
EXPORT_SYMBOL_GPL(name);
MODULE_PARM_DESC(name,"The name of substitution");

static int __init hello_init(void)
{
	for(i=0; i<__NEW_UTS_LEN+1; i++)
		prev[i] = init_uts_ns.name.sysname[i];
	for(i=0; name[i] != '\0'; i++)
		init_uts_ns.name.sysname[i] = name[i];
	return 0;
}
module_init(hello_init);

static void __exit hello_exit(void)
{
	for(i=0; i<__NEW_UTS_LEN+1; i++)
		init_uts_ns.name.sysname[i] = prev[i];
}
module_exit(hello_exit);

