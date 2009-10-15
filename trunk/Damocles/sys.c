#include "include/sys.h"
#include "include/sysasm.h"
#include "include/sched.h"
#include "include/defs.h"
#include "include/io.h"

void *sysfRead(void **args);
void *sysfWrite(void **args);
void *sysfFlush(void **args);
void *sysfgetCursor(void **arg);
void *sysfsetCursor(void **args);
void *sysfSleep(void **args);

typedef void *(*sysfT)(void **);

sysfT syscalls[] = {
		sysfRead, sysfWrite, sysfFlush, NULL, NULL, /* 0 - 4 */
		NULL, NULL, NULL, NULL, NULL, /*  5- 9 */
		sysfSleep, NULL, NULL, NULL, NULL, /* 10 - 14 */
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


void *sysfSleep(void **args){
	/* Debe ser atómico porque cambiar de contexto mientras se está seteando
	 * un proceso como WAITING puede llegar a errores que marcan el tiempo de
	 * espera como negativo.
	 */
	_cli();
	schedSleep((int) args[0]);
	_sti();
	yield();
	return NULL;
}

void *_dispatcher(int callnum, void **args){
	return syscalls[callnum](args);
}





