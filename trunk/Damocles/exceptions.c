#include "process.h"
#include "sched.h"
#include "sysasm.h"
#include "stdio.h"


void pageFault(){
	int pid = schedCurrentProcess();
	printf("Process %d received PAGE FAULT\n", pid);
	procKill(pid);
	yield();
}

void zeroDiv(){
	int pid = schedCurrentProcess();
	printf("Process %d tried to perform a ZERO DIVISION\n", pid);
	procKill(pid);
	yield();
}

void over_ex(){
	int pid = schedCurrentProcess();
	printf("Process %d caused Overflow Exception\n", pid);
	procKill(pid);
	yield();
}

void inv_op(){
	int pid = schedCurrentProcess();
	printf("Process %d tried to invoque an invalid operand\n", pid);
	procKill(pid);
	yield();
}

void inv_ss(){
	int pid = schedCurrentProcess();
	printf("Process %d tried to use an invalid Stack Segment\n", pid);
	procKill(pid);
	yield();
}

void exception(int exc){
	int pid = schedCurrentProcess();
	printf("Process %d caused exception %d\n", pid, exc);
	procKill(pid);
	yield();
}

void gp_fault(){
	int pid = schedCurrentProcess();
	printf("Process %d caused a General Protection fault\n", pid);
	procKill(pid);
	yield();
}
