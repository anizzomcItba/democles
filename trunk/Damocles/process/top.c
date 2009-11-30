

#include "../include/stdio.h"
#include "../include/video.h"
#include "../include/syscall.h"
#include "../include/sysasm.h"

#define MAX_TO_GET 13


static char *statusString(status_t status);
static int getTotal(schedProcData_t data[], int cant);
static void order(schedProcData_t * data, int len);

int top(int argc, char **argv){


	int currentProcess;
	schedProcData_t data[MAX_TO_GET];
	int running, i, totalticks;

//	if(argc != 2){
//		printf("Cantidad de argumentos inválida!\n");
//		return -1;
//	}
//

	while(1){

		currentProcess = running_process();
		running = running_statics(data, MAX_TO_GET);

		totalticks = getTotal(data, running);
		if(totalticks > 10){

		clearScreen();
		setCursor(0, 0);
		kprintf("Cantidad de procesos: %d\n", currentProcess);
		kprintf("PID\t\tNombre\t\tUso\t\tPriority\t\tStatus\n");


		order(data, running);

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
		}
		sleep(1000);
	}



}


static int getTotal(schedProcData_t data[], int cant){
	int acum;
	int i;

	for(i = 0 ; i < cant ; i++)
		acum += data[i].ticks;

	return acum;
}

static void order(schedProcData_t * data, int len){
	schedProcData_t aux;
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



