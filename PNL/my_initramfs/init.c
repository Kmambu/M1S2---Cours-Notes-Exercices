#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
	write(STDERR_FILENO,"Hello!\n",8);
	sleep(5);
	return EXIT_SUCCESS;
}
