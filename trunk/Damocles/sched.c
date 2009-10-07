
#include "include/sched.h"
#include "include/defs.h"

#define KEYBOARD 8


int schedGetGlobalFd(int fd){
	switch(fd){
	case STDOUT: return 0;
	//8 Terminales
	case STDIN: return KEYBOARD;
	case CURSOR: return 9;
	//8 Terminales
	case CLIPBOARD: return 17;
	}
	return -1;
}


int schedAttachedTTY(){
	return 0; //TODO: Retornar la terminal atacheada.
}
