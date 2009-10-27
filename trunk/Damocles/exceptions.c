#include "include/process.h"
#include "include/sched.h"
#include "include/sysasm.h"
#include "include/stdio.h"


void pageFault(){
	int pid = schedCurrentProcess();
	printf("Process %d received PAGE FAULT\n", pid);
	procKill(pid);
	yield();
}

