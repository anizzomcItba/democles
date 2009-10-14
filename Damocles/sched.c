
#include "include/sched.h"
#include "include/defs.h"
#include "include/string.h"
#include "include/mmu.h"
#include "include/sysasm.h"

#define KEYBOARD 8

#define NAME_LENGTH 30
#define MAX_HEAPPAGES 10
#define MAX_STACKPAGES 1
#define MAX_OPENFILES 10
#define MAX_PROCESS 71 //La cantidad de procesos máxima que soporta el sistema
#define INIT_PROCESS 0
#define MAX_PROCESS_ARGS 20
#define IDLE_PROCCES -1	//TODO: Cambiar al pid del proceso iddle

typedef enum {WAITING, BLOCKED, READY, RUNNING, DEAD, FREE, ZOMBIE} status_t;

static int currentPid;

typedef void(*process_t)(int, void*);

static byte* push(byte *ptr, byte* data, dword size);

void schedSaveStack(dword stackPtr);

static int getFreeSlot();

typedef struct {
	int pid;
	int ppid; //Pid del proceso padre.
	char name[NAME_LENGTH]; //El nombre del proces
	status_t status;	//El estado del proceso
	int watingTicks;	//La cantidad de ticks que espera a ser despertado
	byte *heapPages[MAX_HEAPPAGES]; //Las páginas del heap
	int usedheapMages; //La cantidad de páginas que pertenecen al heap del proceso.
	byte *stackPages[MAX_STACKPAGES]; //Las páginas del stack
	int usedstackPages; //La cantidad de páginas que pertenecen al stack del proceso.
	byte* ESP; //Puntero al tope de la pila del proceso.
	int attachedTTY; //La terminal a la que está attacheado el proceso.
	int fds[MAX_OPENFILES];
	int retval; //El valor de retorno
	int nextProcess; //El slot del proceso siguiente
} process_descriptor_t;



static process_descriptor_t process[MAX_PROCESS];

#pragma pack (1)

typedef struct {
	//Los contenidos de los registros generales.
	//Otra versión guardaría el resto. Cómo mmsX o xmmsX...
	//En este orden los guarda pushad
	dword EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX,  EIP, CS, EFLAGS;
	dword retAddr;
	dword argc;
	dword pArgv;
} context_t;


/* Función que decrementa los ticks que se estan esperando para ejecutar
 * nuvamente.
 */

void schedTicks(){

	int pid, starting;

	/* Si es el proceso Iddle lo tengo que saltear porque no forma parte
	 * de la lista
	 */

	if(currentPid == IDLE_PROCCES){
		pid = starting = process[currentPid%MAX_PROCESS].nextProcess;
	}
	else
		pid = starting = currentPid;


	do{
		/* Recorro todos los processos buscando a quienes estan
		 * esperando. Si estan esperando decremento el contador
		 */
		if(process[pid%MAX_PROCESS].status == WAITING){
			if(--process[pid%MAX_PROCESS].watingTicks){
				/* Si el contador llego a cero, el proceso esta listo
				 * para volver a ejecutar.
				 */
				process[pid%MAX_PROCESS].status = READY;
			}

		}
		pid = process[pid%MAX_PROCESS].nextProcess;

	}while(pid != starting);


}

