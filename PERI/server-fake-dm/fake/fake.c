#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "fakelib.h"

#define S2F "/tmp/s2f_dm"
#define F2S "/tmp/f2s_dm"

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(void)
{
	fd_set set;
	int s2f, f2s, count;
	char buf[64];
	char ret[64];

	if (access(S2F, F_OK) == -1) {
		if (mkfifo(S2F,0666) == -1)
			handle_error("creating s2f");
	}
	if ((s2f = open(S2F, O_RDONLY)) == -1) {
		handle_error("opening s2f");
	}
	if (access(F2S, F_OK) == -1) {
		if (mkfifo(F2S,0666) == -1)
			handle_error("creating f2s");
	}
	if ((f2s = open(F2S, O_WRONLY)) == -1) {
		handle_error("opening f2s");
	}


	while (1)
	{
		FD_ZERO(&set);
		FD_SET(s2f, &set);
		strcpy(ret, "");
		if (select(s2f+1, &set, NULL, NULL, NULL) == -1)
			handle_error("select(...");
		if (FD_ISSET(s2f, &set)) {
			if ((count = read(s2f, buf, sizeof(buf))) == -1)
				handle_error("reading from s2f");
			else if (count == 0)
				continue;
			printf("received %s", buf);
			if ((count = handleRequest(buf, ret)) == -2)
				handle_error("invalid request");
			else if (count == -1)
				handle_error("error while handling request");
			strcpy(buf, ret);
			strcat(buf,"\n");
			printf("ret = %s", buf);
			if (write(f2s, buf, strlen(buf)) == -1)
				handle_error("writing to f2s");
		}
		else {
			// We never come to this condition :D
			printf("Nothing to read\n");
		}
	}
	close(s2f);
	close(f2s);
	return EXIT_SUCCESS;
}
