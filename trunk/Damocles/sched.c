#include "include/process.h"
#include "include/sched.h"
#include "include/defs.h"
#include "include/string.h"
#include "include/mmu.h"
#include "include/io.h"
#include "include/sysasm.h"
#include "include/syslib.h"

void breakpoint();

#define MAX_HEAPPAGES 10
#define MAX_STACKPAGES 1
#define MAX_OPENFILES 10
#define INIT_PROCESS 0
#define MAX_PROCESS_ARGS 20
#define IDLE_PROCCES 1

static int currentSlot;
static int processCant = 0;


typedef struct {
	int pid;
	char name[NAME_LENGTH];
	int priority;
	int waitingTicks;
	status_t status;
	int ticks;
	int nextSlot;
} procces_scheduler_t;


static procces_scheduler_t sched[MAX_PROCESS];

static int getFreeSlot();
static int findSlot(int pid);
static int findPrevSlot(int slot);

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
	sched[INIT_PROCESS].ticks = 0;
	processCant++;
}

void schedSetUpIdle(int pid, char *name, int priority){
	sched[IDLE_PROCCES].pid = pid;
	strcpy(sched[IDLE_PROCCES].name, name);
	sched[IDLE_PROCCES].status = READY;
	sched[IDLE_PROCCES].priority = priority;
	sched[IDLE_PROCCES].nextSlot = INIT_PROCESS;
	sched[IDLE_PROCCES].ticks = 0;
	processCant++;
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
	int newSlot, oldSlot;

	newSlot = oldSlot = currentSlot;

	/* Si el proceso estaba corriendo, es porque está listo para correr */
	if(sched[currentSlot].status == RUNNING)
		sched[currentSlot].status = READY;

	while(1){
		newSlot = sched[newSlot].nextSlot;
		if(sched[newSlot].status == READY){
			currentSlot = newSlot;
			break;
		}

		if(sched[newSlot].status == DEAD){
			/* Este proceso hay que desalojarlo del scheduler */
			int prevSlot = findPrevSlot(newSlot);
			sched[prevSlot].nextSlot = sched[newSlot].nextSlot;
			procReadyToRemove(sched[newSlot].pid);
			sched[newSlot].status = FREE;
			processCant--;
		}

		if(newSlot == oldSlot){
			currentSlot = IDLE_PROCCES;
			break;
		}
	}

	if(oldSlot != currentSlot){
		procDisableMem(sched[oldSlot].pid);
		procEnableMem(sched[newSlot].pid);
	}


	sched[currentSlot].ticks++;
	sched[currentSlot].status = RUNNING;
	return procGetStack(sched[currentSlot].pid);
}

int schedAdd(int pid,char *name, int priority){
	int slot = getFreeSlot();

	if(slot == -1)
		/* No encontré un slot libre retorno 0; */
		return 0;

	sched[slot].pid = pid;
	strcpy(sched[slot].name, name);
	sched[slot].priority = 0;

	/* Agrega al proceso a la siguiente posición del proc actual */
	sched[slot].nextSlot = sched[currentSlot].nextSlot;
	sched[currentSlot].nextSlot = slot;

	sched[slot].status = BLOCKED;

	schedResetStatics();
	processCant++;
	return 1;
}


void schedResetStatics(){
	int i;

	for(i = 0 ; i < MAX_PROCESS ; i++)
		sched[i].ticks = 0;

}

int schedGetInfo(schedProcData_t data[], int max){
	int i,j;

	/* Copio, mientras sea menor que max y no haya recorrido todos los
	 * procesos, al array pasado como argumento.
	 */

	for(j = 0, i = 0; j < MAX_PROCESS && i <= max; j++){
		if(sched[j].status != FREE){
			strcpy(data[i].name, sched[j].name);
			data[i].pid = sched[j].pid;
			data[i].priority = sched[j].priority;
			data[i].ticks = sched[j].ticks;
			data[i].status = sched[j].status;
			i++;
		}
	}

	return i;
}



/* Cambia el estado de un proceso,  si lo encuentra. Si no lo encuentra
 * retorna 0
 */
int schedChangeStatus(int pid, status_t status){
	int slot;

	if((slot = findSlot(pid)) == -1){
		return 0;
	}

	sched[slot].status = status;
	return 1;
}



int schedContinue(int pid){
	return schedChangeStatus(pid, READY);
}

int schedBlock(int pid){
	return schedChangeStatus(pid, BLOCKED);
}

int schedCurrentProcess(){
	return sched[currentSlot].pid;
}

/* Marca un proceso como muerto, no lo borra enseguida porque sinó
 * el scheduler si tiene otra cosa que hacer
 */
int schedRemove(int pid){
	int slot;


	if(pid == IDLE_PROCCES || pid == INIT_PROCESS)
		/* No se pueden desalojar estos procesos */
		return 0;

	slot = findSlot(pid);

	if(slot == -1)
		return -1;

	sched[slot].status = DEAD;
	return 1;
}


int schedSetPriority(int pid, int priority){
	int slot;

	if((slot = findSlot(pid)) == -1){
		return -1;
	}

	sched[slot].priority = priority;
	return sched[slot].priority;
}

int schedGetPriority(int pid){
	int slot = findSlot(pid);

	if(slot == -1)
		return -1;

	return sched[slot].priority;
}

void schedSleep(int milliseconds){
	if(milliseconds > 0) {
		sched[currentSlot].status = WAITING;
		sched[currentSlot].waitingTicks = 18.2*((milliseconds+1)/1000) + 1;
	}
	return;
}

int schedCantProcess(){
	return processCant;
}


static int getFreeSlot(){
	int i;
	for(i = 0 ; i < MAX_PROCESS ; i++){
		if(sched[i].status == FREE)
			return i;
	}

	return -1;
}


/* Busco al slot que tiene como siguiente slot al pasado como parámetro.
 * No debería llamarse con IDLE porque nunca se encontraría el slot previo
 */
static int findPrevSlot(int slot){
	int prevSlot = sched[slot].nextSlot;

	if(slot == IDLE_PROCCES)
		return -1;

	do {
		prevSlot = sched[prevSlot].nextSlot;
	} while(slot != sched[prevSlot].nextSlot);

	return prevSlot;
}

static int findSlot(int pid){
	int slot = currentSlot;

	while(sched[slot].pid != pid){
		slot = sched[slot].nextSlot;

		if(slot == currentSlot){
			/* Recorrí todos los elementos y no encontré el pid
			 * que buscaba */
			return -1;
		}
	}

	return slot;
}
