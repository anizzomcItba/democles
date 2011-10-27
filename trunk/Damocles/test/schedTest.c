/* schedTest.c */


#include "sched.h"
#include <stdlib.h>
#include <stdio.h>

#define MAX_PROCESS 70
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
} process_scheduler_t;

static process_scheduler_t *processList = NULL;
static int processCant = 0;

static process_scheduler_t sched[MAX_PROCESS];
static process_scheduler_t *indexes[MAX_PROCESS];
static process_scheduler_t *priorities[CANT_PRIORITIES] = {NULL};

void checkPoint();


void dumpProcessList();

void dumpIndexes();

void dumpPriorities();

void dumpPriority(int lvl);


static process_scheduler_t *freeList = NULL;


static void addToReadyList(process_scheduler_t *slot);
static process_scheduler_t *getPrevReadySlot(process_scheduler_t *slot);
static int removeFromReadyList(process_scheduler_t *slot);

static void testporcessList();
static void testIndexes();
static void testFreeSlots();
static void testPriorities();


int main(void)
{
	//testporcessList();
	//testIndexes();
	//testFreeSlots();
	testPriorities();


}

void testPriorities(){
	int i;
	process_scheduler_t *proc;
	printf("Let's test fill a priorities\n");

	for(i = 0 ; i < 30 ; i++){
		proc = malloc(sizeof(process_scheduler_t));
		proc->pid = i;
		proc->priority =  i%CANT_PRIORITIES;
		addToReadyList(proc);
	}

	dumpPriorities();

	proc = priorities[CANT_PRIORITIES -1];

	printf("Lets remove the proc %d\n", proc->pid);
	removeFromReadyList(proc);
	dumpPriority(proc->priority);


	proc = priorities[CANT_PRIORITIES -1]->nextInPriority->nextInPriority;

	printf("Lets remove the proc %d\n", proc->pid);
	removeFromReadyList(proc);
	dumpPriority(proc->priority);

	proc = priorities[CANT_PRIORITIES -1];

	printf("Lets clear the lvl %d\n", proc->priority);

	while(removeFromReadyList(proc)){
		proc = proc->nextInPriority;
		dumpPriority(proc->priority);
	}

	dumpPriority(proc->priority);


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
			printf(" %d;", curr->pid);
			curr = curr->nextInPriority;
		}while(curr != priorities[lvl]);
	}
	putchar('\n');
}

//TODO: Test
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
//TODO: Test
static process_scheduler_t *getPrevReadySlot(process_scheduler_t *slot){

	process_scheduler_t *rta;

	rta = priorities[slot->priority];

	if(rta == NULL){
		printf("La lista %d no es circular!!\n", slot->priority);
		asm("cli; hlt;");
	}

	while(rta->nextInPriority != slot){
		rta = rta->nextInPriority;
	}

	return rta;
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

//void testFreeSlots(){
//	int i = 0;
//	schedSetUp();
//	process_scheduler_t *proc = freeList;
//
//
//	while(proc != NULL){
//		i++;
//		proc = proc->nextProcess;
//	}
//	printf("La cantidad de slots libres es: %d\n", i);
//
//
//
//	while((proc = getEmptySlot())!= NULL){
//		printf("Pid: %d\n", proc->pid);
//	}
//}
//
//void testIndexes(){
//	process_scheduler_t *proc = malloc(sizeof(process_scheduler_t));
//	int i, pid;
//
//	printf("Should not appear nothing:\n");
//	dumpIndexes();
//	printf("Should Appear one thing\n");
//
//	proc->pid = 54;
//
//
//	processCant++;
//	addToIndexes(proc);
//	dumpIndexes();
//
//
//
//
//	for(i = 0 ; i < 10 ; i++){
//		proc = malloc(sizeof(process_scheduler_t));
//		proc->pid = (i%2)? i*2 : i*2+1;
//		processCant++;
//		addToIndexes(proc);
//
//		if(i == 5)
//			pid = proc->pid;
//	}
//
//	printf("Should Appear %d process ordered\n", i+1);
//	dumpIndexes();
//
//	proc = getByPid(pid, NULL);
//
//	printf("The sixth process has the pid: %d\n", proc->pid);
//
//	pid = 54;
//
//	proc = getByPid(pid, &i);
//	if(proc == NULL)
//		printf("There is no process with pid %d\n", pid);
//	else
//	{
//		printf("The process with pid %d has index %d\n", pid, i);
//		printf("The process with pid %d should have desapeared\n", pid);
//		removeFromIndexes(i);
//		processCant--;
//		dumpIndexes();
//
//	}
//
//
//	for(pid = 1 ; pid < 100 ; pid++){
//		if((proc = getByPid(pid,&i)) != NULL){
//			removeFromIndexes(i);
//			processCant--;
//			dumpIndexes();
//		}
//	}
//
//
//}
//
//void dumpIndexes(){
//	int i;
//
//	printf("Process: ");
//
//	for(i = 0 ; i < processCant ; i++)
//		printf(" %d:%d;", i, indexes[i]->pid);
//	printf("\tCant:%d\n", processCant);
//}
//
//void dumpProcessList(){
//	process_scheduler_t *proc = processList;
//
//	while(proc != NULL){
//		printf("PID: %d\t", proc->pid);
//		proc = proc->nextProcess;
//	}
//	putchar('\n');
//	return;
//}
//
//static void testporcessList(){
//	/* Esta parte testea la inclusion y la remoción de los procesos en
//	 * la lista de procesos.
//	 */
//		process_scheduler_t *proc, *toRemove;
//		int i;
//
//		//Insertion test on process list
//
//		printf("About to test insertion on processList\n");
//		for(i = 1 ; i < 10 ; i++){
//			proc = malloc(sizeof(process_scheduler_t));
//			proc->pid = i;
//			addToProcList(proc);
//
//			if(i == 5)
//				toRemove = proc;
//		}
//
//		checkPoint();
//		dumpProcessList();
//
//		removeFromProcList(toRemove);
//		printf("The the fifth process should have desapeared\n");
//		dumpProcessList();
//
//
//		removeFromProcList(processList);
//		printf("The the first process should have desapeared\n");
//		dumpProcessList();
//
//		proc = processList;
//		while(proc->nextProcess != NULL){
//			proc = proc->nextProcess;
//		}
//		removeFromProcList(proc);
//		printf("The last element should have desapear\n");
//		dumpProcessList();
//}
//

