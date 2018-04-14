#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(void)
{
    int fd = open("/dev/ledbpDM", O_RDWR, 0644); //O644: en lecture et en écriture
    char buf;
    while(1)
    {
        if(read(fd, &buf, 1)) {
            write(fd,&buf, 1);
            printf("wrote %c\n", buf);
        }
       
    }
}
