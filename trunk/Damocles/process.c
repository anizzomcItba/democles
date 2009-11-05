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
#include "include/semaphore.h"
#include "include/syscall.h"


#define MAX_PROCESS_CONTEXTS 10
#define MAX_HEAPPAGES 10
#define MAX_STACKPAGES 1

#define INIT_PROCESS 0
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
	int tag;
	int semChild; //Semaforo en el cual señalizan los hijos que terminaron
	int semFather; //Semáforo en el cual señalizo a mi padre cuando yo. termine.
	int childCount;
	exitStatus_t exitStatus; //En que estado terminó el proceso
} process_descriptor_t;


typedef struct process_contex_t {
	char name[CONTEX_NAME_LENGTH];
	process_t process;
	char *argv[MAX_PROCESS_ARGS];
	int fds[MAX_OPENFILES];
	int priority;
	int argc;
	int inUse;
} process_contex_t;



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
static process_contex_t contexs[MAX_PROCESS_CONTEXTS];

static void procWrapper(process_t proc, int argc, char **argv);
static byte *push(byte *ptr, byte* data, dword size);
static byte *buildStack(byte *stack, process_t p, int argc, char **argv);
static int getFreeSlot();
static void freeProcessMemory(int pid);
static void deallocProcess(int pid);


static void untagAll();
static int tagChildren(int pid, int depth);
static void tagDescendants(int pid);
static void removeTagged();



static void initContext(int index, char *name, process_t p, int priory);
static int *getFDs(int pid);

int idle(int argc, char **argv);

processApi_t getContext(char *name, process_t p, int priority){
	int i, found;

	found = 0;

	for(i = 0 ; i < MAX_PROCESS_CONTEXTS ; i++){
		if(contexs[i].inUse == 0){
			initContext(i, name, p, priority);
			found = 1;
			break;
		}
	}

	contextAddArg( &contexs[i], name);


	if(found){
		return (processApi_t) &contexs[i];
	}
	return NULL;
}


static void initContext(int index, char *name, process_t p, int priory){
	int i;

	strcpy(contexs[index].name, name);
	contexs[index].process = p;
	contexs[index].priority = priory;
	contexs[index].argc = 0;
	contexs[index].inUse = 1;

	for(i = 0 ; i < MAX_PROCESS_ARGS ; i++)
		contexs[index].argv[i] = NULL;

	for(i = 0 ; i < MAX_OPENFILES ; i++)
			contexs[index].fds[i] = -1;

	return;
}

void contextAddFd(processApi_t context, int oldfd, int newfd){
	if((0 <= oldfd && oldfd < MAX_OPENFILES) && (0 <= newfd && newfd < MAX_OPENFILES))
		context->fds[oldfd] = getFDs(schedCurrentProcess())[newfd];
}

void contextRemoveFd(processApi_t context, int fd){
	if(0 <= fd && fd < MAX_OPENFILES)
		context->fds[fd] = -2;
}

void contextAddArg(processApi_t context, char *arg){
	if(context->argc > MAX_PROCESS_ARGS)
		return;

	context->argv[context->argc++] = arg;
}

void contextDestroy(processApi_t context){
	context->inUse = 0;
}

static int *getFDs(int pid){
	return proc[pid%MAX_PROCESS].fds;
}


int contextCreate(processApi_t context){
	int i, *fds = getFDs(schedCurrentProcess());
	void *stack =  (void*)getPage();

	if(stack == NULL)
		return -1;

	/* Si no seteo fds los heredo de mi padre */
	for( i = 0 ; i < MAX_OPENFILES ; i++){
		if(context->fds[i] == -1){
			context->fds[i] = fds[i];
		}
		else if(context->fds[i] == -2)
			context->fds[i] = -1;
	}

	int ret = procCreate(context->name, context->process, stack, NULL,
			context->fds, MAX_OPENFILES, context->argc, context->argv,
			procAttachedTTY(schedCurrentProcess()), 0, context->priority);

	if(ret != -1)
		contextDestroy(context);
	return ret;
}

