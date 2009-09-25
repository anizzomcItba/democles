#include "include/kernel.h"
#include "include/defs.h"
#include "include/multiboot.h"
#include "include/sysasm.h"
#include "include/string.h"
#include "include/video.h"
#include "include/shell.h"
#include "drivers/mouse/mouse.h"
#include "drivers/keyboard/keyboard.h"
#include "include/stdio.h"
#include "include/timer.h"
#include "include/clipboard.h"


#include "drivers/video/crtc6845.h" //TODO: Esta de debugeo esto
void testText(void);


extern int default_layout;
DESCR_INT idt[0x81];			/* IDT de 81h entradas*/
IDTR idtr;				/* IDTR */





/**********************************************
KERNEL
*************************************************/

int _main(multiboot_info_t* mbd, unsigned int magic)
{

	fdTableInit();

	setCursor(0, 0);
/* CARGA DE IDT CON LA RUTINA DE ATENCION DE IRQ0    */





	setup_IDT_entry (&idt[0x08], 0x08, (dword)&int_08_handler, ACS_INT, 0);
	setup_IDT_entry (&idt[0x80], 0x08, (dword)&int_80_handler, ACS_INT, 0);
	setup_IDT_entry (&idt[0x09], 0x08, (dword)&int_09_handler, ACS_INT, 0);
	setup_IDT_entry (&idt[0x74], 0x08, (dword)&int_74_handler, ACS_INT, 0);

/* Carga de IDTR    */

	idtr.base =(dword) &idt;
	idtr.limit = sizeof(idt)-1;

	_lidt(&idtr);


/* Habilito interrupcion de timer tick*/

	_cli();

	mouseCallback callbck;
    callbck = &updateMouseCursor;


	mouseInitialize(callbck);

	setPen(10,10);

	/* 1 1 1 1 1 0 0 0
	 * ? ? ? ? ? 2 K T  T = timer, K = Keyboard, 2 = IRQ2
	 */

	mascaraPIC1(0xF8);

	/* 1 1 1 0 1 1 1 1
	 * ? ? ? M ? ? ? ?  M= Mouse
	 */

	mascaraPIC2(0xEF);


	saverInit();


	setPage(WORK_PAGE);
	clearScreen();

	_sti();


	clearScreen();
	setCursor(0, 0);

	shell();

	kprint("System Halted");
	return 0;

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


