
#include "include/stdio.h"
#include "include/sched.h"
#include "include/io.h"
#include "include/mmu.h"
#include "include/sysasm.h"
#include "include/syscall.h"
#include "include/sysasm.h"
#include "include/math.h"

int foobar(int argc, char **argv);
int bar(int argc, char **argv);



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

	int ret, retval;
	exitStatus_t status;


	processApi_t proc = getcontext("bar", bar, 0);

	contextaddarg(proc, "Hola Lucho!");
	contextaddarg(proc, "Esto es otro argumento.");

	if(proc == NULL || contextCreate(proc) == -1 )
		printf("proc NULL!\n");


	ret = waitpid(-1, &status, &retval, 0);
	printf("Pid: %d has ended %s with return code: %d\n", ret, (status == KILLED)? "KILLED":"NORMALY", retval);

}

int foobar(int argc, char **argv){
		printf("Alive! %d\n", getpid());
		sleep(1000);
		return random(100);
}




void breakpoint(){
//	asm("cli; hlt"); /* Hace un halt del micro y se para completamente */
}

