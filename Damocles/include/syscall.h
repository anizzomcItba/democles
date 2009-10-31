#include "defs.h"



#ifndef __SYSCALL_H_
#define __SYSCALL_H_

#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_FLUSH 2

#define SYS_SLEEP 10
#define SYS_EXIT 11
#define SYS_GETPID 12

#define SYS_SET_CURSOR 25
#define SYS_GET_CURSOR 26
#define SYS_CLEARSCREEN 27


/* IO Syscalls */
void write(int fd, void *buff, int qty);
void read(int fd, void *buff, int qty);
void flush(int fd);


/* Video Syscalls */

void getCursorCall(coord_t *t);
void setCursorCall(coord_t *t);
void clearScreenCall();



/* Scheduler Syscalls */

void sleep(int milliseconds);




/* Process Syscalls */

int getpid();
void exit(int status);


dword syscall(dword sysnum, dword arg1, dword arg2, dword arg3,dword arg4,
		dword arg5);

#endif
