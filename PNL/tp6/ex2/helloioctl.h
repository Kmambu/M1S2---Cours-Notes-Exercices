#ifndef _HELLOIOCTL_H
#define _HELLOIOCTL_H
#include <sys/ioctl.h>

#define MAGICN 'N'
#define HELLO _IOR(MAGICN, 0, char *)

#endif
