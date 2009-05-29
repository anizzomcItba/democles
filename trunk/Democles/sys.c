#include "include/sys.h"
#include "drivers/video/crtc6845.h"
#include "include/string.h"

#define MAX_FDS 5

//Descriptor de archivos.
typedef struct {
	void *buffer;
	void (*flush)(int);
} fdT;


static fdT fdTable[MAX_FDS];

int fdTableInit(){
	int i;

	for(i = 0; i < MAX_FDS ; i++)
	{
		fdTable[i].buffer = NULL;
		fdTable[i].flush = NULL;
	}
}

int sysopen(int fd){

}

void syswrite(int fd, char * buffer, size_t qty)
{
	memcpy(fdTable[fd].buffer, buffer, qty);
	fdTable[fd].flush(qty);
}

void sysread(int fd, char * buffer, int qty)
{
	return;
}

