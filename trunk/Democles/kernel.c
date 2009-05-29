#include "include/kernel.h"
#include "include/defs.h"
#include "include/multiboot.h"
#include "include/sysasm.h"
#include "include/sys.h"

DESCR_INT idt[0x81];			/* IDT de 81h entradas*/
IDTR idtr;				/* IDTR */



/**********************************************
KERNEL
*************************************************/

int _main(multiboot_info_t* mbd, unsigned int magic)
{


/* CARGA DE IDT CON LA RUTINA DE ATENCION DE IRQ0    */

	setup_IDT_entry (&idt[0x08], 0x08, (dword)&int_08_handler, ACS_INT, 0);
	setup_IDT_entry (&idt[0x80], 0x08, (dword)&int_80_handler, ACS_INT, 0);



/* Carga de IDTR    */

	idtr.base =(dword) &idt;
	idtr.limit = sizeof(idt)-1;

	_lidt (&idtr);


/* Habilito interrupcion de timer tick*/

	mascaraPIC1(0xFE);
	mascaraPIC2(0xFF);

	_sti();


	/**
	 * writeLine devería reemplazarse por un _write o printn
	 * cuando esten implementadas las system calls.
	 */
	clearScreen();
	writeLine("Kernel Alive!", 13);
	writeLine("Scroll Testing", 14);

	while(1)
	{
		/*Shell */
//		_write(STDOUT,NULL,0);
    }

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
