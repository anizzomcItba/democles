
#include "include/syscall.h"
#include "include/sysasm.h"
#include "drivers/video/crtc6845.h"


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

void getCursorCall(coord_t *t){
	syscall(SYS_GET_CURSOR, (dword)t,(dword) NULL,(dword) NULL,(dword) NULL,(dword) NULL);
}

void setCursorCall(coord_t *t){
	syscall(SYS_SET_CURSOR, (dword)t,(dword) NULL,(dword) NULL,(dword) NULL,(dword) NULL);
}
