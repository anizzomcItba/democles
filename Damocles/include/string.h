
#ifndef _STRING_H_
#define _STRING_H_

#include "defs.h"

void *memcpy(void *dst, const void *src, size_t qty);
int strlen(const char *str);
void itoa (char *buf, int base, int d);
int atoi (char * s);
int strcmp(char * str1, char * str2 );
void strcpy(char *dst, char *src);
void substr(char * dst, char *src, int len, int start);
void token(char * dst, char * str, char sep);
void stradd(char * dst, char * src);
void strremove(char * str, int start, int finish);
#endif
