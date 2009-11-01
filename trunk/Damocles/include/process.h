/* process.h */

#ifndef PROCESS_H_
#define PROCESS_H_


#include "defs.h"

/* La cantidad máxima de procesos que soporta el sistema */
#define MAX_PROCESS 71


/* Opciones de WaitPid */
#define O_NOWAIT 0x1


typedef enum {KILLED, NORMAL} exitStatus_t;


typedef int(*process_t)(int, char**);

/* Crea un proceso */
int procCreate(char *name, process_t p, void *stack, void *heap,
		int fds[],int files, int argc, char **argv, int tty, int orphan, int priority);

void procSetup();

void procSaveStack(byte *stackPtr);

int procKill(int pid);

void procEnd(int retval);

int procSign(int pid, int signal); //TODO

int procRetVal(int pid, exitStatus_t *status, int *retVal);

void procEnableMem(int pid);

void procDisableMem(int pid);

void procReadyToRemove(int pid);

int procAttachedTTY(int pid);

int procGetFD(int fd);

int procGetPpid(int pid);

dword procGetStack(int pid);

int procWaitPid(int pid, exitStatus_t *status,int *retval, int option);

#endif /* PROCESS_H_ */
