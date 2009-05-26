/* video.h */

#ifndef _VIDEO_H_
#define _VIDEO_H_


/*
 * Driver de Video.
 */

#define WHITE_TXT 0x07

/**
 * Limpia la pantalla. Pone fondo negro y agrega los atributos
 * para que los mensajes escritos salgan en blanco.
 */
void clearScreen();

/**
 * Escribe una linea de texto apuntada por str de longitud qty
 * Todo el contenido de la pantalla es scrolled una linea hacia
 * arriba.
 */
void writeLine(char *str, int qty);


#endif
