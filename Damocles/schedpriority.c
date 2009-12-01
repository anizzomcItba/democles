/* schedpriority.c */

//#define DEBUG

#include "include/process.h"
#include "include/sched.h"

#ifndef DEBUG
#include "include/defs.h"
#include "include/string.h"
#include "include/mmu.h"
#include "include/io.h"
#include "include/sysasm.h"
#include "include/syslib.h"

#else

#include <string.h>
#include <stdio.h>
#include <time.h>

#define uprintf printf

void procDisableMem(int pid){
	//printf("Memoria del proceso %d desabilitada\n", pid);
}

void procEnableMem(int pid){
	//printf("Memoria del proceso %d habilitada\n", pid);
}

void procReadyToRemove(int pid){
	printf("Proceso %d listo para remover\n", pid);
}


dword procGetStack(int pid){
	printf("Retornado el stack del proceso %d\n", pid);
}

#endif



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


dword schedSchedule();

/* Los contenedores a los procesos */
static process_scheduler_t sched[MAX_PROCESS];

/* Los punteros a los procesos para accederlos rápidamente */
static process_scheduler_t *indexes[MAX_PROCESS];

static int processCant = 0;

static process_scheduler_t *currentProcess = NULL;
static process_scheduler_t *waitingList = NULL;
static process_scheduler_t *freeList = NULL;

static process_scheduler_t *priorities[CANT_PRIORITIES];


static process_scheduler_t *getEmptySlot();
static void addToIndexes(process_scheduler_t *slot);
static void addToFreeSlots(process_scheduler_t *current);
static process_scheduler_t *getByPid(int pid, int *index);
static void addToReadyList(process_scheduler_t *slot);
static int removeFromReadyList(process_scheduler_t *slot);
static void removeFromIndexes(int index);
static process_scheduler_t *getPrevReadySlot(process_scheduler_t *slot);
static void enqueTicks(process_scheduler_t *proc, int ticks);
static void orderIndexes();



#ifdef DEBUG

void dumpPriorities();
void dumpPriority(int lvl);
void dumpIndexes();
void dumpWaitingList();

int main(void){

	printf("Testing!\n");

	schedSetUp();

	schedAdd(0, "init", 0);
	schedAdd(1, "iddle", 3);
	schedAdd(2, "Shell.0", 1);
	schedAdd(3, "Shell.1", 1);
	schedAdd(4, "Shell.2", 1);
	schedAdd(5, "foo", 1);


	//schedContinue(0);

	//schedStart();


	printf("About enqueu 10 ticks\n");
	enqueTicks(indexes[0], 10);
	dumpWaitingList();

	printf("About enqueu 20 ticks\n");
	enqueTicks(indexes[1], 20);
	dumpWaitingList();

	printf("About enqueu 30 ticks\n");
	enqueTicks(indexes[2], 30);
	dumpWaitingList();

	printf("About enque 5 ticks\n");
	enqueTicks(indexes[3], 5);
	dumpWaitingList();

	printf("About to eunque 7 ticks\n");
	enqueTicks(indexes[4], 7);
	dumpWaitingList();


	printf("About to eunque 7 ticks\n");
	enqueTicks(indexes[5], 7);
	dumpWaitingList();


	while(1){
		sleep(2);
		schedTicks();
		dumpWaitingList();
	}


	return 0;
}


void dumpWaitingList(){
	process_scheduler_t *proc = waitingList;
	printf("Waiting List: ");
	while(proc != NULL){
		printf(" pid: %d: ticks: %d;", proc->pid, proc->waitingTicks);
		proc = proc->nextWaiting;
	}
	putchar('\n');
}

void dumpIndexes(){
	int i;

	printf("Process Index:");

	for(i = 0 ; i < processCant ; i++)
		printf(" %d:%d:%s;", i, indexes[i]->pid, indexes[i]->name);
	printf("\tCant:%d\n", processCant);
}


void dumpPriorities(){
	int i;

	for(i = 0 ; i < CANT_PRIORITIES ; i++){
		dumpPriority(i);
	}
}

void dumpPriority(int lvl){
	int i;
	process_scheduler_t *curr;

	printf("Priority %d:", lvl);
	curr = priorities[lvl];
	if(curr != NULL){
		do {
			printf(" %d:%s;", curr->pid, curr->name);
			curr = curr->nextInPriority;
		}while(curr != priorities[lvl]);
	}
	putchar('\n');
}

#endif


void schedStart(){
	int i;

	for(i = 0 ; i < CANT_PRIORITIES ; i++){
		if(priorities[i] != NULL){
			currentProcess = priorities[i];
			return;
		}
	}
	uprintf("No hay procesos listos para correr!!%d\n", __LINE__);
	asm("cli; hlt");

}

//TODO: Test
dword schedSchedule(){
	process_scheduler_t *current = currentProcess;
	process_scheduler_t *next;
	int priority = 0;
	int found = 0;


	/* Si el proceso actual estaba corriendo, es porque está
	 * listo para seguir corriendo y está en la lista de procesos
	 * listos
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
		removeFromReadyList(current);
		addToFreeSlots(current);
	}

	currentProcess = next;
	currentProcess->ticks++;

	return procGetStack(currentProcess->pid);

}

int schedRemove(int pid){
	int index;
	process_scheduler_t *slot = getByPid(pid, &index);

	if(slot == NULL)
		return 0;

	removeFromReadyList(slot);
	//removeFromProcList(slot);
	removeFromIndexes(index);
	slot->status = DEAD;
	processCant--;

	if(slot != currentProcess){
		/* Si no está corriendo lo desalojo ahora
		 * sinó espero que lo haga el scheduler */
		procDisableMem(slot->pid);
		procReadyToRemove(slot->pid);
		slot->status = FREE;
		addToFreeSlots(slot);
	}

	return 1;
}

