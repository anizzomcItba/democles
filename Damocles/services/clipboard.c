/* clipboard.c */

#include "../include/string.h"
#include "../include/sysasm.h"

static char clipBuffer[80*25*2+160];		//Video Size;

static int dataSize = 0;

static void _cflush(int qty){
	_read(CLIPBOARD, clipBuffer, qty);
	dataSize = qty;
}

void clipboardPaste(int fd){
	_write(fd, clipBuffer, dataSize);
}

int clipboardData(){
	return dataSize;
}

void clipboardRead(int offset, int limit, void *bufferOut){
	memcpy(bufferOut, clipBuffer+offset, limit-offset);
}

void(*_cgetcflush())(int){
	return _cflush;
}
