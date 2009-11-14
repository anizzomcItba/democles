/*
 * sched.h
 *
 *  Created on: Oct 7, 2009
 */

#ifndef SCHED_H_
#define SCHED_H_

#define NAME_LENGTH 28

typedef enum {WAITING, BLOCKED, READY, RUNNING, DEAD, FREE, ZOMBIE} status_t;

typedef struct {
	int pid;
	char name[NAME_LENGTH];
	int priority;
	status_t status;
	int ticks;
} schedProcData_t;



/* Retorna el pid del proceso que está corriendo en este momento */

int schedCurrentProcess();

void schedSetUp();

void schedTicks();

/* Cambia el estado del proceso a WAITING la cantidad de milisegundos indicado
 * como parámetro. Los milisegundos van de a bloques de 55 cada uno.
 */
void schedSleep(int miliseconds);

int schedSetPriority(int pid, int priority);

int schedGetPriority(int pid);

/* Agrega el processo al scheduler */

int schedAdd(int pid, char *name, int priority);

/* Desaloja al proceso del scheduler */

int schedRemove(int pid);

int schedBlock(int pid);

int schedContinue(int pid);

int schedChangeStatus(int pid, status_t status);

void schedSetUpInit(int pid, char *name, int priroty);

void schedSetUpIdle(int pid/*, char *name, int priority*/);

/* Retorna la cantidad de procesos que hay en el scheduler */
int schedCantProcess();

/* Retorna la información de que procesos estan corriendo y cuantas veces recibieron
 * la cpu.Retorna en su nombre la cantidad de procesos copiados
 */
int schedGetInfo(schedProcData_t data[], int max);

void schedResetStatics();

#endif
