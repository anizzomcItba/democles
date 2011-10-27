/* clipboard.c */

#include "string.h"
#include "sysasm.h"
#include "syscall.h"
#include "io.h"
#include "tty.h"

static char clipBuffer[80*25*2+160];		//Video Size;

static int dataSize = 0;

void clipboardPaste(void){
	int i;
	for(i=0; i<dataSize ; i++)
	{
		bufferAdd(IN_0 + ttyGetActive(), clipBuffer[i]);
	}
}


void * getClipBuffer()
{
	return (void * ) clipBuffer;
}


void setDataSize(int size)
{
	dataSize=size;
}



int clipboardData(){
	return dataSize;
}


