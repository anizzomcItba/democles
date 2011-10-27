/* semaphore.c */

#include "semaphore.h"
#include "sched.h"
#include "sysasm.h"
#include "syslib.h"

#define MAX_PIDS 10
#define MAX_SEMAPHORES 70
typedef struct {
	int value;
	int pids[MAX_PIDS]; /* array de pids a levantar cuando el semaforo esté > 0 */
	int first;
	int last;
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
	int f;

	/* Desabilitar interrupciones */
	f = disableInts();

	for (i = 0 ; i < MAX_SEMAPHORES ; i++){
		if(!sems[i].inUse){
			sems[i].value = value;
			sems[i].inUse = 1;
			setup(i);
			return i;
		}
	}

	/* Restauro las interrupciones */
	restoreInts(f);
	return -1;
}

void semConsume(int id){
	if(sems[id].value > 0)
		sems[id].value--;
	return;
}

int semDec(int id){

	int f;
	int pid = schedCurrentProcess();
	if(isFull(id)){
		return 0;
	}

	/* Desabilitar interrupciones */
	f = disableInts();

	if(sems[id].value <= 0){
		addPid(id, pid);
		/* Esto nunca va a fallar porque es el proceso que está
		 * corriento es este momento el que se bloquea
		 */
		schedChangeStatus(pid, BLOCKED);
		yield();
	}else
		sems[id].value--;

	/* Restauro las interrupciones */
	restoreInts(f);

	return 1;
}


void semInc(int id){

	int iter = 1;
	int f;

	/* Desabilito interrupciones */
	f = disableInts();

	while(iter){
		if(isEmpty(id)){
			/* Si no hay ningun proceso a despertar cuento el wake y salgo
			 * del ciclo */
			sems[id].value++;
			iter = 0;
		} else {
			if(!schedChangeStatus(getPid(id), READY))
				/* El proceso a despertar está muerto, vuelvo a probar */
				continue;
			else
				/* lo logre despertar, salgo del ciclo */
				iter = 0;
		}
	}

	/* Restauro las interrupciones */
	restoreInts(f);

}

void semRetID(int id){
	sems[id].inUse = 0;
}


static void setup(int id){
	sems[id].first = sems[id].last = 0;
}

static void addPid(int id, int pid){
	int last = sems[id].last;

	sems[id].pids[last] = pid;
	sems[id].last = (last + 1)%MAX_PIDS;
}

static int isEmpty(int id){
	return sems[id].first == sems[id].last;
}

static int isFull(int id){
	return (sems[id].last + 1)%MAX_PIDS == sems[id].first%MAX_PIDS;
}

static int getPid(int id){
	int first = sems[id].first;
	int ret;

	ret = sems[id].pids[first];
	sems[id].first = (first + 1)%MAX_PIDS;
	return ret;
}
