/* video.h */

#ifndef _VIDEO_H_
#define _VIDEO_H_

#include "defs.h"

/*
 * Biblioteca standard del video
 */


/*
 * Resetea la pantalla a fondo negro y letras blancas.
 */
void clearScreen();

/*
 * Coloca el cursor en la posicion indicada:
 * x lugares desde el margen izquierdo e
 * y lugares del superior.
 * Si la posicion es inválida se ignora el movimiento.
 * VER: getXc, getYx, colsQty, rowsQty.
 */
void setCursor(char x, char y);

/*
 * Retorna la poscion actual del cursor en la coordenada x,
 * es decir horizontal con respecto al margen izquierdo.
 */
char getXc();
/*
 * Retorn la posción actual del cursor en la coordenada y,
 * es decir vertial con respecto al margen superior.
 */
char getYc();

/*
 * Setea el cursor luminoso en la posición indicada.
 * Similar a setCursor, con la diferencia que si
 * se colocan coordenadas invalidas el cursor se desactiva.
 */
void setPen(char x, char y);

/*
 * Analogo a getXc pero con el cursor luminoso.
 */
char getXpen();
/*
 * Analogo a getYx pero con el cursor luminoso.
 */
char getYpen();

/*
 * Cambia la posición del cursor luminoso según el paquete de información
 * pasado. mirar defs.h y mouse.h
 */
void updateMouseCursor(MOUSE_DATA * mData );

/*
 * Realiza un scroll del contenido de la pantalla, desplazando el contenido
 * una linea hacia arriba y completa la linea inferior con blancos.
 */

void scroll();

/*
 * Escribe una cadena de caracteres a la posición actual del cursor. Si en
 * el argumento moveCursor se pasa un valor distinto de cero, el mismo se
 * actualiza al final de la cadena escrita. Sinó, este se mantendrá en la
 * posición original.
 * El salto de linea \n es ignorado.
 */
void writeToCursor(char *str, char moveCursor);

/*
 * Coloca un caracter en la posición actual del cursor. Si el argumento
 * moveCursor es distinto de cero, el cursor se mueve una posición a la
 * derecha.
 */
void putToCursor(char c);

/*
 * Cambia la página activa. Toda la información de la actual se guarda para ser
 * reemplazada por la pagina nueva. Es recomendable llamar a clearScreen luego
 * de cambiar a una página que no ha sido usada antes. Esta puede contener
 * basura.
 */
void setPage(int p);

/*
 * Informa cual es la página activa actual
 */
int currentPage();

/*
 * Informa cuantas paginas hay disponibles para ser utilizadas.
 */
int pagesQty();


/*
 * Informa cuantas columnas tiene la pantalla
 */
int colsQty();


/*
 * Informa cuantas filas tiene la pantall
 */
int rowQty();

/*
 * Copia de la pantalla, agregando \n cada vez que se saltea una linea el texto
 * de la pantalla. La lectura se deja en el clipboard.
 * Xi e Yi son las coordenadas iniciales y Xe e Ye las finales.
 */
void copyScreen(char xi, char yi, char xe, char ye);


/*
 * Las siguientes dos funcionas necesitan ser usadas en conjunto. Ya que una des
 * hace lo que hace la otra.
 * Ejemplo de uso:
 *  resetScreen 	//Se limpia la selección anterior
 *  selectScreen	//Se crea una nueva selección
 */

/*
 * Genera un cuadro blanco con texto negro entre los puntos pasados
 * como parámetro. Para deseleccionar, llamar a resetScreen.
 */
void selectScreen(char xi, char yi, char xe, char ye);


/*
 * Resetea la selecicón hecha por selectScreen.
 */
void resetSelection();

#endif

/* fin de video.h */
