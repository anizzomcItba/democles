/*
 * keyboard.h
 *
 *  Created on: May 30, 2009
 *      Author: dgrandes
 */

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#define BUFFER_LENGTH 4
#define START_POS 0

void keyboardRoutine( unsigned char scanCode );

char getKey( void );

int isBufferEmpty( void );

void * _keyGetBuffer( void );

int _keyGetBuffSize( void );

int(* _keyIsBufferReady())(void);

#define KEYBOARD_PORT 0x60
#define ACK	0xFA
#define SET_INDICATORS 0xED

#define ARROW_UP 0x48
#define ARROW_DOWN 0x50
#define ARROW_LEFT 0x4B
#define ARROW_RIGHT 0x4D

/*Son codigos para ya que los scanCodes pisan con valores ASCII*/
#define UP 28
#define DOWN 29
#define LEFT 30
#define RIGHT 31

#define BACKSPACE '\b'
#define DELETE 0xB2
#define INSERT 0xDC

#define NOP 0x00

#define RSHIFT 	0x36
#define RSHIFTUP 0xB6

#define LSHIFT 	0x2A
#define LSHIFTUP 0xAA

#define CAPS_LOCK 0x3A
#define CAPS_LOCKUP 0xBA

#define SHIFT_PRESSED 1
#define SHIFT_RELEASED 0


#define CONTROL 0xE0
#define ESC 	0x01

#define F1	0xB3
#define F2	0x3c
#define F3	0x3d
#define F4	0x3e

#define F5	0x3f
#define F6	0x40
#define F7	0x41
#define F8	0x42

#define F9	0x43
#define F10	0x44
/*#define F11
#define F12	*/

#define NUM_LOCK 0x45
#define NUM_LOCKUP 0xC5

#define SCROLL_LOCK 0x46
#define SCROLL_LOCKUP 0xc6

/*defino la variable que indica si hay que poner el protector de pantalla*/



#endif /* KEYBOARD_H_ */
