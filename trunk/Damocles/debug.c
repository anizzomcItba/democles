
#include "include/stdio.h"
#include "include/sched.h"


void debug(){

	int i;
	while(i++);
	_cli();
	schedRemove(schedCurrentProcess());
	_sti();
	yield();
}

void breakpoint(){
	/* Funcion que no hace nada para usar de breakpoint con el gdb */
}

