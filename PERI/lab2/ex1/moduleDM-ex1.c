#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charlie, 2015");
MODULE_DESCRIPTION("Ali Debbaghi - Kevin Mambu");

static int __init mon_module_init(void)
{
   printk(KERN_DEBUG "Hello World <debbaghi-mambu> !\n");
   return 0;
}

static void __exit mon_module_cleanup(void)
{
   printk(KERN_DEBUG "Goodbye World! <debbaghi-mambu>\n");
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);
