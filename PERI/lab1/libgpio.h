#ifndef _LIBGPIO_H
#define _LIBGPIO_H
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

/*
 * Base of peripherals and base of GPIO controller.
 */

#define BCM2835_PERIPH_BASE     0x20000000
#define BCM2835_GPIO_BASE       ( BCM2835_PERIPH_BASE + 0x200000 )

/*
 * Paging definitions.
 */

#define RPI_PAGE_SIZE           4096
#define RPI_BLOCK_SIZE          4096

/*
 * Helper macros for accessing GPIO registers.
 */

#define GPIO_CONF_FUNC_REG( ptr, gpio ) \
    *( (ptr) + ( (gpio) / 10 ) ) 

#define GPIO_CONF_FUNC_INPUT_MASK( gpio ) \
    ( ~( 0x7 << ( ( (gpio) % 10 ) * 3 ) ) ) 

#define GPIO_CONF_FUNC_OUTPUT_MASK( gpio ) \
    ( 0x1 << ( ( (gpio) % 10 ) * 3 ) ) 

#define GPIO_CONF_AS_INPUT( ptr, gpio ) \
    GPIO_CONF_FUNC_REG( ptr, gpio ) &= GPIO_CONF_FUNC_INPUT_MASK( gpio ) 

#define GPIO_CONF_AS_OUTPUT( ptr, gpio ) \
    do { \
        GPIO_CONF_AS_INPUT( ptr, gpio ); \
        GPIO_CONF_FUNC_REG( ptr, gpio ) |= \
            GPIO_CONF_FUNC_OUTPUT_MASK( gpio ); \
    } while ( 0 ) 

#define GPIO_CONF_REG( ptr, addr, gpio ) \
    *( (ptr) + ( ( (addr) / sizeof( uint32_t ) ) + ( (gpio) / 32 ) ) ) 

#define GPIO_SET_REG( ptr, gpio ) \
    GPIO_CONF_REG( ptr, 0x1c, gpio )

#define GPIO_CLR_REG( ptr, gpio ) \
    GPIO_CONF_REG( ptr, 0x28, gpio )

#define GPIO_SET( ptr, gpio ) \
    GPIO_SET_REG( ptr, gpio ) = 1 << ( (gpio) % 32 ) 

#define GPIO_CLR( ptr, gpio ) \
    GPIO_CLR_REG( ptr, gpio ) = 1 << ( (gpio) % 32 )

#define GPIO_VALUE( ptr, gpio ) \
    ( ( GPIO_CONF_REG( ptr, 0x34, gpio ) >> ( (gpio) % 32 ) ) & 0x1 ) 

/*
 * comparison macros available to lighten
 * the code
 *
 */
#define min( a, b ) ( a < b ? a : b )
#define max( a, b ) ( a < b ? b : a )

/*
 * Legal ports declarations :
 * only before mentioned ports are legal to use
 * a user may check the legality of a port using
 * is_gpio_legal()
 *
 */
#define NB_PORTS 13
static const int legal_ports[NB_PORTS] =
    {4,17,27,22,10,9,11,18,23,24,25,8,7};

/*
 * GPIO file descriptor and base address
 * WARN : for consistency issues, it is best to manipulate this address
 * using only below-mentioned methods
 */
static int mmap_fd;
static uint32_t volatile * gpio_base = 0;

int
setup_gpio_mmap ( uint32_t volatile ** ptr );
/*
 * maps the GPIO bay to the virtual memory
 * using /dev/mem, the virtual file assigned
 * to physical components
 *
 * takes :
 *      ptr, the address towards a pointer
 * returns :
 *      -1 if the memory mappping failed
 *      0 if no errors were encountered
 *
 */

void
teardown_gpio_mmap ( void * ptr );
/*
 * unmaps a memory segment from the virtual memory
 * takes :
 *      ptr, the pointer towards said segment
 *
 * returns :
 *      nothing
 *
 * WARN : because of the way the syscall munmap works,
 * using this method on a pointer not initialized by
 * above-mentioned setup_gpio_mmap may result on an 
 * unspecified behaviour
 *
 */

void
delay ( unsigned int milisec );
/*
 * delays an execution thread by a certain time
 * takes :
 *      milisec, said time in miliseconds
 * returns :
 *      nothing
 *
 */

int
gpio_init(void);
/*
 * setups the memory mapping of the GPIO bay using 
 * above-mentioned method setup_gpio_mmap
 *
 * takes :
 *      nothing
 * returns :
 *      -1 if the memory mappping failed
 *      0 if no errors were encountered
 *
 */

int
is_gpio_legal(int gpio);
/*
 * wether or not a gpio port is legal
 *
 * takes :
 *      the number of the GPIO port
 * returns :
 *      1 if the port is legal, 0 otherwise
 *
 */

int
gpio_setup (int gpio, int direction);
/*
 * setups a gpio port acording to a certain direction value :
 *
 * takes :
 *      gpio : the number of the gpio port
 *      direction :
 *          0 : to set as input
 *          1 : to set as output
 * returns :
 *      -1 if the port is not legal or the direction is 
 *          neither of before-mentioned options
 *      0 if no errors were encountered
 *
 */

int gpio_read (int gpio, int * val);
/*
 * reads a gpio port :
 *
 * takes :
 *      gpio : the number of the gpio port
 *      val : the address of the varable the value will be 
 *            stocked in
 * returns :
 *      -1 the port is not setup as an input
 *      0 if no errors were encountered
 *
 */

int gpio_write (int gpio, int val);
/*
 * writes a value `val` in a gpio port :
 *
 * takes :
 *      gpio : the number of the gpio port
 *      val : may only be equals either 0 or 1
 * returns :
 *      -1 the port is not setup as an output or said 
 *          conditions about val are not respected
 *      0 if no errors were encountered
 *
 */
#endif
