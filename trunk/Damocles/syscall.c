
#include "include/syscall.h"
#include "include/sysasm.h"
#include "drivers/video/crtc6845.h"


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

/* System call de procesos */
void sleep(int miliseconds){
	syscall(SYS_SLEEP,(dword) miliseconds,(dword) NULL,(dword) NULL,(dword) NULL,(dword) NULL);
}



void exit(int status){
	syscall(SYS_EXIT, (dword)status,(dword) NULL,(dword) NULL,(dword) NULL,(dword) NULL);
}

int getpid(){
	return syscall(SYS_GETPID, (dword)NULL,(dword) NULL,(dword) NULL,(dword) NULL,(dword) NULL);
}
