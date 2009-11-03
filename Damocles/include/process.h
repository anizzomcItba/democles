/* process.h */

#ifndef PROCESS_H_
#define PROCESS_H_


#include "defs.h"

/* La cantidad máxima de procesos que soporta el sistema */
#define CONTEX_NAME_LENGTH 28
#define MAX_PROCESS 71
#define MAX_OPENFILES 10
#define MAX_PROCESS_ARGS 20

/* Opciones de WaitPid */
#define O_NOWAIT 0x1

typedef enum {KILLED, NORMAL} exitStatus_t;

typedef int(*process_t)(int, char**);



typedef struct process_contex_t *processApi_t;

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

processApi_t getContext(char *name, process_t p, int priority);

void contextAddFd(processApi_t context, int localfd, int globalfd);

void contextAddArg(processApi_t context, char *arg);

int contextCreate(processApi_t context);

void contextDestroy(processApi_t context);

void contextRemoveFd(processApi_t context, int fd);

#endif /* PROCESS_H_ */
