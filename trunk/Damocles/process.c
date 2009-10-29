/* process.c */

#include "include/sched.h"
#include "include/defs.h"
#include "include/string.h"
#include "include/mmu.h"
#include "include/io.h"
#include "include/sysasm.h"
#include "include/process.h"
#include "include/stdio.h"
#include "include/syslib.h"

#define MAX_HEAPPAGES 10
#define MAX_STACKPAGES 1
#define MAX_OPENFILES 10
#define INIT_PROCESS 0
#define MAX_PROCESS_ARGS 20
#define IDLE_PROCCES 1

typedef struct {
	int pid;
	int ppid; //Pid del proceso padre.
	char name[NAME_LENGTH]; //El nombre del proces
	status_t status;	//El estado del proceso
	byte *heapPages[MAX_HEAPPAGES]; //Las páginas del heap
	int usedheapPages; //La cantidad de páginas que pertenecen al heap del proceso.
	byte *stackPages[MAX_STACKPAGES]; //Las páginas del stack
	int usedstackPages; //La cantidad de páginas que pertenecen al stack del proceso.
	byte* ESP; //Puntero al tope de la pila del proceso.
	int attachedTTY; //La terminal a la que está attacheado el proceso.
	int fds[MAX_OPENFILES];
	int retval; //El valor de retorno
	int readyToRemove;
} process_descriptor_t;

#pragma pack (1)

typedef struct {
	//Los contenidos de los registros generales.
	//Otra versión guardaría el resto. Cómo mmsX o xmmsX...
	//En este orden los guarda pushad
	dword EDI, ESI, EBP, ESP, EBX, EDX, ECX, EAX,  EIP, CS, EFLAGS;
	dword retAddr;
	dword process;
	dword argc;
	dword pArgv;
} context_t;


#pragma pack ()

static process_descriptor_t proc[MAX_PROCESS];

static void procWrapper(process_t proc, int argc, char **argv);
static byte *push(byte *ptr, byte* data, dword size);
static byte *buildStack(byte *stack, process_t p, int argc, char **argv);
static int getFreeSlot();
static void freeProcessMemory(int pid);
static void deallocProcess(int pid);

int idle(int argc, char **argv);


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
	proc[slot].usedheapPages = 0;
	proc[slot].stackPages[0] = stack;
	proc[slot].usedstackPages = 1;
	proc[slot].attachedTTY = tty;
	proc[slot].retval = 0;
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
	context.EIP = (dword) procWrapper;
	context.CS = 0x08;
	context.retAddr = (dword) NULL; /* Nunca se va a leer esto, ya que
	el wrapper va a liberar las cosas */
	context.process = (dword)p;
	context.argc = argc;
	context.pArgv = (dword) stack;
	stack = push(stack, (byte*)&context, sizeof(context_t));

	return stack;

}

/* Le dice al scheduler que desabilite la memoria en la proxima ejecución y ya
 * lo marca listo para remover */

int procKill(int pid){

	if(pid == IDLE_PROCCES || pid == INIT_PROCESS)
		/* Estos procesos no se pueden matar */
		return 0;

	if(proc[pid%MAX_PROCESS].pid != pid)
		return 0;

	schedRemove(pid);
	proc[pid%MAX_PROCESS].readyToRemove = 1;
	proc[pid%MAX_PROCESS].status = DEAD;
	//TODO hacer recursividad a los hijos
	return 1;
}

void procEnd(int retval){
	int pid = schedCurrentProcess();
	int i;


	/* Esta operación debe ser atómica, pero los flags se van a perder
	 * cuando se libere el proceos */
	disableInts();


	/* El proceso está en estado zombie hasta que alguien le pida
	 * el retval */

	proc[pid%MAX_PROCESS].status = ZOMBIE;
	proc[pid%MAX_PROCESS].retval = retval;

	/* El proceso no está listo para remover hasta que el scheduler desabilite
	 * la memoria
	 */
	proc[pid%MAX_PROCESS].readyToRemove = 0;

	/* Le informo al scheduler que no ejecute más al proceso */

	schedRemove(pid);

	/* A todos los hijos de este proceso los hereda init */

	for(i = 0 ; i < MAX_PROCESS ; i++){
		if(proc[i].ppid == pid){
			proc[i].ppid = INIT_PROCESS;
		}
	}

}

