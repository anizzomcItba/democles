/* process.h */

#ifndef PROCESS_H_
#define PROCESS_H_


#include "defs.h"

/* La cantidad máxima de procesos que soporta el sistema */
#define MAX_PROCESS 71


typedef int(*process_t)(int, char**);

/* Crea un proceso */
int procCreate(char *name, process_t p, void *stack, void *heap,
		int fds[],int files, int argc, char **argv, int tty, int orphan, int priority);

void procSetup();

void procSaveStack(byte *stackPtr);

int procKill(int pid); //TODO

void procEnd(int retval);

int procSign(int pid, int signal); //TODO

int procRetVal(int pid);

void procEnableMem(int pid);

void procDisableMem(int pid);

void procReadyToRemove(int pid);

int procAttachedTTY(int pid);

int procGetFD(int fd);

dword procGetStack(int pid);

#endif /* PROCESS_H_ */