/* Crea un proceso */
int procCreate(char *name, process_t p, void *stack, void *heap,
		int fds[],int files, int argc, char **argv, int tty, int orphan,
			int priority){

	int slot = getFreeSlot();

	/* Obtengo un semaforo para controlar la salida de los hijos */

	proc[slot].semChild = semGetID(0);
	proc[slot].semFather = semGetID(0);

	/* Obtengo un pid para el cual pid%MAX_PROCESS de el slot obtenido */
	proc[slot].pid  = proc[slot].pid + MAX_PROCESS;
	/* Si el proceso es huerfano, es hijo de INIT */
	proc[slot].ppid = (orphan)? INIT_PROCESS : schedCurrentProcess();

	/* Aumento en 1 la cantidad de hijos del padre */
	proc[proc[slot].ppid%MAX_PROCESS].childCount++;

	strcpy(proc[slot].name, name);
	proc[slot].status = READY;

	/* No está implementado el uso del heap, pero se podría agregar muy
	 * sencillamente asignando el heap, y luego a travez de un syscall
	 * se pueda retornar, además un malloc podría administrar esta memoria
	 * y en caso de que necesite más memoria podría pedir más páginas */


	proc[slot].heapPages[0] = heap;
	proc[slot].usedheapPages = 0;


	/* Este proceso no tiene hijos */
	proc[slot].childCount = 0;


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

	stack = push(stack, (byte*) pArgs, (argc+1)*sizeof(dword));


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


/* Retorna la cantidad de hijos que tiene el proceso indicado por su
 * pid
 */

int procGetChildCant(int pid){
	int i, j;

	for(i = j = 0; i < MAX_PROCESS ;i++){
		if(proc[i].ppid == pid)
			j++;
	}
	return j;
}


/* Le dice al scheduler que desabilite la memoria en la proxima ejecución y ya
 * lo marca listo para remover */

int procKill(int pid){

	if(pid == IDLE_PROCCES || pid == INIT_PROCESS)
		/* Estos procesos no se pueden matar */
		return 0;

	/* El proceso no existe o no está vivo */
	if(proc[pid%MAX_PROCESS].pid != pid || proc[pid%MAX_PROCESS].status != READY)
		return 0;

	untagAll();
	tagDescendants(pid);

	removeTagged();
	return 1;
}


int procGetPpid(int pid){
	if(proc[pid%MAX_PROCESS].pid != pid || proc[pid%MAX_PROCESS].status == FREE)
		return -1;
	return proc[pid%MAX_PROCESS].ppid;
}


static void removeTagged(){
	int i;
	for(i = 0 ; i < MAX_PROCESS ; i++){
		if(proc[i].tag != 0){
			proc[i].readyToRemove = 0;
			schedRemove(proc[i].pid);
			proc[i].status = ZOMBIE;
			proc[i].exitStatus = KILLED;
			/* Señalizo a mi padre que un hijo terminó */
			semInc(proc[proc[i].ppid%MAX_PROCESS].semChild);

			/* Señalizo a mi padre que yo terminé */
			semInc(proc[i].semFather);

		}

	}
}


void procEnd(int retval){
	int pid = schedCurrentProcess();


	/* El proceso está en estado zombie hasta que alguien le pida
	 * el retval */

	proc[pid%MAX_PROCESS].exitStatus = NORMAL;
	proc[pid%MAX_PROCESS].status = ZOMBIE;
	proc[pid%MAX_PROCESS].retval = retval;

	/* El proceso no está listo para remover hasta que el scheduler desabilite
	 * la memoria
	 */
	proc[pid%MAX_PROCESS].readyToRemove = 0;


	/* Le informo al scheduler que no ejecute más al proceso */

	/* Señalizo a mi padre que un hijo  terminó */
	semInc(proc[proc[pid%MAX_PROCESS].ppid%MAX_PROCESS].semChild);

	/* Señalizo a mi padre que yo terminé */

	semInc(proc[pid%MAX_PROCESS].semFather);

	schedRemove(pid);
}

/* Marca el proceso como listo, pero no lo remueve si es que no estaba listo
 * para remover, porque el scheduler tiene que desabilitar la memoria, o hay
 * que pedirle la el valor de retorno, esto es para controlar que pasen las
 * 2 cosas.
 */
void procReadyToRemove(int pid){

	int i;

	if(proc[pid%MAX_PROCESS].readyToRemove){
		/* Si el procso ya estba listo para remover, lo remuevo, reto la cantidad
		 * de hijos del padre o  lo marco como listo */

		proc[proc[pid%MAX_PROCESS].ppid%MAX_PROCESS].childCount--;
		deallocProcess(pid);
	}
	else{
		proc[pid%MAX_PROCESS].readyToRemove = 1;

		/* A todos los hijos de este proceso los hereda init */
		for(i = 0 ; i < MAX_PROCESS ; i++){
			if(proc[i].ppid == pid && proc[i].status != FREE){
				proc[i].ppid = INIT_PROCESS;
				proc[INIT_PROCESS%MAX_PROCESS].childCount++;
			}
		}


	}
	return;
}


/* Manda una señal a un proceso, sin implementar */

int procSign(int pid, int signal){
	return -1;
}

/* Retorna el estado del proceso pasado cómo parámetro, si ese proceso no
 * existe, retorna -1. En otro caso, deja el estado de la salida del proceso
 * caso que si status es KILLED, retVal no tiene sentido, una vez que ya está
 * lo marca como listo para remover.
 */

int procRetVal(int pid, exitStatus_t *status, int *retVal){
	int ret;

	/* Ese proceso no existe, no está muerto, no es hijo del proceso acutal o
	 * no está listo para pedirle el retval */
	if(proc[pid%MAX_PROCESS].pid != pid || proc[pid%MAX_PROCESS].status != ZOMBIE ||
			proc[pid%MAX_PROCESS].ppid != schedCurrentProcess())
		return -1;


	/* Asigno los valores que busca la función, preguntar por el estado de exitStatus
	 * solo consume micro, ya que el usuario no va a usar retVal si el estado es
	 * KILLED.
	 */
	*status = proc[pid%MAX_PROCESS].exitStatus;
	*retVal = proc[pid%MAX_PROCESS].retval;

	ret = proc[pid%MAX_PROCESS].pid;


	procReadyToRemove(ret);

	return ret;
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
		proc[i].pid = i - MAX_PROCESS;
		//Para que el pid pueda referenciarse con %MAX_PROCESS
		proc[i].status = FREE;
		for(j = 0 ; j < MAX_OPENFILES ; j++)
			proc[i].fds[j] = -1;

	}

	//Reset de la tabla de contextos
	for(i = 0 ; i < MAX_PROCESS_CONTEXTS ; i++){
		contexs[i].inUse = 0;
	}


	strcpy(proc[INIT_PROCESS].name, "init");
	proc[INIT_PROCESS].semChild = semGetID(0);
	proc[INIT_PROCESS].pid = INIT_PROCESS;
	proc[INIT_PROCESS].ppid = INIT_PROCESS;
	proc[INIT_PROCESS].status = RUNNING;
	proc[INIT_PROCESS].attachedTTY = 0;
	proc[INIT_PROCESS].fds[STDIN] = IN_0;
	proc[INIT_PROCESS].fds[STDOUT] = TTY_0;
	proc[INIT_PROCESS].fds[CURSOR] = TTY_CURSOR_0;
	proc[INIT_PROCESS].childCount = 0;


	int fds[3];
	fds[STDIN] = IN_0;
	fds[STDOUT] = TTY_0;
	fds[CURSOR] = TTY_CURSOR_0;


	schedSetUpInit(INIT_PROCESS, "init", 0);
	schedSetUpIdle(procCreate("idle", (process_t)idle, (void *)getPage(), NULL,
			fds, 3, 0, NULL, 0, 0, 0));

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
	/* Retorno los semaforos al sistema */
	semRetID(proc[pid%MAX_PROCESS].semChild);
	semRetID(proc[pid%MAX_PROCESS].semFather);
	/* Libero la memoria */
	freeProcessMemory(pid);
	/* Libero el slot */
	proc[pid%MAX_PROCESS].status = FREE;
}

