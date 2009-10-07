#include "include/sys.h"
#include "include/sysasm.h"
#include "include/defs.h"
#include "include/io.h"

void *sysfRead(void **args);
void *sysfWrite(void **args);
void *sysfFlush(void **args);
void *sysfgetCursor(void **arg);
void *sysfsetCursor(void **args);


typedef void *(*sysfT)(void **);

sysfT syscalls[] = {
		sysfRead, sysfWrite, sysfFlush, NULL, NULL, /* 0 - 4 */
		NULL, NULL, NULL, NULL, NULL, /*  5- 9 */
		NULL, NULL, NULL, NULL, NULL, /* 10 - 14 */
		NULL, NULL, NULL, NULL, NULL, /* 15 - 19 */
		NULL, NULL, NULL, NULL, NULL, /* 20 - 24 */
		sysfsetCursor, sysfgetCursor, NULL, NULL, NULL /* 25 - 29 */
};

void *sysfgetCursor(void **args){
	sysgetCursor(args[0]);
	return NULL;
}

void *sysfsetCursor(void **args){
	syssetCursor(args[0]);
	return NULL;
}

void *sysfRead(void **args){
	sysread((int) args[0], args[1],(int)args[2]);
	return NULL;
}

void *sysfWrite(void **args){
	syswrite((int) args[0], args[1], (int)args[2]);
	return (void *)0;
}

void *sysfFlush(void **args){
	sysflush((int) args[0]);
	return NULL;
}


void *_dispatcher(int callnum, void **args){
	return syscalls[callnum](args);
}





