#include "sys.h"
#include "drivers/video/video.h"

void syswrite(int fd, char * buffer, int qty )
{

	switch(fd)
	{
		case STDOUT: testVideo();
		break;
	}
	return;
}


void sysread(int fd, char * buffer, int qty )
{
	return;
}
