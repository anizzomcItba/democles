
#include "include/stdio.h"
#include "include/sched.h"


void debug(){
	printf("Sacando del scheduler foo\n");
	schedRemove(4);
}

void breakpoint(){

}
