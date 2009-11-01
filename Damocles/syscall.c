
#include "include/syscall.h"
#include "include/sysasm.h"
#include "drivers/video/crtc6845.h"
#include "include/process.h"
#include "include/sched.h"


/* System calls de Archivos */

void write(int fd, void *buff, int qty){
	/* Defined in sys.asm */
	syscall(SYS_WRITE, (dword)fd,(dword) buff,(dword) qty,(dword) NULL,(dword) NULL);
}
void read(int fd, void *buff, int qty){
	/* Defined in sys.asm */
	syscall(SYS_READ, (dword)fd,(dword) buff,(dword) qty,(dword) NULL,(dword) NULL);
}
void flush(int fd){
	/* Defined in sys.asm */
	syscall(SYS_FLUSH, (dword)fd,(dword) NULL,(dword) NULL,(dword) NULL,(dword) NULL);
}

/* System call de video */

void getCursorCall(coord_t *t){
	syscall(SYS_GET_CURSOR, (dword)t,(dword) NULL,(dword) NULL,(dword) NULL,(dword) NULL);
}

void setCursorCall(coord_t *t){
	syscall(SYS_SET_CURSOR, (dword)t,(dword) NULL,(dword) NULL,(dword) NULL,(dword) NULL);
}

void clearScreenCall(){
	syscall(SYS_CLEARSCREEN, (dword)NULL,(dword) NULL,(dword) NULL,(dword) NULL,(dword) NULL);
}

/* System de Scheduler */
void sleep(int miliseconds){
	syscall(SYS_SLEEP,(dword) miliseconds,(dword) NULL,(dword) NULL,(dword) NULL,(dword) NULL);
}

/* Sycall de procesos */

void exit(int status){
	syscall(SYS_EXIT, (dword)status,(dword) NULL,(dword) NULL,(dword) NULL,(dword) NULL);
}

int kill(int pid){
	return syscall(SYS_KILL, (dword)pid, (dword) NULL,(dword) NULL,(dword) NULL,(dword) NULL);
}

int waitpid(int pid, exitStatus_t *status, int *retval, int option){
	return syscall(SYS_WAITPID, (dword)pid, (dword) status,(dword) retval,(dword) option,(dword) NULL);
}

int getppid(){
	return syscall(SYS_GETPPID, (dword)NULL,(dword) NULL,(dword) NULL,(dword) NULL,(dword) NULL);
}

int getpid(){
	return syscall(SYS_GETPID, (dword)NULL,(dword) NULL,(dword) NULL,(dword) NULL,(dword) NULL);
}

int running_process(){
	return syscall(SYS_RUNNING, (dword)NULL,(dword) NULL,(dword) NULL,(dword) NULL,(dword) NULL);
}

int running_statics(schedProcData_t data[], int max){
	return syscall(SYS_STATICS, (dword) data, (dword) max, (dword) NULL,(dword) NULL,(dword) NULL);
}

