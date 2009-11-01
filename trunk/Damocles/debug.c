
#include "include/stdio.h"
#include "include/sched.h"
#include "include/io.h"
#include "include/process.h"
#include "include/mmu.h"
#include "include/sysasm.h"
#include "include/syscall.h"
#include "include/sysasm.h"

int foobar(int argc, char **argv);
int bar(int argc, char **argv);
void opDie();
void zeroDie();
void overDie();

static int flag = 0;

int bar(int argc, char **argv){
	int i, j, k;

	int fds[3];
	fds[STDIN] = IN_5;
	fds[STDOUT] = TTY_5;
	fds[CURSOR] = TTY_CURSOR_5;


//	for(i = 0 ; i < 5 ; i++){
//		procCreate("foobar", (process_t)foobar, (void *)getPage(), NULL, fds, 3, 0, NULL, 0, 0, 0);
//	}

	i=procCreate("zeroDie", (process_t)zeroDie, (void*)getPage(), NULL, fds, 3, 0, NULL, 0, 0, 0);
	j=procCreate("overDie", (process_t)overDie, (void*)getPage(), NULL, fds, 3, 0, NULL, 0, 0, 0);
	k=procCreate("opDie", (process_t)opDie, (void*)getPage(), NULL, fds, 3, 0, NULL, 0, 0, 0);
	printf("Zero! %d", i);
	printf("over! %d", j);
	printf("op! %d", k);
//	procCreate("zeroDie", (process_t)zeroDie, (void*)getPage(), NULL, fds, 3, 0, NULL, 0, 0, 0);
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

void zeroDie(){
	int i, a;

	for(i=0; ;i++){
		a=20/(60-i);
		sleep(100);
	}
}

void overDie(){
	unsigned int a=0xFFFFFFF0, i;

	printf("Je suis %d!", schedCurrentProcess());

	for (i=0; ; i++){
		a += i;
		_overDie();
	}
}

void opDie(){
	printf("Yo soy %d!", schedCurrentProcess());
	sleep(100);
	_opDie();
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

