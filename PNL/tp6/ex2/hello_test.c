#include "helloioctl.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#define BUFSIZE 64
#include "helloioctl.h"

int main(void)
{
	int hello;
	char buf[BUFSIZE];
	if ((hello = open("/dev/hello",O_RDWR)) == -1) {
		printf("cannot open hello\n");
		return EXIT_FAILURE;
	}
	if (ioctl(hello, HELLO, (unsigned long) buf) < 0) {
		printf("Error in sending request\n");
		return EXIT_FAILURE;
	}
	printf("%s", buf);
	strncpy(buf,"Hold my beer\n",BUFSIZE);
	if (ioctl(hello, WHO, (unsigned long) buf) < 0) {
		printf("Error in sending request\n");
		return EXIT_FAILURE;
	}
	if (ioctl(hello, HELLO, (unsigned long) buf) < 0) {
		printf("Error in sending request\n");
		return EXIT_FAILURE;
	}
	printf("%s", buf);
	close(hello);
	return EXIT_SUCCESS;
}
