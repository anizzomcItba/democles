
#include "include/stdio.h"
#include "include/sched.h"
#include "include/io.h"
#include "include/process.h"
#include "include/mmu.h"
#include "include/sysasm.h"
#include "include/syscall.h"

int bar(int argc, char **argv){
	int i;

	for(i = 0 ; i < 30 ; i++){
		printf("Tick %d\n", i);
		sleep(1000);
	}
	return 7;
}


void debug(){

	char *ptr =(char*) getPage();

	freePage((unsigned int)ptr);

	*ptr = 0;

}

void breakpoint(){
	asm("cli; hlt"); /* Hace un halt del micro y se para completamente */
}

