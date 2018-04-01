#include "libgpio.h"

int setup_gpio_mmap ( uint32_t volatile ** ptr )
{
    void * mmap_result;

    mmap_fd = open ( "/dev/mem", O_RDWR | O_SYNC );

    if ( mmap_fd < 0 ) {
        return -1;
    }

    mmap_result = mmap (
        NULL
      , RPI_BLOCK_SIZE
      , PROT_READ | PROT_WRITE
      , MAP_SHARED
      , mmap_fd
      , BCM2835_GPIO_BASE );

    if ( mmap_result == MAP_FAILED ) {
        close ( mmap_fd );
        return -1;
    }

    *ptr = ( uint32_t volatile * ) mmap_result;

    return 0;
}

void teardown_gpio_mmap ( void * ptr )
{
    munmap ( ptr, RPI_BLOCK_SIZE );
    return;
}

void delay ( unsigned int milisec )
{
    struct timespec ts, dummy;
    ts.tv_sec  = ( time_t ) milisec / 1000;
    ts.tv_nsec = ( long ) ( milisec % 1000 ) * 1000000;
    nanosleep ( &ts, &dummy );
}

#define min( a, b ) ( a < b ? a : b )
#define max( a, b ) ( a < b ? b : a )
#define NB_PORTS 13

int gpio_init(void)
{
    return setup_gpio_mmap(&gpio_base);
}

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
int gpio_setup (int gpio, int direction)
{
    if(!is_gpio_legal(gpio))
    {
        printf("gpio_setup : gpio port is not legal");
        return -1;
    }

    if(direction == 0)
    {
        GPIO_CONF_AS_INPUT(gpio_base,gpio);
        return 0;
    }
    if(direction == 1)
    {
        GPIO_CONF_AS_OUTPUT(gpio_base,gpio);
        return 0;
    }
    printf("gpio_setup : invalid direction (either 0 or 1)");
    return -1;
}

int gpio_read (int gpio, int * val)
{
    if( (GPIO_CONF_FUNC_REG(gpio_base,gpio) &
        GPIO_CONF_FUNC_INPUT_MASK(gpio)) == 0) 
    {
        printf("gpio_read : GPIO %d not configured as input\n",gpio);
        return -1;
    }
    *val = GPIO_VALUE(gpio_base,gpio);
    return 0;

}

int gpio_write (int gpio, int val)
{
    if( (GPIO_CONF_FUNC_REG(gpio_base,gpio) &
        GPIO_CONF_FUNC_OUTPUT_MASK(gpio)) == 0) 
    {
        printf("gpio_write : GPIO %d not configured as output\n",gpio);
        return -1;
    }
    if(val == 0)
    {
        GPIO_CLR(gpio_base,gpio);
        return 0;
    }
    if(val == 1)
    {
        GPIO_SET(gpio_base,gpio);
        return 0;
    }
    printf("gpio_write : invalid value (either 0 or 1)");
    return -1;
}
