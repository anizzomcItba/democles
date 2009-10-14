/* stdio.h */


#ifndef _STDIO_H_
#define _STDIO_H_

/* ~G~ ver de cambiarles los nombres a putchar y getchar. se est� volviendo medio loco con eso.*/
void kputchar(char c);

char kgetchar();
/* ---------------------------------------------------------------------------
 * kprintf
 * ---------------------------------------------------------------------------
 * Impresor con formato, similar a printf. No poseé protección de stack así
 * que no levantar más datos que los pasados como argumentos, ya que mostraía
 * errores.
 */
void kprintf(const char *fmt, ...);


void printf(const char *fmt, ...);


/* ---------------------------------------------------------------------------
 * kprint
 * ---------------------------------------------------------------------------
 * Imprime un string a travez de la salida estandart.
 */
void kprint(const char *str);


/* ---------------------------------------------------------------------------
 * getchar
 * ---------------------------------------------------------------------------
 * Retorna un caracter del FileDescriptor STDIN
 *
 */
int getchar();


#endif
