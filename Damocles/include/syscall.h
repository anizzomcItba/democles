#include "defs.h"


#ifndef __SYSCALL_H_
#define __SYSCALL_H_

#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_FLUSH 2

#define SYS_SET_CURSOR 25
#define SYS_GET_CURSOR 26


void write(int fd, void *buff, int qty);
void read(int fd, void *buff, int qty);
void flush(int fd);


dword syscall(dword sysnum, dword arg1, dword arg2, dword arg3,dword arg4,
		dword arg5);

#endif
