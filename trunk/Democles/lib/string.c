/* string.c */

#include "../include/string.h"

void *memcpy(void *dst, const void *src, size_t qty) {
		char *dstC = (char *)dest;
		char *srcC = (char *)src;

		while (qty--)
			*dstC++ = *srcC++;
		return dst;
}
