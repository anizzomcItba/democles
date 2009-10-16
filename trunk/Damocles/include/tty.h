/* tty.h */

#ifndef TTY_H_
#define TTY_H_


/* Cambia la tty activa, cambiando de terminal */
void ttySetActive(int tty);

/* Retorna la tty activa */
int ttyGetActive();

#endif /* TTY_H_ */
