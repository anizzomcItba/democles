#include "include/kernel.h"
#include "include/defs.h"
#include "include/multiboot.h"
#include "include/sysasm.h"
#include "include/string.h"
#include "include/video.h"
#include "include/shell.h"
#include "drivers/mouse/mouse.h"
#include "drivers/keyboard/keyboard.h"
#include "drivers/video/crtc6845.h"
#include "include/stdio.h"
#include "include/timer.h"
#include "include/clipboard.h"
#include "include/mmu.h"
#include "include/syscall.h"
#include "include/sched.h"
#include "include/syscall.h"
#include "include/io.h"
#include "include/tty.h"
#include "include/process.h"
#include "include/semaphore.h"
#include "drivers/video/crtc6845.h"
#include "include/syslib.h"

void testText(void);


#include "include/sched.h"


extern int default_layout;
DESCR_INT idt[0x81];			/* IDT de 81h entradas*/
IDTR idtr;				/* IDTR */


static void *temporalSchedStack;
static void *temporalFaultStack;

void top(int argc, char **argv);
void debug();

void foo(int argc, char *argv[]){
	char *video =(char*) 0xB8000;
	int i = 0, j = 0;

	printf("La cantidad de Args es: %d\n", argc);
	for (i = 0 ; i < argc; i++)
		printf("%s\n", argv[i]);

	while(1){
		for(i = 0 ; i < 160 ; i++){
			video[i] = j++;
			sleep(10);
		}
//		uprintf("lalala \n %d\n %s", 4, "Hola mundo!");
	}
}


/**********************************************
KERNEL
*************************************************/

int idle(int argc, char **argv);


int _main(multiboot_info_t* mbd, unsigned int magic)
{
	/* Primero habilito la paginación porque los procesos default tienen
	 * páginas.
	 */



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

	/* Inicialización de todas las terminales */
	_vinit();

	ttySetActive(WORK_PAGE);





	int fds[3];
	fds[STDIN] = IN_0;
	fds[STDOUT] = TTY_0;
	fds[CURSOR] = TTY_CURSOR_0;

	procCreate("Shell-0", (process_t)shell, (void *)getPage(), NULL, fds, 3, 0, NULL, 0, 0, 0);


	fds[STDIN] = IN_1;
	fds[STDOUT] = TTY_1;
	fds[CURSOR] = TTY_CURSOR_1;



	procCreate("Shell-1", (process_t)shell, (void *)getPage(), NULL, fds, 3, 0, NULL, 1, 0, 0);

	fds[STDIN] = IN_2;
	fds[STDOUT] = TTY_2;
	fds[CURSOR] = TTY_CURSOR_2;


	procCreate("Top", (process_t)top, (void *)getPage(), NULL, fds, 3, 0, NULL, 2, 0, 0);


	procCreate("foo",(process_t) foo, (void *)getPage(), NULL, fds, 3, 0, NULL, 0, 0, 0);

	debug();
	_sti();

	//setCursor(0, 0);


	while(1){
		_cli();
		schedResetStatics();
		_sti();
		sleep(5000);
	}

	kprint("System Halted");
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


