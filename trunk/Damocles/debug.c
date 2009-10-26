
#include "include/stdio.h"
#include "include/sched.h"
#include "include/io.h"
#include "include/process.h"
#include "include/mmu.h"
#include "include/sysasm.h"
#include "include/syscall.h"

static int pid = -1;

int bar(int argc, char **argv){
	int i;

	for(i = 0 ; i < 30 ; i++){
		printf("Tick %d\n", i);
		sleep(1000);
	}
	return 7;
}


void debug(){
	int fds[3] = { IN_3 , TTY_3, TTY_CURSOR_3};

	_cli();

	if(pid != -1){
		breakpoint();
		printf("Ret: %d\n",procRetVal(pid));
		pid = -1;
	}
	else
	{
	pid = procCreate("bar", (process_t)bar, (void *)getPage(), NULL, fds, 3, 0, NULL, 3, 0, 0);
	printf("Pid: %d\n", pid);
	}
	_sti();


}

void breakpoint(){
	/* Funcion que no hace nada para usar de breakpoint con el gdb */
}

