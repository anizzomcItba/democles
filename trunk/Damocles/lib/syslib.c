/* syslib.c */

#include "../include/syslib.h"
#include "../include/sysasm.h"

int disableInts(){
	int flags = gFlags();
	/* Retorna si el I flag está on o no */
	_cli();
	return (flags & (0x1 << 9));
}

void restoreInts(int iflag){

	/* Activo el iflag si no estab!a activado */
	if(iflag)
		_sti();
	return;
}
