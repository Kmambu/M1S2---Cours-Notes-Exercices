#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <mach/platform.h>
#include <asm/delay.h>


/*******************************************************************************
 * GPIO Pins
 ******************************************************************************/
#define RS 7
#define E  27
#define D4 22
#define D5 23
#define D6 24
#define D7 25

#define GPIO_INPUT  0
#define GPIO_OUTPUT 1

#define RPI_BLOCK_SIZE  0xB4
#define RPI_GPIO_BASE   0x20200000

/*******************************************************************************
 * LCD's Instructions ( source = doc )
 ******************************************************************************/

#define B_0000  0
#define B_0001  1
#define B_0010  2
#define B_0011  3
#define B_0100  4
#define B_0101  5
#define B_0110  6
#define B_0111  7
#define B_1000  8
#define B_1001  9
#define B_1010  a
#define B_1011  b
#define B_1100  c
#define B_1101  d
#define B_1110  e
#define B_1111  f

#define __HEX(h,l) 0x##h##l
#define _HEX(h,l)  __HEX(h,l)
#define HEX(h,l)   _HEX(B_##h,B_##l)

#define BIT(b,n)   (((n)>>(b))&1)

/* commands */
#define LCD_CLEARDISPLAY        HEX(0000,0001)
#define LCD_RETURNHOME          HEX(0000,0010)
#define LCD_ENTRYMODESET        HEX(0000,0100)
#define LCD_DISPLAYCONTROL      HEX(0000,1000)
#define LCD_CURSORSHIFT         HEX(0001,0000)
#define LCD_FUNCTIONSET         HEX(0010,0000)
#define LCD_SETCGRAMADDR        HEX(0100,0000)
#define LCD_SETDDRAMADDR        HEX(1000,0000)

/* flags for display entry mode : combine with LCD_ENTRYMODESET */
#define LCD_EM_RIGHT            HEX(0000,0000)
#define LCD_EM_LEFT             HEX(0000,0010)
#define LCD_EM_DISPLAYSHIFT     HEX(0000,0001)
#define LCD_EM_DISPLAYNOSHIFT   HEX(0000,0000)

/* flags for display on/off control : combine with LCD_DISPLAYCONTROL */
#define LCD_DC_DISPLAYON        HEX(0000,0100)
#define LCD_DC_DISPLAYOFF       HEX(0000,0000)
#define LCD_DC_CURSORON         HEX(0000,0010)
#define LCD_DC_CURSOROFF        HEX(0000,0000)
#define LCD_DC_BLINKON          HEX(0000,0001)
#define LCD_DC_BLINKOFF         HEX(0000,0000)

/* flags for display/cursor shift : combine with LCD_CURSORSHIFT */
#define LCD_CS_DISPLAYMOVE      HEX(0000,1000)
#define LCD_CS_CURSORMOVE       HEX(0000,0000)
#define LCD_CS_MOVERIGHT        HEX(0000,0100)
#define LCD_CS_MOVELEFT         HEX(0000,0000)

/* flags for function set : combine with LCD_FUNCTIONSET */
#define LCD_FS_8BITMODE         HEX(0001,0000)
#define LCD_FS_4BITMODE         HEX(0000,0000)
#define LCD_FS_2LINE            HEX(0000,1000)
#define LCD_FS_1LINE            HEX(0000,0000)
#define LCD_FS_5x10DOTS         HEX(0000,0100)
#define LCD_FS_5x8DOTS          HEX(0000,0000)

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
} *gpio_regs= (struct gpio_s *)__io_address(GPIO_BASE);

const int ddram[4] = {0x00, 0x40, 0x14, 0x54};

#define DEV_NAME "lcdDM"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kevin Mambu, Ali Debbaghi [2017]");
MODULE_DESCRIPTION("Driver module for a LCD ");

//Device : Major number
static int major;


// gpio_fsel()
// selectionne un pin selon l'un des deux modes de fonctionements possibles :
// fun  = 0 : input
// fun != 0 : output

void gpio_config(int gpio, int value)
{
  gpio_regs->gpfsel[gpio/10] = ( gpio_regs->gpfsel[gpio/10] & ~(0x7 << ((gpio % 10) * 3)) )
                             | (value << ((gpio % 10) * 3));
}


// gpio_write()
// met a jour la valeur du pin d'apres la valeur val :
// val = 1 : GPIO_SET
// val = 0 : GPIO_CLR