void procReadyToRemove(int pid){

	if(proc[pid%MAX_PROCESS].readyToRemove)
		/* Si el procso ya estba listo para remover, lo remuevo, sinó
		 * lo marco como listo */
		deallocProcess(pid);
	else
		proc[pid%MAX_PROCESS].readyToRemove = 1;


	return;
}



int procSign(int pid, int signal){
	return 1;
}

int procRetVal(int pid){
	int i = proc[pid%MAX_PROCESS].retval;
	//TODO: Acá debería hacer un dec de un sem
	/* Si el proceso estaba listo para remover, lo libero */
	if(proc[pid%MAX_PROCESS].readyToRemove)
		deallocProcess(pid);
	else
		proc[pid%MAX_PROCESS].readyToRemove = 1;

	return i;
}

void procEnableMem(int pid){
	int i;

	/* La memoria del init no se habilita */
	if(pid == INIT_PROCESS)
		return;

	for (i = 0 ; i < proc[pid%MAX_PROCESS].usedstackPages ; i++)
		enablePage((unsigned int)proc[pid%MAX_PROCESS].stackPages[i]);

	for (i = 0 ; i < proc[pid%MAX_PROCESS].usedheapPages ; i++)
		enablePage((unsigned int)proc[pid%MAX_PROCESS].heapPages[i]);

	return;
}

void procDisableMem(int pid){
	int i;

	/* La memoria del init no se desabilita */
	if(pid == INIT_PROCESS)
		return;

	for (i = 0 ; i < proc[pid%MAX_PROCESS].usedstackPages ; i++)
		disablePage((unsigned int)proc[pid%MAX_PROCESS].stackPages[i]);

	for (i = 0 ; i < proc[pid%MAX_PROCESS].usedheapPages ; i++)
		disablePage((unsigned int)proc[pid%MAX_PROCESS].heapPages[i]);

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


	int fds[3];
	fds[STDIN] = IN_0;
	fds[STDOUT] = TTY_0;
	fds[CURSOR] = TTY_CURSOR_0;



/*

	strcpy(proc[IDLE_PROCCES].name, "idle");
	proc[IDLE_PROCCES].pid = IDLE_PROCCES;
	proc[IDLE_PROCCES].ppid = IDLE_PROCCES;
	proc[IDLE_PROCCES].status = READY;
	proc[IDLE_PROCCES].attachedTTY = 0;
	proc[IDLE_PROCCES].stackPages[0] = (byte *)idle_stack;
	proc[IDLE_PROCCES].usedstackPages = 1;
	proc[IDLE_PROCCES].usedheapPages = 0;
	proc[IDLE_PROCCES].fds[STDIN] = IN_0;
	proc[IDLE_PROCCES].fds[STDOUT] = TTY_0;
	proc[IDLE_PROCCES].fds[CURSOR] = TTY_CURSOR_0;
	proc[IDLE_PROCCES].ESP =(byte*) buildStack((byte*)idle_stack, (process_t)idle, 0, NULL);

*/
	schedSetUpInit(INIT_PROCESS, "init", 0);
	schedSetUpIdle(procCreate("idle", (process_t)idle, (void *)getPage(), NULL, fds, 3, 0, NULL, 0, 0, 0));

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

int idle(int argc, char **argv){
	while(1){
		//printf("\t\tHalted!\n");
		halt();
	}
}

static void freeProcessMemory(int pid){
	int i;
	for (i = 0 ; i < proc[pid%MAX_PROCESS].usedstackPages ; i++)
		freePage((int)proc[pid%MAX_PROCESS].stackPages[i]);

	for (i = 0 ; i < proc[pid%MAX_PROCESS].usedheapPages ; i++)
			freePage((int)proc[pid%MAX_PROCESS].heapPages[i]);
}

static void deallocProcess(int pid){

	freeProcessMemory(pid);
	proc[pid%MAX_PROCESS].status = FREE;
}

static void procWrapper(process_t p, int argc, char **argv){
	procEnd(p(argc, argv));
	/* El scheduler va a ser el encargado de avisarle a proc que puede
	 * remover el proceso
	 */
	yield();
}


