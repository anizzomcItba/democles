
#include "syscall.h"
#include "sysasm.h"
#include "system/drivers/video/crtc6845.h"
#include "process.h"
#include "sched.h"




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

processApi_t getcontext(char *name, process_t p, int priority){
	return (processApi_t) syscall(SYS_CONTEXT, (dword) name, (dword) p, (dword) priority,(dword) NULL,(dword) NULL);
}

void contextaddfd(processApi_t context, int oldfd, int newfd){
	syscall(SYS_ADD_FD, (dword)context, (dword)oldfd,(dword) newfd, (dword)NULL, (dword)NULL);
}

void contextaddarg(processApi_t context, char *arg){
	syscall(SYS_ADD_ARG, (dword)context, (dword)arg, (dword)NULL,(dword) NULL, (dword)NULL);
}

void contexdestroy(processApi_t context){
	syscall(SYS_CONTX_DES, (dword)context,(dword) NULL,(dword) NULL, (dword)NULL, (dword)NULL);
}

void contexremovefd(processApi_t context, int fd){
	syscall(SYS_FD_REM,(dword) fd, (dword)NULL,(dword) NULL,(dword) NULL,(dword) NULL);
}

int conextstart(processApi_t context){
	return syscall(SYS_START_CON,(dword) context,(dword) NULL,(dword) NULL,(dword) NULL, (dword)NULL);
}
