#ifndef _CRTC6845_H_
#define _CRTC6845_H_

#include "../../include/defs.h"

#define WHITE_TXT 0x07
#define INV 0x70


/* Coordenada espacial para guardar las posiciones del cursor */
typedef struct {
	int x;
	int y;
} coord_t;


void _vtflush(int page, void *buff, size_t qty);
void _vtcflush(int page, void *buff, size_t qty);
void _vreset();
coord_t _vgetcursor(int page);
void _vtsetcursor(int page, coord_t coord);
void _vsetpage(int p);
void _vinitscreen();
void _vsetpen(coord_t coord);
void _vdisblepen();
void _vtsetcursor(int page, coord_t coord);
int _vcopy(void *buff, coord_t begin, coord_t end, int max);
coord_t _vgetpen();
int _vcols();
int _vrows();
int _vtabs();
void _vsetpage(int p);
int _vcurrentpage();
int _vpagesqty();
void _vresetpage(int p);
void _vpaint(coord_t begin, coord_t end);
void _vscroll(int page);


 #endif
