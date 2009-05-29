#include "defs.h"
#include "drivers/video/video.h"

void syswrite(int fd, char * buffer, int qty );
void sysread(int fd, char * buffer, int qty);
void timer_tick_handler();



