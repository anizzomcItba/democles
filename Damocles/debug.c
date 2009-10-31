
#include "include/stdio.h"
#include "include/sched.h"
#include "include/io.h"
#include "include/process.h"
#include "include/mmu.h"
#include "include/sysasm.h"
#include "include/syscall.h"

int foobar(int argc, char **argv);
int bar(int argc, char **argv);

static int flag = 0;

int bar(int argc, char **argv){
	int i;

	int fds[3];
	fds[STDIN] = IN_5;
	fds[STDOUT] = TTY_5;
	fds[CURSOR] = TTY_CURSOR_5;


	for(i = 0 ; i < 5 ; i++){
		procCreate("foobar", (process_t)foobar, (void *)getPage(), NULL, fds, 3, 0, NULL, 0, 0, 0);
	}
	while(1){
		sleep(1000);
	}
}



int foobar(int argc, char **argv){
	while(1){
		printf("Alive! %d\n", getpid());
		sleep(1000);
	}
	return 0;
}



void debug(){
	if(flag == 0){
		flag = procCreate("bar", (process_t)bar, (void *)getPage(), NULL, 0, 0, 0, NULL, 0, 0, 0);
	}
	else{
		procKill(flag);
		flag = 0;
	}

}

void breakpoint(){
//	asm("cli; hlt"); /* Hace un halt del micro y se para completamente */
}

