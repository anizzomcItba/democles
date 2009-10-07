#include "include/sys.h"
#include "include/sysasm.h"
#include "include/defs.h"
#include "include/io.h"

void *sysfRead(void **args);
void *sysfWrite(void **args);
void *sysfFlush(void **args);

typedef void *(*sysfT)(void **);

sysfT syscalls[] = {
		sysfRead, sysfWrite, sysfFlush
};


void *sysfRead(void **args){
	sysread((int) args[0], args[1],(int)args[2]);
	return (void *)0;
}

void *sysfWrite(void **args){
	syswrite((int) args[0], args[1], (int)args[2]);
	return (void *)0;
}

void *sysfFlush(void **args){
	sysflush((int) args[0]);
	return (void*)0;
}


void *_dispatcher80(int callnum, void **args){
	return syscalls[callnum](args);
}





