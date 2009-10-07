
#include "include/syscall.h"
#include "include/sysasm.h"


void write(int fd, void *buff, int qty){
	syscall(SYS_WRITE, (dword)fd,(dword) buff,(dword) qty,(dword) NULL,(dword) NULL);
}
void read(int fd, void *buff, int qty){
	syscall(SYS_READ, (dword)fd,(dword) buff,(dword) qty,(dword) NULL,(dword) NULL);
}
void flush(int fd){
	syscall(SYS_FLUSH, (dword)fd,(dword) NULL,(dword) NULL,(dword) NULL,(dword) NULL);
}

