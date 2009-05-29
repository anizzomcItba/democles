#ifndef _CRTC6845_H_
#define _CRTC6845_H_

/**
 * Inicializa la pantalla para que tenga letras blancas, fondo negro y la deja
 * limpia.
 */

void _vinitscreen();

/**
 * Scrolea
 */
void _vscroll();
void _vflush(void *, int qty);
void * _vgetbuffer();

#endif
