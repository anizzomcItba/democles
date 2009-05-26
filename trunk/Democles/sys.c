#include "sys.h"
#include "drivers/video/video.h"

void syswrite(int fd, char * buffer, int qty )
{

	switch(fd)
	{
		case STDOUT: writeLine(buffer, qty);
		break;
	}
	return;
}


void sysread(int fd, char * buffer, int qty )
{
	return;
}
