#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "fakelib.h"

#define S2F "/tmp/s2f_dm"
#define LOG_IN "./log_in"
#define LOG_OUT "./log_out"
#define BUFSIZE 128

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(void)
{
	struct timeval tv;
	fd_set set;
	int s2f, log_in, log_out, count;
	float first_rand;
	float perc;
	char buf[BUFSIZE];

	if (access(S2F, F_OK) == -1) {
		if (mkfifo(S2F,0666) == -1)
			handle_error("creating s2f");
	}
	if ((s2f = open(S2F, O_RDONLY)) == -1) {
		handle_error("opening s2f");
	}
	if (access(LOG_IN, F_OK) == -1) {
		if (open(LOG_IN, O_CREAT|O_WRONLY, 0666) == -1)
			handle_error("creating log_in");
	}
	if ((log_in = open(LOG_IN, O_WRONLY)) == -1) {
		handle_error("opening log_in");
	}
	if (access(LOG_OUT, F_OK) == -1) {
		if (open(LOG_OUT, O_CREAT|O_RDONLY, 0666) == -1)
			handle_error("creating log_in");
	}
	if ((log_out = open(LOG_IN, O_WRONLY)) == -1) {
		handle_error("opening log_in");
	}

	srand(time(NULL));
	first_rand=rand()%1024;

	while (1)
	{
		tv.tv_sec = 1;
		tv.tv_usec= 0;
		FD_ZERO(&set);
		FD_SET(s2f, &set);
		if (select(s2f+1, &set, NULL, NULL, &tv) == -1)
			handle_error("select(...");
		if (FD_ISSET(s2f, &set)) {
			if((count = read(s2f,buf,BUFSIZE)) == -1)
				handle_error("reading in s2f");
			else if (count != 0) {
				if(write(log_out, buf, sizeof(buf)) == -1)
					handle_error("writing in log_out");
				printf("%s",buf);
			}
		}
		srand(time(NULL));
		perc=((rand()%100)/100.0);
		first_rand += (perc > 0.5 ? perc : -perc);
		snprintf(buf,BUFSIZE,"%ld;%.2f\n", time(NULL), first_rand);
		if(write(log_in,buf,strlen(buf)) == -1)
			handle_error("writing in log_in");
	}
	close(s2f);
	return EXIT_SUCCESS;
}
