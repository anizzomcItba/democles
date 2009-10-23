
#include "../include/sched.h" //XXX: Syscall!
#include "../include/process.h" //XXX: Syscall
#include "../include/stdio.h"

static char *statusString(status_t status);

int top(int argc, char **argv){


	int currentProcess;
	schedProcData_t data[10];
	int run, i;

//	if(argc != 2){
//		printf("Cantidad de argumentos inválida!\n");
//		return -1;
//	}
//

	while(1){
		_cli();
		currentProcess = schedCantProcess();
		run = schedGetInfo(data, 10);
		_sti();

		clearScreen();
		printf("Cantidad de procesos: \n", currentProcess);
		printf("PID\t\tNombre\t\tTicks\t\tPriority\t\tStatus\n");
		for (i = 0 ; i < run ; i++){
			printf("%d\t\t%s\t\t%d\t\t%d\t\t%s\n",data[i].pid, data[i].name, data[i].ticks,
					data[i].priority, statusString(data[i].status));
		}
		sleep(2000);
	}



}


static char *statusString(status_t status){
	char *ret;
	switch(status){
	case WAITING:
		ret = "WAITING";
		break;
	case BLOCKED:
		ret = "BLOCKED";
		break;
	case READY:
	case RUNNING:
		ret = "READY";
		break;
	default:
		ret = "STATUS?";
	}

	return ret;
}
