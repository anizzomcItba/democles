/* semaphore.c */

#include "include/semaphore.h"
#include "include/sched.h"
#include "include/sysasm.h"

#define MAX_PIDS 10
#define MAX_SEMAPHORES 30

typedef struct {
	int pids[MAX_PIDS]; /* array de pids a levantar cuando el semaforo esté > 0 */
	int index; /* puntero que usa el adt para guardar que pids levantar */
	int value;
	int inUse;
} semaphore_t;

static semaphore_t sems[MAX_SEMAPHORES];

/* Funciones del ADT que guarda y despierta pids */

static void setup();
static int isFull(int id);
static int isEmpty(int id);
static void addPid(int id, int pid);
static int getPid(int id);

void semSetup(){
	int i;
	for (i = 0 ; i < MAX_SEMAPHORES ; i++)
		sems[i].inUse = 0;
}


int semGetID(int value){
	int i;
	for (i = 0 ; i < MAX_SEMAPHORES ; i++){
		if(!sems[i].inUse){
			sems[i].value = value;
			sems[i].inUse = 1;
			setup(i);
			return i;
		}
	}
	return -1;
}

int semDec(int id){

	int pid = schedCurrentProcess();
	if(isFull(id)){
		return 0;
	}

	if(--sems[id].value < 0){
		addPid(id, pid);
		schedChangeStatus(pid, BLOCKED);
		yield();
	}

	return 1;
}


void semInc(int id){

	/* Despierto a todos los procesos dormidos en el semaforo */
	if(++sems[id].value >= 0){
		while(!isEmpty(id)){
			schedChangeStatus(getPid(id), READY);
		}
	}
}

void semRetID(int id){
	sems[id].inUse = 0;
}


static void setup(int id){
	sems[id].index = 0;
}

static void addPid(int id, int pid){
	sems[id].pids[sems[id].index++] = pid;
}

static int isEmpty(int id){
	return sems[id].index == 0;
}

static int isFull(int id){
	return sems[id].index == MAX_PIDS;
}

static int getPid(int id){
	return sems[id].pids[--sems[id].index];
}
