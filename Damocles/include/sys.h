#include "defs.h"

#ifndef _DEFS_H_
#define _DEFS_H_


void syswrite(int fd, char * buffer, size_t qty);
void sysread(int fd, char * buffer, size_t qty);
void timer_tick_handler();

void fdTableInit();

#endif
