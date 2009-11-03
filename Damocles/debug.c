
#include "include/stdio.h"
#include "include/sched.h"
#include "include/io.h"
#include "include/process.h"
#include "include/mmu.h"
#include "include/sysasm.h"
#include "include/syscall.h"
#include "include/sysasm.h"
#include "include/math.h"

int foobar(int argc, char **argv);
int bar(int argc, char **argv);

static int fds[3];


int bar(int argc, char **argv){

	void breakpoint();

	int j, i = 1;


	for(j = 0 ; j <= argc ; j++)
		printf("Argumento %d: %s\n", j, argv[j]);


	int max = random(30);
	for(i = 0 ; i <  max ;){
		printf("[*]PID: %d Tick %d \n", getpid(), i++);
		sleep(1000);
	}


	exit(max);

	char *p = (char *) 0x500000;

	*p = 'A';




	return max;
}


void debug(){

//	int i, max = 10;
//	int ret, retval;
//	exitStatus_t status;
//	int i1, i2;

	fds[0] = IN_5;
	fds[1] = TTY_5;
	fds[2] = TTY_CURSOR_5;

	//procCreate("Bar", (process_t)bar, (void *)getPage(), NULL, fds, 3, 2, arg, 0, 1, 0);

	processApi_t proc = getContext("bar", bar, 0);

	contextAddArg(proc, "Hola Lucho!");
	contextAddArg(proc, "Esto es otro argumento.");

	if(proc == NULL || contextCreate(proc) == -1 )
		printf("proc NULL!\n");


		//		exit(-1);
//	}//	int i, max = 10;
	//	int ret, retval;
	//	exitStatus_t status;
	//	int i1, i2;

//
//	ret = waitpid(-1, &status, &retval, 0);
//	printf("Pid: %d has ended %s with return code: %d\n", ret, (status == KILLED)? "KILLED":"NORMALY", retval);

}

int foobar(int argc, char **argv){
		printf("Alive! %d\n", getpid());
		sleep(1000);
		return random(100);
}




void breakpoint(){
//	asm("cli; hlt"); /* Hace un halt del micro y se para completamente */
}

