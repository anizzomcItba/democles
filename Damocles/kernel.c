#include "kernel.h"
#include "defs.h"
#include "multiboot.h"
#include "sysasm.h"
#include "string.h"
#include "video.h"
#include "shell.h"
#include "drivers/mouse/mouse.h"
#include "drivers/keyboard/keyboard.h"
#include "system/drivers/video/crtc6845.h"
#include "stdio.h"
#include "timer.h"
#include "clipboard.h"
#include "mmu.h"
#include "syscall.h"
#include "sched.h"
#include "syscall.h"
#include "io.h"
#include "tty.h"
#include "process.h"
#include "semaphore.h"
#include "system/drivers/video/crtc6845.h"
#include "syslib.h"
#include "filesystem.h"
#include "system/drivers/rtc.h"

void testText(void);


#include "include/sched.h"


extern int default_layout;
DESCR_INT idt[0x81];			/* IDT de 81h entradas*/
IDTR idtr;				/* IDTR */


static void *temporalSchedStack;
static void *temporalFaultStack;
static Directory root;
void top(int argc, char **argv);
void debug();


/**********************************************
KERNEL
*************************************************/

int idle(int argc, char **argv);


int _main(multiboot_info_t* mbd, unsigned int magic)
{
	/* Primero habilito la paginación porque los procesos default tienen
	 * páginas.
	 */


	//Configuracion de RTC
	rtc_configure();

	setup_IDT_entry (&idt[0x08], 0x08, (dword)&int_08_handler, ACS_INT, 0);
	setup_IDT_entry (&idt[0x09], 0x08, (dword)&int_09_handler, ACS_INT, 0);
	setup_IDT_entry (&idt[0x0E], 0x08, (dword)&int_0E_handler, ACS_INT, 0);
	setup_IDT_entry (&idt[0x74], 0x08, (dword)&int_74_handler, ACS_INT, 0);
	setup_IDT_entry (&idt[0x7F], 0x08, (dword)&int_7F_handler, ACS_INT, 0);
	setup_IDT_entry (&idt[0x80], 0x08, (dword)&int_80_handler, ACS_INT, 0);
	setup_IDT_entry (&idt[0x00], 0x08, (dword)&int_00_handler, ACS_INT, 0);
	setup_IDT_entry (&idt[0x04], 0x08, (dword)&int_04_handler, ACS_INT, 0);
	setup_IDT_entry (&idt[0x06], 0x08, (dword)&int_06_handler, ACS_INT, 0);
	setup_IDT_entry (&idt[0x0C], 0x08, (dword)&int_0C_handler, ACS_INT, 0);
	setup_IDT_entry (&idt[0x0D], 0x08, (dword)&int_0D_handler, ACS_INT, 0);

	/* Carga de IDTR    */

	idtr.base =(dword) &idt;
	idtr.limit = sizeof(idt)-1;

	_lidt(&idtr);

	startPaging();


	/* Los stacks crecen hacia abajo, así que tengo que retornar el fondo
	 * de la página */
	temporalSchedStack = (void *)(getPage() + MEM_PAGE_SIZE);
	temporalFaultStack = (void *)(getPage() + MEM_PAGE_SIZE);


	semSetup();
	fdTableInit();
	schedSetUp();

	procSetup();


	mouseCallback callbck;
    callbck = &updateMouseCursor;


	mouseInitialize(callbck);

	/* 1 1 1 1 1 0 0 0
	 * ? ? ? ? ? 2 K T  T = timer, K = Keyboard, 2 = IRQ2
	 */

	mascaraPIC1(0xF8);

	/* 1 1 1 0 1 1 1 1
	 * ? ? ? M ? ? ? ?  M= Mouse
	 */

	mascaraPIC2(0xEF);


//	saverInit();

	/*TODO SYSCALLS*/
	root = startFileSystem();
	populateFileSystem(root);
	/* Inicialización de todas las terminales */
	_vinit();


	ttySetActive(WORK_PAGE);





	int fds[3];
	fds[STDIN] = IN_0;
	fds[STDOUT] = TTY_0;
	fds[CURSOR] = TTY_CURSOR_0;
	procCreate("Shell-0", (process_t)shell, (void *)getPage(), NULL, fds, 3, 0, NULL, 0, 0, 1);

	fds[STDIN] = IN_1;
	fds[STDOUT] = TTY_1;
	fds[CURSOR] = TTY_CURSOR_1;
	procCreate("Shell-1", (process_t)shell, (void *)getPage(), NULL, fds, 3, 0, NULL, 1, 0, 2);

	fds[STDIN] = IN_2;
	fds[STDOUT] = TTY_2;
	fds[CURSOR] = TTY_CURSOR_2;
	procCreate("Shell-2", (process_t)shell, (void *)getPage(), NULL, fds, 3, 0, NULL, 2, 0, 2);
//
//	fds[STDIN] = IN_3;
//	fds[STDOUT] = TTY_3;
//	fds[CURSOR] = TTY_CURSOR_3;
//	procCreate("Shell-3", (process_t)shell, (void *)getPage(), NULL, fds, 3, 0, NULL, 3, 0, 2);
//
//	fds[STDIN] = IN_4;
//	fds[STDOUT] = TTY_4;
//	fds[CURSOR] = TTY_CURSOR_4;
//	procCreate("Shell-4", (process_t)shell, (void *)getPage(), NULL, fds, 3, 0, NULL, 4, 0, 2);
//
//	fds[STDIN] = IN_5;
//	fds[STDOUT] = TTY_5;
//	fds[CURSOR] = TTY_CURSOR_5;
//	procCreate("Shell-5", (process_t)shell, (void *)getPage(), NULL, fds, 3, 0, NULL, 5, 0, 2);
//
//	fds[STDIN] = IN_6;
//	fds[STDOUT] = TTY_6;
//	fds[CURSOR] = TTY_CURSOR_6;
//	procCreate("Shell-6", (process_t)shell, (void *)getPage(), NULL, fds, 3, 0, NULL, 6, 0, 2);



	fds[STDIN] = IN_7;
	fds[STDOUT] = TTY_7;
	fds[CURSOR] = TTY_CURSOR_7;
	procCreate("Top", (process_t)top, (void *)getPage(), NULL, fds, 3, 0, NULL, 7, 0, 2);

	//setCursor(0, 0);


	int pid;
	exitStatus_t status;
	int retval;


	while(1){
		_cli();
		schedResetStatics();
		_sti();
		sleep(1800);
		//printf("[*]%d:%d:%d\n",rtc_getHours(), rtc_getMinutes(), rtc_getSeconds());
		if ((pid = procWaitPid(-1, &status, &retval, O_NOWAIT)) != -1)
			printf("[*]Process %d: The process %d has ended %s with exit code: %d\n",getpid(), pid, (status == KILLED)? "KILLED": "NORMALY", retval);
	}

	kprint("System Halted");
	asm("cli; hlt");
	return 0;

}




void *getTemporalSchedStack(){
	return temporalSchedStack;
}

void *getTemporalFaultStack(){
	return temporalFaultStack;
}


/***************************************************************
*setup_IDT_entry
* Inicializa un descriptor de la IDT
*
*Recibe: Puntero a elemento de la IDT
*	 Selector a cargar en el descriptor de interrupcion
*	 Puntero a rutina de atencion de interrupcion
*	 Derechos de acceso del segmento
*	 Cero
****************************************************************/



void setup_IDT_entry (DESCR_INT *item, byte selector, dword offset, byte access,
			 byte cero) {
  item->selector = selector;
  item->offset_l = offset & 0xFFFF;
  item->offset_h = offset >> 16;
  item->access = access;
  item->cero = cero;
}


