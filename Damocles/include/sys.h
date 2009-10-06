#include "defs.h"

#ifndef _DEFS_H_
#define _DEFS_H_


void syswrite(int fd, char * buffer, size_t qty);
void sysread(int fd, char * buffer, size_t qty);
int sysflush(int fd);
void timer_tick_handler();

void fdTableInit();

void bufferAdd(int fd, char input);

#endif