static void procWrapper(process_t p, int argc, char **argv){
	exit(p(argc, argv));
	/* El scheduler va a ser el encargado de avisarle a proc que puede
	 * remover el proceso
	 */
	yield();
}

/* Función que marca la descendencia de todos los procesos de
 * pid, él inclusive. El uso de esta función está para no
 * realizar recursividad en en el kernel, si se usara recursividad
 * además de apilar en el stack la cantidad de llamadas a función
 * habría que recorrer al menos 1 vez el array de procesos por cada
 * proceso encontrado. Este algoritmo realiza la misma cantidad de
 * recorridos, así que es más eficiente que un algoritmo recursivo.
 */

static void tagDescendants(int pid){
	int tagged = 1;
	int depth = 1;

	proc[pid%MAX_PROCESS].tag = 1;

	while(tagged){
		int i;
		tagged = 0;

		for (i = 0 ; i < MAX_PROCESS ; i++){
			if(proc[i].tag == depth)
				tagged += tagChildren(proc[i].pid, depth+1);
		}
		depth++;
	}
}

/* Remueve las etiquetas asignadas a todos los procesos */

static void untagAll(){
	int i;
	for (i = 0 ; i < MAX_PROCESS ; i++)
		proc[i].tag = 0;
	return;
}

/* Taggea a todos los hijos del proceso dado con la etiqueta pasada como
 * parámetro
 */