void gpio_update(int gpio, int value)
{
  if( value )
    gpio_regs->gpset[gpio/32] = (0x1 << (gpio % 32) );
  else
    gpio_regs->gpclr[gpio/32] = (0x1 << (gpio % 32) );
}

static int gpio_read(int pin)
{
	return (gpio_regs->gplev[pin / 32] >> (pin % 32)) & 1;
}

/*void  gpio_teardown ( void ){
  munmap((void*)gpio_regs, RPI_BLOCK_SIZE);
}*/

/* generate E signal */
void lcd_strobe(void)
{
  gpio_update(E,1);
  udelay(1);
  gpio_update(E,0);
}

/* send 4bits to LCD */
void lcd_write4bits(int data)
{
  /* first 4 bits */
  gpio_update(D7, BIT(7,data));
  gpio_update(D6, BIT(6,data));
  gpio_update(D5, BIT(5,data));
  gpio_update(D4, BIT(4,data));
  lcd_strobe();
  gpio_update(D7, BIT(3,data));
  gpio_update(D6, BIT(2,data));
  gpio_update(D5, BIT(1,data));
  gpio_update(D4, BIT(0,data));
  lcd_strobe();
  udelay(50); /* le délai minimum est de 37us */
}

void lcd_command(int cmd)
{
  gpio_update(RS, 0);
  lcd_write4bits(cmd);
  udelay(2000); /* delai nécessaire pour certaines commandes */
}

void lcd_init(void)
{
  gpio_update(E, 0);
  lcd_command(HEX(0011,0011)); /* initialization */
  lcd_command(HEX(0011,0010)); /* initialization */
  lcd_command(LCD_FUNCTIONSET    | LCD_FS_4BITMODE | LCD_FS_2LINE | LCD_FS_5x8DOTS );
  lcd_command(LCD_DISPLAYCONTROL | LCD_DC_DISPLAYON | LCD_DC_CURSOROFF );
  lcd_command(LCD_ENTRYMODESET   | LCD_EM_RIGHT | LCD_EM_DISPLAYNOSHIFT );
}

void lcd_clear(void)
{
  lcd_command(LCD_CLEARDISPLAY);
  lcd_command(LCD_RETURNHOME);
}

void lcd_data(int character)
{
  gpio_update(RS, 1);
  lcd_write4bits(character);
}

void lcd_set_cursor(int l, int c)
{
	lcd_command(LCD_SETDDRAMADDR + ddram[l%4] + c%0x14);
}

void lcd_message(const char* txt)
{
  int i, l = 0;
  for(i=0; i<strlen(txt) && txt[i] != '\n'; i++){
  	if(i%20 == 0 && i != 0) {
  		lcd_set_cursor(++l, 0);
  	}	
    lcd_data(txt[i]);
  }
}

static int 
open_lcd(struct inode *inode, struct file *file) {

	printk(KERN_DEBUG "open()\n");
	return 0;
}


static ssize_t 
write_lcd(struct file *file, const char *buf, size_t count, loff_t *ppos) {
	lcd_message(buf);
	printk(KERN_DEBUG "write(%s)\n", buf);
	//printk(KERN_DEBUG "write(%c)\n",buf[0]);
	return count;
}

static int 
release_lcd(struct inode *inode, struct file *file) {
	printk(KERN_DEBUG "close()\n");
	return 0;
}

struct file_operations fops_lcd =
{
	.open       = open_lcd,
	.write      = write_lcd,
	.release    = release_lcd 
};


static int __init mon_module_init(void)
{
	major = register_chrdev(0, DEV_NAME, &fops_lcd);
	printk(KERN_DEBUG "lcdDM initialized\n");
	/* Setting up GPIOs to output */
	  gpio_config(RS, GPIO_OUTPUT);
	  gpio_config(E , GPIO_OUTPUT);
	  gpio_config(D4, GPIO_OUTPUT);
	  gpio_config(D5, GPIO_OUTPUT);
	  gpio_config(D6, GPIO_OUTPUT);
	  gpio_config(D7, GPIO_OUTPUT);
          lcd_init();
          lcd_clear();
	return 0;
}

static void __exit mon_module_cleanup(void)
{
	unregister_chrdev(major, DEV_NAME);
	printk(KERN_DEBUG "lcdDM shut down <debbaghi-mambu>\n");
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);
