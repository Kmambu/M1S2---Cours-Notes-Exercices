#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <mach/platform.h>

struct gpio_s
{
    uint32_t gpfsel[7];
    uint32_t gpset[3];
    uint32_t gpclr[3];
    uint32_t gplev[3];
    uint32_t gpeds[3];
    uint32_t gpren[3];
    uint32_t gpfen[3];
    uint32_t gphen[3];
    uint32_t gplen[3];
    uint32_t gparen[3];
    uint32_t gpafen[3];
    uint32_t gppud[1];
    uint32_t gppudclk[3];
    uint32_t test[1];
}
*gpio_regs = (struct gpio_s *)__io_address(GPIO_BASE);

#define DEV_NAME "ledbpDM"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kevin Mambu, Ali Debbaghi [2017]");
MODULE_DESCRIPTION("Driver module for a PushButton/LED Combo");

//Device : Major number
static int major;

//Number-of-Leds parameters
static int LED, BP;
module_param(LED, int, 0);
module_param(BP , int, 0);
MODULE_PARM_DESC(LED, "Pin de la LED");
MODULE_PARM_DESC(BP, "Pin du Bouton Poussoir");

#define NB_PORTS 13
static const int legal_ports[NB_PORTS] =
    {4,17,27,22,10,9,11,18,23,24,25,8,7};
int is_gpio_legal(int gpio)
{
    int i,legal=0;
    for(i=0; i<NB_PORTS && !legal; i++)
        if(gpio == legal_ports[i])
        {
            legal = 1;
            break;
        }
    return legal;
}

// gpio_fsel()
// selectionne un pin selon l'un des deux modes de fonctionements possibles :
// fun  = 0 : input
// fun != 0 : output
static void gpio_fsel(int pin, int fun)
{
    uint32_t reg = pin / 10;
    uint32_t bit = (pin % 10) * 3;
    uint32_t mask = 0b111 << bit;
    gpio_regs->gpfsel[reg] = (gpio_regs->gpfsel[reg] & ~mask) | ((fun << bit) & mask);
}

// gpio_write()
// met a jour la valeur du pin d'apres la valeur val :
// val = 1 : GPIO_SET
// val = 0 : GPIO_CLR
static void gpio_write(int pin, bool val)
{
    if (val)
        gpio_regs->gpset[pin / 32] = (1 << (pin % 32));
    else
        gpio_regs->gpclr[pin / 32] = (1 << (pin % 32));
}

static int gpio_read(int pin)
{
	return (gpio_regs->gplev[pin / 32] >> (pin % 32)) & 1;
}

static int 
open_ledbp(struct inode *inode, struct file *file) {
	if(LED <= 0) {
		printk(KERN_DEBUG "[ERROR @open()] : LED pin unspecified\n");
		return -1;
	} else if (!is_gpio_legal(LED)) {
		printk(KERN_DEBUG "[ERROR @open()] : LED pin non-legal\n");
		return -1;
	} 
	if(BP <= 0) {
		printk(KERN_DEBUG "[ERROR @open()] : BP pin unspecified\n");
		return -1;
	} else if (!is_gpio_legal(BP)) {
		printk(KERN_DEBUG "[ERROR @open()] : BP pin non-legal\n");
		return -1;
	}
	gpio_fsel(LED,1);
	gpio_fsel(BP, 0);
	printk(KERN_DEBUG "open()\n");
	return 0;
}

static ssize_t 
read_ledbp(struct file *file, char *buf, size_t count, loff_t *ppos) {
	int ret = gpio_read(BP);
	if (ret)
		buf[0] = '1';
	else
		buf[0] = '0';
	printk(KERN_DEBUG "read(%c)\n",buf[0]);
	return count;
}

static ssize_t 
write_ledbp(struct file *file, const char *buf, size_t count, loff_t *ppos) {
	if (buf[0] == '0')
		gpio_write(LED, 0);
	else
		gpio_write(LED, 1);
	printk(KERN_DEBUG "write(%c)\n",buf[0]);
	return count;
}

static int 
release_ledbp(struct inode *inode, struct file *file) {
	printk(KERN_DEBUG "close()\n");
	return 0;
}

struct file_operations fops_ledbp =
{
	.open       = open_ledbp,
	.read       = read_ledbp,
	.write      = write_ledbp,
	.release    = release_ledbp 
};

/*static struct timer_list led_blink_timer;
static int led_blink_period = 3000;

static void 
led_blink_handler(unsigned long unused)
{
    static bool on = false;
    on = !on;
    gpio_write(LED, on);
    mod_timer(&led_blink_timer, jiffies + msecs_to_jiffies(led_blink_period));
}*/

static int __init mon_module_init(void)
{
	//int result;
	major = register_chrdev(0, DEV_NAME, &fops_ledbp);
	printk(KERN_DEBUG "ledbpDM initialized\n");
	printk(KERN_DEBUG "LED=%d !\n", LED);
	//setup_timer(&led_blink_timer, led_blink_handler, 0);
	//result = mod_timer(&led_blink_timer, jiffies + msecs_to_jiffies(led_blink_period));
	//BUG_ON(result < 0);
	printk(KERN_DEBUG "Blink loaded!\n");
	return 0;
}

static void __exit mon_module_cleanup(void)
{
	unregister_chrdev(major, DEV_NAME);
	//del_timer(&led_blink_timer);
	printk(KERN_DEBUG "blink removed\n");
	printk(KERN_DEBUG "ledbpDM shut down <debbaghi-mambu>\n");
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);