static int tagChildren(int pid, int depth){
	int i, j;
	for(i  = j = 0; i < MAX_PROCESS ; i++){
		if(proc[i].ppid == pid){
			proc[i].tag = depth;
			j++;
		}
	}
	return j;
}

/* Esta función es más eficiente que la anterior, hay
 * que testearla para ver si funciona.
 */




//static int tagChildren(int depth){
//	int i, j;
//	for(i = j = 0 ; i < MAX_PROCESS ; i++){
//		if(proc[proc[i].ppid%MAX_PROCESS].tag == depth - 1){
//			proc[i].tag = depth;
//			j++;
//		}
//	}
//	return j;
//}

int procWaitPid(int pid, exitStatus_t *status, int *retval, int option){

	int i;
	int slot = -1;
	int ppid = schedCurrentProcess();
	int rta;


	if(pid == 0 || pid == 1)
		/* No se le pide retval a estos! */
		return -1;


	/* El proceso no tiene hijos! */
	if(proc[ppid%MAX_PROCESS].childCount == 0)
		return -1;


	if(pid >= 2){
		slot = pid%MAX_PROCESS;
		if(proc[slot].pid != pid || proc[slot].status == FREE ||
				proc[slot].ppid != ppid){
			/* El proceso no existe o no es hijo de este proceso */
			return -1;
		}

		/* Espero a que esté terminado */
		if(!(option & O_NOWAIT))
			semDec(proc[slot].semFather);
	}
	else
	{
		/* Espero a que termina un hijo */
		if(!(option & O_NOWAIT))
			semDec(proc[ppid%MAX_PROCESS].semChild);

		/* Busco el hijo que terminó */
		for(i = 0 ; i < MAX_PROCESS ; i++){
			if(proc[i].ppid == ppid && proc[i].status == ZOMBIE){
				slot = i;
				break;
			}
		}

		if(slot == -1)
			/* No encontré ningun proceso que haya terminado, pasé por O_NOWAIT
			 * returno */
		return -1;

	}

	if((rta = procRetVal(proc[slot].pid, status, retval)) != -1){
		/* Encontré un hijo que terminó y conseguí su valor de retorno */
		if((option & O_NOWAIT)){
			/* Consumo los wakes que no use */
			semConsume(proc[ppid%MAX_PROCESS].semChild);
			semConsume(proc[proc[slot].pid%MAX_PROCESS].semFather);
		}
		else
		{
			if(pid < 0){
				/* Esperé a cualquier hijo, consumo el wake de ese proceso */
				semConsume(proc[slot].semFather);
			}
			else
			{
				/* Esperé a un hijo particular, consume un wake mio */
				semConsume(proc[ppid%MAX_PROCESS].semChild);
			}
		}
	}

	return rta;
}
