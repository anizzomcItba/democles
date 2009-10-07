
#ifndef __IO_H_
#define __IO_H_


void syswrite(int fd, char * buffer, size_t qty);
void sysread(int fd, char * buffer, size_t qty);
int sysflush(int fd);

void fdTableInit();

void bufferAdd(int fd, char input);

#endif