int schedCreateProcess(char *name, process_t p, void *stack, void *heap,
		int fds[],int files, int argc, char **argv, int tty, int orphan){

	_cli(); //TODO

	context_t context;
	int i = 0, j= 0;
	int slot = getFreeSlot();
	char *pArgs[MAX_PROCESS_ARGS];

	/* Obtengo un pid para el cual pid%MAX_PROCESS de el slot obtenido */
	process[slot].pid  = process[slot].pid + MAX_PROCESS;
	/* Si el proceso es huerfano, es hijo de INIT */
	process[slot].ppid = (orphan)? INIT_PROCESS : schedCurrentProcess();
	strcpy(process[slot].name, name);
	process[slot].status = READY;
	process[slot].watingTicks = 0;
	process[slot].heapPages[0] = heap;
	process[slot].usedheapMages = 1;
	process[slot].stackPages[0] = stack;
	process[slot].usedstackPages = 1;
	process[slot].attachedTTY = tty;
	memcpy(process[slot].fds, fds, files*sizeof(int));
	process[slot].nextProcess = process[schedCurrentProcess()%MAX_PROCESS].nextProcess;
	process[schedCurrentProcess()%MAX_PROCESS].nextProcess = process[slot].pid;
	process[slot].ESP = (byte*) process[slot].stackPages[0] + MEM_PAGE_SIZE;


	/* Creado del estado inicial de la pila */

	for(i = argc - 1 ; i >= 0 ; i++){
		process[slot].ESP = push(process[slot].ESP, (byte*) argv[i], strlen(argv[i])+1);
		pArgs[j++]= (char*) process[slot].ESP;
	}
	pArgs[j] = NULL;

	/* Creado el array de punteros a argumentos */
	process[slot].ESP = push(process[slot].ESP, (byte*) pArgs, j*sizeof(dword));


	context.EFLAGS = 0x1 << 9;
	context.EIP = (dword) p;
	context.CS = 0x08;
	context.retAddr = (dword) &schedExitProcess;
	context.argc = j;
	context.pArgv = (dword) process[slot].ESP;
	process[slot].ESP = push(process[slot].ESP, (byte*)&context, sizeof(context_t));

	_sti();

	return process[slot].pid;
}

void schedSaveStack(dword stackPtr){
	process[schedCurrentProcess()%MAX_PROCESS].ESP = (byte*)stackPtr;
}


byte* schedSchedule(){
	/* Antes de entrar en esta función debería estar
	 * trabajando en un stack distinto, separado del resto,
	 * sinó bajar y levantar paginas usandose va a hacer
	 * estallar*/
	int pid, startingPid = schedCurrentProcess();

	/* Si el proceso actual estaba corriendo, es que esta listo para
	 * seguir corriendo */
	if(process[currentPid%MAX_PROCESS].status == RUNNING)
		process[currentPid%MAX_PROCESS].status = READY;


	while(1){
		pid = process[startingPid%MAX_PROCESS].nextProcess;
		if(process[pid%MAX_PROCESS].status == READY){

			//Acá se deberían desactivar las páginas del proceso actual
			 currentPid = pid;
			 //Acá se deberían activar las páginas del nuevo proceso

			 static char * video = (char*)0xB8000;
			 static int k = 0;
			 video[k+=2] = pid + '0';

			 return process[currentPid%MAX_PROCESS].ESP;
		}

		if(pid == startingPid){
			/* There is nothing to do... I will call the idlle process... */
			//Should exec de iddle process
		}
	}
}


/* Es la llamada que configura el scheduler y init */
void schedSetUp(){
	int i, j;

	//Reset de la tabla de process
	for(i = 0 ; i < MAX_PROCESS ; i++){
		process[i].pid = i - MAX_PROCESS; //Para que el pid pueda referenciarse con %MAX_PROCESS
		process[i].status = FREE;
		for(j = 0 ; j < MAX_OPENFILES ; j++)
			process[i].fds[j] = -1;

	}

	currentPid = 0;
	strcpy(process[0].name, "init");
	process[0].pid = 0;
	process[0].ppid = 0;
	process[0].status = RUNNING;
	process[0].nextProcess = 0;
	process[0].attachedTTY = 0;
	process[0].fds[STDOUT] = 0;
	process[0].fds[STDIN] = 8;
	process[0].fds[CURSOR] = 9;

	return;
}


static byte* push(byte *ptr, byte* data, dword size){
	ptr = ptr - size;
	memcpy(ptr, data, size);
	return ptr;
}



int schedCurrentProcess(){
	return currentPid;
}

int schedGetGlobalFd(int fd){
	switch(fd){
	case STDOUT: return 0;
	//8 Terminales
	case STDIN: return KEYBOARD;
	case CURSOR: return 9;
	//8 Terminales
	case CLIPBOARD: return 17;
	}
	return -1;
}


static int getFreeSlot(){
	int i;
	for(i = 0 ; i < MAX_PROCESS ; i++){
		if(process[i].status == FREE){
			return i;
		}
	}

	return -1;
}


void schedSleep(int miliseconds){
	process[currentPid%MAX_PROCESS].status = WAITING;
	process[currentPid%MAX_PROCESS].watingTicks = 55*miliseconds;
	return;
}

int schedAttachedTTY(){
	return process[currentPid%MAX_PROCESS].attachedTTY;
}

void schedExitProcess(int ret){
	return;
}
