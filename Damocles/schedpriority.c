/* schedpriority.c */

#include "include/process.h"
#include "include/sched.h"
#include "include/defs.h"
#include "include/string.h"
#include "include/mmu.h"
#include "include/io.h"
#include "include/sysasm.h"
#include "include/syslib.h"


#define CANT_PRIORITIES 4

typedef struct  process_scheduler_t{
	int pid;
	int priority;
	int waitingTicks;
	int ticks;
	char name[NAME_LENGTH];
	status_t status;
	struct process_scheduler_t *nextInPriority;
	struct process_scheduler_t *nextWaiting;
	struct process_scheduler_t *nextProcess;
}process_scheduler_t;


/* Los contenedores a los procesos */
static process_scheduler_t sched[MAX_PROCESS];

/* Los punteros a los procesos para accederlos rápidamente */
static process_scheduler_t *indexes[MAX_PROCESS];

static int processCant = 0;

static process_scheduler_t *currentProcess;
static process_scheduler_t *waitingList;
static process_scheduler_t *processList;
static process_scheduler_t *freeList;

static process_scheduler_t *priorities[CANT_PRIORITIES];


static process_scheduler_t *getEmptySlot();
static void addToIndexes(process_scheduler_t *slot);
static void addToFreeSlots(process_scheduler_t *current);
static process_scheduler_t *getByPid(int pid);
static void addToReadyList(process_scheduler_t *slot);
static void removeFromReadyList(process_scheduler_t *slot);
static void addToProcList(process_scheduler_t *slot);
static void removeFromProcList(process_scheduler_t *slot);
static process_scheduler_t *getPrevReadySlot(process_scheduler_t *slot);
static process_scheduler_t * getPrevProcSlot(process_scheduler_t *slot);


dword schedSchedule(){
	process_scheduler_t *current = currentProcess;
	process_scheduler_t *next;
	int priority = 0;
	int found = 0;


	/* Si el proceso actual estaba corriendo, es porque está
	 * listo
	 */
	if(current->status == RUNNING)
		current->status = READY;


	while(!found){
		/* Busco por prioridad el próximo proceso q ejecutar,
		 * si las listas estan vacias, no hay procesos para ejecutar */
		if(priorities[priority] != NULL){
			if(current->priority == priority){
				/* Si coincide el nivel, corresponde que ejecute le proceso
				 * siguente al que estaba ejecutando
				 */
				next = current->nextInPriority;

				/* Cambio el puntero al primer elemento para no generar
				 * inacición por misma prioridad
				 */
				priorities[priority] = next;
			}

			else
				/* Si no coincide el nivel, elijo el "siguiente" del nivel
				 * encontrado.
				 */
				next = priorities[priority];
			found = 1;
		}
		priority++;

		/* Al menos voy a encontrar el Iddle Process */
	}

	/* Si resuelta que el proceso nuevo es el mismo que el viejo, no es necesario
	 * cambiar el estado de la memoria
	 */
	if(current != next){
		procDisableMem(current->pid);
		procEnableMem(next->pid);
	}

	/* Preparo el desalodo del proceso muerto */
	if(current->status == DEAD){
		procReadyToRemove(current->pid);
		current->status = FREE;
		addToFreeSlots(current);
		processCant--;
	}

	currentProcess = next;
	currentProcess->ticks++;

	return procGetStack(currentProcess->pid);

}

int schedRemove(int pid){
	process_scheduler_t *slot = getByPid(pid);

	if(slot == NULL)
		return 0;

	removeFromReadyList(slot);
	removeFromProcList(slot);
	slot->status = DEAD;
	return 1;
}

int schedCantProcess(){
	return processCant;
}

int schedSetPriority(int pid, int priority){
	process_scheduler_t *slot = getByPid(pid);

	if(slot == NULL)
		return 0;

	removeFromReadyList(slot);

	slot->priority = priority;

	if(slot->status == READY || slot->status == RUNNING)
		addToReadyList(slot);

	return 1;

}

int schedCurrentProcess(){
	return currentProcess->pid;
}

void schedTicks(){
	/* No hay ningun proceso durmiendo, no hay nada que hacer */
	if(waitingList == NULL)
		return;


	/* Solo le resto al primero, porque el resto tienen offset de tiempo */
	if(--waitingList->waitingTicks == 0){
		/* Puede haber varios procesos que esten durmiendo la misma
		 * cantidad de tiempo
		 */
		while(waitingList != NULL && waitingList->waitingTicks == 0){
			process_scheduler_t *next;
			next = waitingList->nextWaiting;
			schedContinue(waitingList->pid);
			waitingList = next;
		}
	}

	return;
}

void schedSetUp(){
	int i;

	for(i = 0 ; i < MAX_PROCESS ; i++){
		sched[i].status = FREE;
		indexes[i] = NULL;
	}

	for( i = 0 ; i < CANT_PRIORITIES ; i++){
		priorities[i] = NULL;
	}
}

int schedAdd(int pid, char *name, int priority){
	process_scheduler_t *slot = getEmptySlot();

	if(slot == NULL)
		return 0;

	slot->pid = pid;
	slot->priority = priority;
	slot->nextInPriority = NULL;
	slot->nextWaiting = NULL;
	slot->status = BLOCKED;
	slot->waitingTicks = 0;
	strcpy(slot->name, name);

	processCant++;

	addToIndexes(slot);
	addToProcList(slot);
	return pid;
}


int schedGetPriority(int pid){
	process_scheduler_t *slot = getByPid(pid);

	if(slot == NULL)
		return -1;

	return slot->priority;
}

