/* tty.c */

/*
 * Módulo que lleva mantiene el control de las ttys.
 */

#include "tty.h"
#include "system/drivers/video/crtc6845.h"

static int activeTTY = 0;

/* Cambia la tty activa y le indica al driver de video que cambie
 * la página.
 */
void ttySetActive(int tty){
	_vsetpage(tty);
	activeTTY = tty;

}

/* Retorna la tty activa */
int ttyGetActive(){
	return activeTTY;
}
