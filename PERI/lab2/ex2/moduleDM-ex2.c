#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charlie, 2015");
MODULE_DESCRIPTION("Ali Debbaghi - Kevin Mambu");

static int LED;
module_param(LED, int, 0);
MODULE_PARM_DESC(LED, "Nombre de led");

static int __init mon_module_init(void)
{
    printk(KERN_DEBUG "Hello World !\n");
    printk(KERN_DEBUG "LED=%d !\n", LED);
    return 0;
}
static void __exit mon_module_cleanup(void)
{
   printk(KERN_DEBUG "Goodbye World! <debbaghi-mambu>\n");
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);
