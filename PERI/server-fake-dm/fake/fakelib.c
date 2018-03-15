#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "fakelib.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

int handleRequest(char *buf, char *ret) {
	switch(buf[0]) {
		case 'w' :
			return writeToLED(buf);
			break;
		case 'r' :
			return readFromBP(buf, ret);
			break;
		default :
			break;
	}
	return -2;
}

int writeToLED(char *buf) {
	int status, fd;
	char arg[64];
	char led[2];
	char val[2];
	sscanf(buf,"w %c%c %s\n", led,led+1,val);
	printf("%c\n", val[0]);
	strcpy(arg,"LED=");
	strcat(arg, led);
	printf("%s\n", arg);

	if(fork() == 0) {
		execl("./insdev.sh", "./insdev.sh", "ledBP_dm", arg, NULL);
	}
	wait(&status);
	// if(WEXITSTATUS(&status) == 1)
	// 	return -1;

	if ((fd = open("/dev/ledBP_dm", O_RDWR)) == -1)
		return -1;
	if (write(fd, val, 1) == -1)
		return -1;
	close(fd);

	if(fork() == 0) {
		execl("./rmdev.sh", "./rmdev.sh", "ledBP_dm", NULL);
	}
	wait(&status);
	// if(WEXITSTATUS(&status) == 1)
	// 	return -1;
	return 0;	
}

int readFromBP(char *buf, char *ret) {
	int status, fd;
	char arg[64];
	char bp[2];
	sscanf(buf,"r %c%c\n", bp,bp+1);
	strcpy(arg,"BP=");
	strcat(arg, bp);
	printf("%s\n", arg);

	if(fork() == 0) {
		execl("./insdev.sh", "./insdev.sh", "ledBP_dm", arg, NULL);
	}
	wait(&status);
	// if(WEXITSTATUS(&status) == 1)
	// 	return -1;

	if ((fd = open("/dev/ledBP_dm", O_RDWR)) == -1)
		return -1;
	if (read(fd, ret, 1) == -1)
		return -1;
	close(fd);

	if(fork() == 0) {
		execl("./rmdev.sh", "./rmdev.sh", "ledBP_dm", NULL);
	}
	wait(&status);
	// if(WEXITSTATUS(&status) == 1)
	// 	return -1;
	return 0;	
}
