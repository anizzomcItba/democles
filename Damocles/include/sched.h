/*
 * sched.h
 *
 *  Created on: Oct 7, 2009
 */

#ifndef SCHED_H_
#define SCHED_H_


typedef enum {WAITING, BLOCKED, READY, RUNNING, DEAD, FREE, ZOMBIE} status_t;

/* Retorna el indice de la tabla global de descriptores que tiene el proceso
 * que la llama.
 */
int schedGetGlobalFd(int fd);
/* Retorna la terminal a la cual está "atacheado" el proceso que la llama */
int schedAttachedTTY();

/* Retorna el pid del proceso que está corriendo en este momento */



int schedCurrentProcess();

void schedExitProcess();

int schedCreateProcess();

void schedSetUp();

void schedTicks();

void schedSleep(int miliseconds);

int schedSetPriority(int pid, int priority);

int schedGetPriority(int pid);

void schedAdd(int pid, char *name, int priority);

int schedRemove(int pid);

int schedBlock(int pid);

int schedContinue(int pid);

void schedChangeStatus(int pid, status_t status);

void schedSetUpInit(int pid, char *name, int priroty);

void schedSetUpIdle(int pid, char *name, int priroty);



#endif /* SCHED_H_ */
