#ifndef _CRTC6845_H_
#define _CRTC6845_H_

#include "../../include/defs.h"

#define WHITE_TXT 0x07
#define INV 0x70
/**
 * Inicializa la pantalla para que tenga letras blancas, fondo negro y la deja
 * limpia.
 * Puede ser usado como un clearscreen con fondo negro.
 */
void _vinitscreen();

/**
 * Realiza un scroll de una linea, dejando la ultima linea con espacios en blancos.
 */
void _vscroll();

/*
 * Retorna el puntero a la funcion que le informa a la controladora de
 * video que hay información.
 * @param cantidad de información en bytes que hay en el buffer
 */
void(*_vgetflush())(size_t);
/*
 * Retorna el tamaño del buffer.
 */
size_t _vgetbuffsize();
void _vsetcursor(char x, char y);
char _vgetxcursor();
char _vgetycursor();
void _vselect(char xi, char yi, char xe, char ye);
void _vsetpen(char x, char y);
char _vgetxpen();
char _vgetypen();
int _vtabs();
void _vreset();
void _vpaint(char xi, char yi, char xe, char ye);
void _vcopy(char xi, char yi, char xe, char ye);
void _vscroll();
void _vcflush(size_t qty);

int _vcols();
int _vrows();

int _vcurrentpage();
int _vpagesqty();

void _vsetpage();
void _vflush();

void _vcflush(size_t qty);
void(*_vgetcflush())(size_t);


 #endif
