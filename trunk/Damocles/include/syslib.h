/* syslib.h */

/* Esta librería contiene funciones que interactuan con el sistema */

#ifndef SYSLIB_H_
#define SYSLIB_H_

#include "../include/defs.h"


/* Desabilita las interrupciones si estaban habilitadas y guarda y retorna el
 * estado de las mismas.
 */
int disableInts();


/* Restarua el estado de las interrupciones deacuerdo a como estaban según
 * el parámetro recuperado por disableInts();
 */

void restoreInts(int iflag);


/* Imprime mensajes en pantalla sin pasar por el número de página.
 * Escribe a partir de la última línea y no altera la posición del cursor.
 */
void kernelPanic(const char* format, ...);

#endif /* SYSLIB_H_ */
