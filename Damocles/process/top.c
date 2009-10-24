
#include "../include/sched.h" //XXX: Syscall!
#include "../include/process.h" //XXX: Syscall
#include "../include/stdio.h"

static char *statusString(status_t status);
static int getTotal(schedProcData_t data[], int cant);

int top(int argc, char **argv){


	int currentProcess;
	schedProcData_t data[10];
	int running, i, totalticks;

//	if(argc != 2){
//		printf("Cantidad de argumentos inválida!\n");
//		return -1;
//	}
//

	while(1){
		_cli();
		currentProcess = schedCantProcess();
		running = schedGetInfo(data, 10);
		_sti();

		clearScreen();
		setCursor(0, 0);
		kprintf("Cantidad de procesos: \n", currentProcess);
		kprintf("PID\t\tNombre\t\tTicks\t\tPriority\t\tStatus\n");

		totalticks = getTotal(data, running);

		for (i = 0 ; i < running ; i++){
			kprintf("%d\t\t%s\t\t%d\t\t%d\t\t%s\n",data[i].pid, data[i].name, (100*data[i].ticks)/totalticks,
					data[i].priority, statusString(data[i].status));

		}
		sleep(2000);
	}



}


static int getTotal(schedProcData_t data[], int cant){
	int acum;
	int i;

	for(i = 0 ; i < cant ; i++)
		acum += data[i].ticks;

	return acum;
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
		ret = "RUNNING";
		break;
	default:
		ret = "STATUS?";
	}

	return ret;
}
