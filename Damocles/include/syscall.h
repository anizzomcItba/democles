#ifndef __SYSCALL_H_
#define __SYSCALL_H_

#include "defs.h"
#include "sched.h"
#include "process.h"


#define SYS_READ 0
#define SYS_WRITE 1
#define SYS_FLUSH 2


#define SYS_STATICS 8
#define SYS_RUNNING 9
#define SYS_SLEEP 10



#define SYS_EXIT 11
#define SYS_GETPID 12
#define SYS_KILL 13
#define SYS_WAITPID 14
#define SYS_GETPPID 15

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

int running_process();

int running_statics(schedProcData_t data[], int max);

/* Process Syscalls */

int getpid();
int getppid();
void exit(int status);
int kill(int pid);
int waitpid(int pid, exitStatus_t *status, int *retval, int option);


dword syscall(dword sysnum, dword arg1, dword arg2, dword arg3,dword arg4,
		dword arg5);

#endif
