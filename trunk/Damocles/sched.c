#include "include/process.h"
#include "include/sched.h"
#include "include/defs.h"
#include "include/string.h"
#include "include/mmu.h"
#include "include/io.h"
#include "include/sysasm.h"

#define KEYBOARD 8

#define NAME_LENGTH 30
#define MAX_HEAPPAGES 10
#define MAX_STACKPAGES 1
#define MAX_OPENFILES 10
#define INIT_PROCESS 0
#define MAX_PROCESS_ARGS 20
#define IDLE_PROCCES 1	//TODO: Cambiar al pid del proceso iddle

static int currentSlot;


typedef struct {
	int pid;
	char name[NAME_LENGTH];
	int priority;
	int waitingTicks;
	status_t status;
	int nextSlot;
} procces_scheduler_t;


static procces_scheduler_t sched[MAX_PROCESS];

static int getFreeSlot();



void schedSetUp(){
	int i;
	for (i = 0 ; i < MAX_PROCESS ; i++){
		sched[i].status = FREE;
	}
	return;
}


void schedSetUpInit(int pid, char *name, int priroty){
	sched[INIT_PROCESS].pid = pid;
	strcpy(sched[INIT_PROCESS].name, name);
	sched[INIT_PROCESS].status = RUNNING;
	sched[INIT_PROCESS].priority = priroty;
	sched[INIT_PROCESS].nextSlot = INIT_PROCESS;
}

void schedSetUpIdle(int pid, char *name, int priority){
	sched[IDLE_PROCCES].pid = pid;
	strcpy(sched[IDLE_PROCCES].name, name);
	sched[IDLE_PROCCES].status = READY;
	sched[IDLE_PROCCES].priority = priority;
	sched[IDLE_PROCCES].nextSlot = INIT_PROCESS;
}
/* Función que decrementa los ticks que se estan esperando para ejecutar
 * nuvamente.
 */

void schedTicks(){
	int slot, starting;

	/* Si el proceso actual es IDDLE, lo salteo */
	if(currentSlot == IDLE_PROCCES)
		slot = starting = sched[currentSlot].nextSlot;
	else
		slot = starting = currentSlot;

	do {
		/* Si el proceso está esperando, le decremento en 1 los ticks */
		if(sched[slot].status == WAITING){
			/* Si los Ticks llegaron a cero, es hora de despertar al proceso */
			if(!--sched[slot].waitingTicks)
				sched[slot].status = READY;
		}
		slot = sched[slot].nextSlot;
	}while(slot != starting);
}

dword schedSchedule(){
	//TODO: Esto debería correr sobre un stack temporal, al igual que
	//Ticks

	int slot, startingSlot;
	slot = startingSlot = currentSlot;

	/* Si el proceso estaba corriendo, es que está listo para
	 * volver a correr.
	 */
	if(sched[currentSlot].status == RUNNING)
		sched[currentSlot].status = READY;

	while(1){
		slot = sched[slot].nextSlot;

		if(sched[slot].status == READY){
			/* Encontre un proceso listo para correr, checkeo que no sea
			 * el mismo, si no lo es, desabilito/habilito las páginas de
			 * memoria */
			if (slot != currentSlot){
				//TODO: Disable old process memory
				currentSlot = slot;
				//TODO: Enable new process memory
			}
			return  procGetStack(sched[slot].pid);
		}

		if(slot == startingSlot){
			/* No hay nada que hacer, retorno al proceso idle */
			currentSlot = IDLE_PROCCES;
			return procGetStack(sched[currentSlot].pid);
		}

	}
}

void schedAdd(int pid,char *name, int priority){
	int slot = getFreeSlot();

	sched[slot].pid = pid;
	strcpy(sched[slot].name, name);
	sched[slot].priority = 0;

	/* Agrega al proceso a la siguiente posición del proc actual */
	sched[slot].nextSlot = sched[currentSlot].nextSlot;
	sched[currentSlot].nextSlot = slot;

	sched[slot].status = BLOCKED;
}

void schedChangeStatus(int pid, status_t status){
	int slot = currentSlot;

	while(sched[slot].pid != pid){
		slot = sched[slot].nextSlot;
		if(slot == currentSlot){
			/* Recorri la lista y no encontre el proceso */
			return;
		}
	}
	sched[slot].status = status;
	return;
}


int schedCurrentProcess(){
	return sched[currentSlot].pid;
}

void schedSleep(int milliseconds){
	if(milliseconds > 0) {
		sched[currentSlot].status = WAITING;
		sched[currentSlot].waitingTicks = 18.2*((milliseconds+1)/1000) + 1;
	}
	return;
}

static int getFreeSlot(){
	int i;
	for(i = 0 ; i < MAX_PROCESS ; i++){
		if(sched[i].status == FREE)
			return i;
	}

	return -1;
}