int schedCantProcess(){
	return processCant;
}

int schedSetPriority(int pid, int priority){
	process_scheduler_t *slot = getByPid(pid, NULL);

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
//TODO: Test
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
		sched[i].pid = i;  //Only for debuging porpouses
		indexes[i] = NULL;
		addToFreeSlots(&sched[i]);
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
	//addToProcList(slot);
	return pid;
}

int schedGetPriority(int pid){
	process_scheduler_t *slot = getByPid(pid, NULL);

	if(slot == NULL)
		return -1;

	return slot->priority;
}

int schedContinue(int pid){
	process_scheduler_t *slot = getByPid(pid, NULL);

	if(slot == NULL && slot->status != BLOCKED)
		return 0;

	slot->status = READY;
	addToReadyList(slot);
	return 1;
}

int schedBlock(int pid){
	process_scheduler_t *slot = getByPid(pid, NULL);

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
	schedContinue(pid);
}

void schedSleep(int milliseconds){

	int ticks = (milliseconds+1)/55;

	/* Si no lo paro al menos un tick, salgo */
	if(ticks < 1)
		return;
	removeFromReadyList(currentProcess);
	currentProcess->status = WAITING;

	enqueTicks(currentProcess, ticks);

	return;
}

static void enqueTicks(process_scheduler_t *proc, int ticks){
	int seted = 0;
	process_scheduler_t *prev, *curr;

	if(ticks < 1)
		return;

	if(waitingList == NULL){
		waitingList = proc;
		proc->waitingTicks = ticks;
		proc->nextWaiting = NULL;
	}
	else
	{
		curr = waitingList;

		while(curr != NULL && !seted){
			if(ticks <= curr->waitingTicks){
				if(curr == waitingList){
					/* Es el primero!*/
					curr->waitingTicks -= ticks;
					proc->waitingTicks = ticks;
					proc->nextWaiting = waitingList;
					waitingList = proc;
				}
				else
				{
					prev->nextWaiting = proc;
					proc->nextWaiting = curr;
					proc->waitingTicks = ticks;
					curr->waitingTicks -= ticks;
				}
				seted = 1;
			}
			else
			{
				prev = curr;
				curr = curr->nextWaiting;
				ticks -= prev->waitingTicks;
			}

		}

		if(seted == 0){
			prev->nextWaiting = proc;
			proc->nextWaiting = NULL;
			proc->waitingTicks = ticks;
		}

	}
	return;
}

int schedChangeStatus(int pid, status_t status){
	process_scheduler_t *slot = getByPid(pid, NULL);

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
//TODO: Test
void schedResetStatics(){

	int i;

	for(i = 0 ; i < processCant ; i++)
		indexes[i]->ticks = 0;


	return;
}

int schedGetInfo(schedProcData_t data[], int max){

	int i;

	for(i = 0; i < max && i < processCant ; i++){
		strcpy(data[i].name, indexes[i]->name);
		data[i].pid = indexes[i]->pid;
		data[i].priority = indexes[i]->priority;
		data[i].status = indexes[i]->status;
		data[i].ticks = indexes[i]->ticks;
	}

	return i;
}




static void addToIndexes(process_scheduler_t *slot){

//	if(indexes[processCant-1] == NULL){
//		uprintf("El proceso anterior es nulo!!! Archivo %s Linea: %d\n", __FILE__,__LINE__);
//		asm("cli; hlt");
//	}

	indexes[processCant-1] = slot;
	orderIndexes();
	return;

}

static void addToFreeSlots(process_scheduler_t *current){

	current->nextProcess = freeList;
	freeList = current;
}

static process_scheduler_t *getByPid(int pid, int *index){
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

      if ((low < processCant) && (indexes[low]->pid == pid)){
          if (index != NULL)
        	  *index = low;
          return indexes[low]; // found
      }
      else
          return NULL; // not found
}


static void addToReadyList(process_scheduler_t *slot){

	process_scheduler_t *next;

	if(priorities[slot->priority] == NULL)
	{
		/*
		 * Este es el único proceso con esta prioridad
		 */
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
static int removeFromReadyList(process_scheduler_t *slot){
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
		if(priorities[slot->priority] == slot){
			/* Este es el elemento apuntado, tengo
			 * que correrlo tambien.
			 */
			priorities[slot->priority] = slot->nextInPriority;
		}
		return 1;
	}
	return 0;
}

static process_scheduler_t *getPrevReadySlot(process_scheduler_t *slot){

	process_scheduler_t *rta;

	rta = priorities[slot->priority];

	if(rta == NULL){
		uprintf("La lista %d no es circular!!\n", slot->priority);
		asm("cli; hlt;");
	}

	while(rta->nextInPriority != slot){
		rta = rta->nextInPriority;
	}

	return rta;
}

static process_scheduler_t *getEmptySlot(){
	process_scheduler_t *free;

	free = freeList;
	if(free != NULL)
		freeList = freeList->nextProcess;

	return free;
}


static void removeFromIndexes(int index){
	int i;

	for(i = index ; i < processCant-1 ; i++)
		indexes[i] = indexes[i+1];
	indexes[processCant-1] = NULL;
	return;
}


static void orderIndexes(){
		int size = processCant;
		process_scheduler_t **v = indexes;
		int inc = size/2;
		int i, j;
		process_scheduler_t *tmp;

		while(inc > 0){
			for(i = inc ; i < size ; i++){
				tmp = v[i];
				j = i;
				while(j >= inc && v[j - inc]->pid > tmp->pid){
					v[j] = v[j - inc];
					j = j - inc;
				}
				v[j] = tmp;
			}
			inc = inc/2;
		}
}
