
#include "../include/sched.h" //XXX: Syscall!
#include "../include/process.h" //XXX: Syscall
#include "../include/stdio.h"
#include "../include/video.h"
#include "../include/syscall.h"
#include "../include/sysasm.h"



static char *statusString(status_t status);
static int getTotal(schedProcData_t data[], int cant);
static void order(schedProcData_t * data, int len, schedProcData_t aux);

int top(int argc, char **argv){


	int currentProcess;
	schedProcData_t data[10];
	int running, i, totalticks;
	schedProcData_t aux;

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

		order(data, running, aux);

		for (i = 0 ; i < running ; i++){
			kprintf("%d", data[i].pid);
			setCursor(13, 2+i);
			kprintf("%s", data[i].name);
			setCursor(29, 2+i);
			kprintf("%d", (100*data[i].ticks)/totalticks);
			setCursor(44, 2+i);
			kprintf("%d", data[i].priority);
			setCursor(62, 2+i);
			kprintf("%s\n", statusString(data[i].status));
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

static void order(schedProcData_t * data, int len, schedProcData_t aux){
	int i, j;
	for(i=0; i<len; i++){
		for(j=0; j<len; j++){
			if(data[i].ticks > data[j].ticks){
				aux=data[i];
				data[i]=data[j];
				data[j]=aux;
			}
		}
	}
	return;
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



