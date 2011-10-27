#include "sys.h"
#include "sysasm.h"
#include "sched.h"
#include "defs.h"
#include "io.h"
#include "process.h"
#include "syslib.h"

void *sysfRead(void **args);
void *sysfWrite(void **args);
void *sysfFlush(void **args);
void *sysfgetCursor(void **arg);
void *sysfsetCursor(void **args);
void *sysfclearScreen(void **args);
void *sysfSleep(void **args);
void *sysfExit(void **args);
void *sysfgetPid(void **args);
void *sysfgetPpid(void **args);
void *sysfKill(void **args);
void *sysfwaitpid(void **args);
void *sysfrunning(void **args);
void *sysfstatics(void **args);

void *sysfgetcontext(void **args);
void *sysfcontextaddfd(void **args);
void *sysfcontextaddarg(void **args);
void *sysfcontextdestroy(void **args);
void *sysfcontextremovefd(void **args);
void *sysfcontexstart(void **args);

typedef void *(*sysfT)(void **);

sysfT syscalls[] = {
		sysfRead, sysfWrite, sysfFlush, NULL, NULL, /* 0 - 4 */
		NULL, NULL, NULL, sysfstatics, sysfrunning, /*  5- 9 */
		sysfSleep, sysfExit, sysfgetPid, sysfKill, sysfwaitpid, /* 10 - 14 */
		sysfgetPpid, sysfgetcontext, sysfcontextaddarg, sysfcontextaddfd, sysfcontextremovefd, /* 15 - 19 */
		sysfcontextdestroy, sysfcontexstart, NULL, NULL, NULL, /* 20 - 24 */
		sysfsetCursor, sysfgetCursor, sysfclearScreen, NULL, NULL /* 25 - 29 */
};


/* Podría ser un mutex */
void *sysfgetcontext(void **args){
	int f = disableInts();
	void *rta = (void *)getContext((char *) args[0], (process_t) args[1], (int) args[3]);
	restoreInts(f);
	return rta;
}

void *sysfcontextaddfd(void **args){
	contextAddFd(args[0], (int) args[1],(int) args[2]);
	return NULL;
}
void *sysfcontextaddarg(void **args){
	contextAddArg(args[0], args[1]);
	return NULL;
}

void *sysfcontextdestroy(void **args){
	int f = disableInts();
	contextDestroy(args[0]);
	restoreInts(f);
	return NULL;
}
void *sysfcontextremovefd(void **args){
	contextRemoveFd(args[0], (int)args[1]);
	return NULL;
}
void *sysfcontexstart(void **args){
	int f = disableInts();
	void *rta = (void *)contextCreate(args[0]);
	restoreInts(f);
	return rta;
}



void *sysfstatics(void **args){
	int f = disableInts();
	void *ret = (void *) schedGetInfo((schedProcData_t *) args[0], (int) args[1]);
	restoreInts(f);
	return ret;
}

void *sysfwaitpid(void **args){
	int f = disableInts();
	void *ret;
	ret = (void *)procWaitPid((int) args[0], (exitStatus_t*) args[1], (int*) args[2], (int) args[3]);
	restoreInts(f);
	return ret;
}

void *sysfKill(void **args){
	int f = disableInts();
	void *ret;
	ret = (void *)procKill((int) args[0]);
	restoreInts(f);
	return ret;
}

void *sysfgetCursor(void **args){
	sysgetCursor(args[0]);
	return NULL;
}

void *sysfsetCursor(void **args){
	syssetCursor(args[0]);
	return NULL;
}

void *sysfclearScreen(void **args){
	_vresetpage(procAttachedTTY(schedCurrentProcess()));
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

void *sysfExit(void **args){
	int f = disableInts();
	procEnd((int) args[0]);
	restoreInts(f);
	yield();
	/* Nunca se va llegar a ejecutar este punto puesto que el scheduler
	 * no va a pasar por acá */
	return NULL;
}


void *sysfSleep(void **args){
	/* Debe ser atómico porque cambiar de contexto mientras se está seteando
	 * un proceso como WAITING puede llegar a errores que marcan el tiempo de
	 * espera como negativo.
	 */
	int f = disableInts();
	schedSleep((int) args[0]);
	restoreInts(f);
	yield();
	return NULL;
}

void *sysfgetPpid(void **args){
	return (void* ) procGetPpid(schedCurrentProcess());
}



void *sysfgetPid(void **args){
	return (void *)schedCurrentProcess();
}

void *_dispatcher(int callnum, void **args){
	return syscalls[callnum](args);
}


void *sysfrunning(void **args){
	int f = disableInts();
	void *ret =(void *) schedCantProcess();
	restoreInts(f);
	return ret;
}




