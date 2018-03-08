#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>

MODULE_DESCRIPTION("Module \"hello word param\" pour noyau linux");
MODULE_AUTHOR("Kevin Mambu");
MODULE_LICENSE("GPL");

static int howmany = 3;
static char *whom = "generic";
module_param(whom, charp, 0644);
EXPORT_SYMBOL_GPL(whom);
MODULE_PARM_DESC(whom,"The user name");
module_param(howmany, int, 0644);
MODULE_PARM_DESC(howmany,"The number of prints");

static int __init hello_init(void)
{
	int i = 0;
	for (i=0; i<howmany; i++)
		pr_info("(%d) Hello, %s\n", i, whom);
	return 0;
}
module_init(hello_init);

static void __exit hello_exit(void)
{
	pr_info("Goodbye, %s!\n",whom);
}
module_exit(hello_exit);

