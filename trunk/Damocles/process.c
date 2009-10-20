/* process.c */

#include "include/sched.h"
#include "include/defs.h"
#include "include/string.h"
#include "include/mmu.h"
#include "include/io.h"
#include "include/sysasm.h"
#include "include/process.h"

#define NAME_LENGTH 30
#define MAX_HEAPPAGES 10
#define MAX_STACKPAGES 1
#define MAX_OPENFILES 10
#define INIT_PROCESS 0
#define MAX_PROCESS_ARGS 20
#define IDLE_PROCCES 1	//TODO: Cambiar al pid del proceso iddle

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
} process_descriptor_t;

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


static process_descriptor_t proc[MAX_PROCESS];

static char idleStack[4096]; //XXX que eso lo de el MMU

static byte *push(byte *ptr, byte* data, dword size);
static byte *buildStack(byte *stack, process_t p, int argc, char **argv);
static int getFreeSlot();


void idle(int argc, char **argv);


/* Crea un proceso */
int procCreate(char *name, process_t p, void *stack, void *heap,
		int fds[],int files, int argc, char **argv, int tty, int orphan, int priority){

	int slot = getFreeSlot();
//

	/* Obtengo un pid para el cual pid%MAX_PROCESS de el slot obtenido */
	proc[slot].pid  = proc[slot].pid + MAX_PROCESS;
	/* Si el proceso es huerfano, es hijo de INIT */
	proc[slot].ppid = (orphan)? INIT_PROCESS : schedCurrentProcess();
	strcpy(proc[slot].name, name);
	proc[slot].status = READY;
	proc[slot].heapPages[0] = heap;
	proc[slot].usedheapMages = 1;
	proc[slot].stackPages[0] = stack;
	proc[slot].usedstackPages = 1;
	proc[slot].attachedTTY = tty;
	memcpy(proc[slot].fds, fds, files*sizeof(int));


	proc[slot].ESP = (byte*) buildStack(stack, p, argc, argv);

	/* Agregarlo al scheduler para que lo empiece a correr */
	schedAdd(proc[slot].pid, name, priority);
	schedChangeStatus(proc[slot].pid, READY);

	return proc[slot].pid;
}



static byte *buildStack(byte *stack, process_t p, int argc, char **argv){
	int i, j;
	context_t context;
	char *pArgs[MAX_PROCESS_ARGS];

	stack = stack + MEM_PAGE_SIZE;


	j = argc;
	pArgs[j] = NULL;

	for(i = argc - 1 ; i >= 0 ; i--){
		stack = push(stack, (byte*) argv[i], strlen(argv[i])+1);
		pArgs[--j]= (char*) stack;
	}

	stack = push(stack, (byte*) pArgs, argc*sizeof(dword));


	context.EFLAGS = 0x1 << 9; /* I flag on */
	context.EIP = (dword) p;
	context.CS = 0x08;
	context.retAddr = (dword) &procEnd;
	context.argc = argc;
	context.pArgv = (dword) stack;
	stack = push(stack, (byte*)&context, sizeof(context_t));

	return stack;

}

int procKill(int pid){
	return 1;
}

int procEnd(int retval){
	return 1;
}

int procSign(int pid, int signal){
	return 1;
}

int procRetVal(int pid){
	return 0;
}

void procEnableMem(int pid){
	return;
}

void procDisableMem(int pid){
	return;
}

void procSetup(){
	int i, j;

	//Reset de la tabla de process
	for(i = 1 ; i < MAX_PROCESS ; i++){
		proc[i].pid = i - MAX_PROCESS; //Para que el pid pueda referenciarse con %MAX_PROCESS
		proc[i].status = FREE;
		for(j = 0 ; j < MAX_OPENFILES ; j++)
			proc[i].fds[j] = -1;

	}


	strcpy(proc[INIT_PROCESS].name, "init");
	proc[INIT_PROCESS].pid = INIT_PROCESS;
	proc[INIT_PROCESS].ppid = INIT_PROCESS;
	proc[INIT_PROCESS].status = RUNNING;
	proc[INIT_PROCESS].attachedTTY = 0;
	proc[INIT_PROCESS].fds[STDIN] = IN_0;
	proc[INIT_PROCESS].fds[STDOUT] = TTY_0;
	proc[INIT_PROCESS].fds[CURSOR] = TTY_CURSOR_0;


	strcpy(proc[IDLE_PROCCES].name, "idle");
	proc[IDLE_PROCCES].pid = IDLE_PROCCES;
	proc[IDLE_PROCCES].ppid = IDLE_PROCCES;
	proc[IDLE_PROCCES].status = READY;
	proc[IDLE_PROCCES].attachedTTY = 0;
	proc[IDLE_PROCCES].fds[STDIN] = IN_0;
	proc[IDLE_PROCCES].fds[STDOUT] = TTY_0;
	proc[IDLE_PROCCES].fds[CURSOR] = TTY_CURSOR_0;
	proc[IDLE_PROCCES].ESP =(byte*) buildStack((byte*)idleStack, (process_t)idle, 0, NULL);



	schedSetUpInit(INIT_PROCESS, "init", 0);
	schedSetUpIdle(IDLE_PROCCES, "iddle", 0);

	return;
}

int procAttachedTTY(int pid){
	return proc[pid%MAX_PROCESS].attachedTTY;
}

int procGetFD(int fd){
	return proc[schedCurrentProcess()%MAX_PROCESS].fds[fd];
}


dword procGetStack(int pid){
	return (dword) proc[pid%MAX_PROCESS].ESP;
}

void procSaveStack(byte* stackPtr){
	proc[schedCurrentProcess()%MAX_PROCESS].ESP = stackPtr;
}

static byte* push(byte *ptr, byte* data, dword size){
	ptr = ptr - size;
	memcpy(ptr, data, size);
	return ptr;
}

static int getFreeSlot(){
	int i;
	for(i = 0 ; i < MAX_PROCESS ; i++){
		if(proc[i].status == FREE){
			return i;
		}
	}

	return -1;
}

void idle(int argc, char **argv){
	while(1){
		halt();
	}
}