int schedContinue(int pid){
	process_scheduler_t *slot = getByPid(pid);

	if(slot == NULL && slot->status != BLOCKED)
		return 0;

	slot->status = READY;
	addToReadyList(slot);
	return 1;
}

int schedBlock(int pid){
	process_scheduler_t *slot = getByPid(pid);

	/* Si el proceso no está corriendo o no está listo, es un error */
	if(slot == NULL || !(slot->status == READY || slot->status == RUNNING))
		return 0;

	removeFromReadyList(slot);
	slot->status = BLOCKED;

	return 1;
}

void schedSetUpIdle(int pid){
	/* En esta versión, Iddle se configura pidiendo que sea
	 * de mínima prioridad
	 */
	return;
}

void schedSetUpInit(int pid, char *name, int priority){
	schedAdd(pid, name, priority);
}


void schedSleep(int milliseconds){
	int ticks = 18.2*((milliseconds+1)/1000) + 1;
	process_scheduler_t *next;
	int acum = 0;

	/* Si no lo paro al menos un tick, salgo */
	if(ticks < 1)
		return;

	removeFromReadyList(currentProcess);

	/* Si el proceso es uno solo, simplemente lo seteo en la lista */

	if(waitingList == NULL){
		waitingList = currentProcess;
		currentProcess->waitingTicks = ticks;
		currentProcess->nextWaiting = NULL;
		return;
	}

	next = waitingList;

	while(next->nextWaiting != NULL && acum < ticks){
		acum = next->waitingTicks;
		next = next->nextWaiting;
	}

	currentProcess->nextWaiting = next->nextWaiting;
	next->nextWaiting = currentProcess;

	currentProcess->waitingTicks = (ticks - acum);

	if(currentProcess->nextWaiting != NULL){
		/* Le saco los ticks al siguiente que le quedaron asignados a este */
	((process_scheduler_t	*)currentProcess->nextWaiting)->waitingTicks =
				(((process_scheduler_t *)currentProcess->nextWaiting)->waitingTicks - currentProcess->waitingTicks);
	}

	return;
}

int schedChangeStatus(int pid, status_t status){
	process_scheduler_t *slot = getByPid(pid);

	if(slot == NULL)
		return 0;

	switch(status){
		case BLOCKED:
			return schedBlock(pid);
			break;
		case READY: case RUNNING:
			return schedContinue(pid);
			break;
		default:
			return 0;
			break;
	}

}

void schedResetStatics(){
	process_scheduler_t *next;

	next = processList;

	while(next != NULL){
		next->ticks = 0;
		next = next->nextProcess;
	}

	return;
}


int schedGetInfo(schedProcData_t data[], int max){

	process_scheduler_t *next;
	int cant = 0;

	next = processList;

	for(cant = 0 ; cant < max && next != NULL; cant++){
		strcpy(data[cant].name, next->name);
		data[cant].pid = next->pid;
		data[cant].priority = next->priority;
		data[cant].status = next->status;
		data[cant].ticks = next->ticks;
		next = next->nextInPriority;
	}
	return cant;
}


static void addToIndexes(process_scheduler_t *slot){

	if(indexes[processCant-1] != NULL){
		/* PANIC */
		asm("cli; hlt");
	}

	indexes[processCant-1] = slot;
	return;

}

static void addToFreeSlots(process_scheduler_t *current){

	current->nextProcess = freeList;
	freeList = current;
}

static process_scheduler_t *getByPid(int pid){
   int low = 0;
   int mid;
   int high = processCant;

      while (low < high) {
          mid = low + ((high - low) / 2);
          if (indexes[mid]->pid < pid)
              low = mid + 1;
          else
               high = mid;
      }

      if ((low < processCant) && (indexes[low]->pid == pid))
          return indexes[low]; // found
      else
          return NULL; // not found
}


static void addToReadyList(process_scheduler_t *slot){

	process_scheduler_t *next;

	if(priorities[slot->priority] == NULL)
	{
		priorities[slot->priority] = slot;
		slot->nextInPriority = slot;
	}
	else
	{
		next = priorities[slot->priority];
		slot->nextInPriority = next->nextInPriority;
		next->nextInPriority = slot;
	}
	return;
}

static void removeFromReadyList(process_scheduler_t *slot){
	process_scheduler_t *prev;

	if(slot->nextInPriority == slot)
	{
		/* Este es el único elemento de la lista */
		priorities[slot->priority] = NULL;
	}
	else
	{
		prev = getPrevReadySlot(slot);
		prev->nextInPriority = slot->nextInPriority;
	}
	return;
}
static process_scheduler_t *getPrevReadySlot(process_scheduler_t *slot){

	process_scheduler_t *rta;

	rta = priorities[slot->priority];
	if(rta == NULL){
		/* PANIC */
		asm("cli; hlt;");
	}

	while(rta->nextInPriority != slot){
		rta = rta->nextInPriority;
	}

	return rta;
}

static void addToProcList(process_scheduler_t *slot){
	slot->nextProcess = processList;
	processList = slot;
	return;
}
static void removeFromProcList(process_scheduler_t *slot){
	process_scheduler_t *prev;

	prev = getPrevProcSlot(slot);
	prev->nextProcess = slot->nextProcess;
}

static process_scheduler_t *getEmptySlot(){
	process_scheduler_t *free;

	free = freeList;
	if(free != NULL)
		freeList = freeList->nextProcess;

	return free;
}

static process_scheduler_t * getPrevProcSlot(process_scheduler_t *slot){
	process_scheduler_t *rta = processList;

	if(rta == NULL){
		/* PANIC */
		asm("cli; hlt");
	}

	while(rta->nextProcess != slot){
		rta = rta->nextProcess;
	}

	return rta;
}
