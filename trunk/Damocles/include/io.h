
#ifndef __IO_H_
#define __IO_H_

#include "defs.h"


void syswrite(int fd, char * buffer, size_t qty);
void sysread(int fd, char * buffer, size_t qty);
int sysflush(int fd);

int sysopen(char *path);

void fdTableInit();

void bufferAdd(int fd, char input);


#define IN_0 0
#define IN_1 1
#define IN_2 2
#define IN_3 3
#define IN_4 4
#define IN_5 5
#define IN_6 6
#define IN_7 7


#define TTY_0 IN_7 + 1
#define TTY_1 IN_7 + 2
#define TTY_2 IN_7 + 3
#define TTY_3 IN_7 + 4
#define TTY_4 IN_7 + 5
#define TTY_5 IN_7 + 6
#define TTY_6 IN_7 + 7
#define TTY_7 IN_7 + 8


#define TTY_CURSOR_0 TTY_7 + 1
#define TTY_CURSOR_1 TTY_7 + 2
#define TTY_CURSOR_2 TTY_7 + 3
#define TTY_CURSOR_3 TTY_7 + 4
#define TTY_CURSOR_4 TTY_7 + 5
#define TTY_CURSOR_5 TTY_7 + 6
#define TTY_CURSOR_6 TTY_7 + 7
#define TTY_CURSOR_7 TTY_7 + 8

#endif
