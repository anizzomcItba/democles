/*
 * keyboard.c
 *
 *  Created on: May 30, 2009
 *      Author: dgrandes
 */

#include "keyboard.h"
#include "../../include/defs.h"
#include "../../include/sys.h"
#include "../../include/sysasm.h"
#include "../../include/kernel.h"
#include "../../include/timer.h"

/*ScanCodes usados con permiso del grupo de Campbell, Rey, Amoros, Arqui 1 cuatrimestre 2008*/
char SCtoASCIInum[0xff] =
{    /*	 0    1	   2    3    4    5    6    7    8    9    A    B    C    D    E     F    */
/*0*/	NOP, ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\'', NOP, '\b', '\t',

/*1*/	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', NOP, '+', '\n', NOP, 'a', 's',

/*2*/	'd', 'f', 'g', 'h', 'j', 'k', 'l', NOP, '{', '}', NOP, '<', 'z', 'x', 'c', 'v',

/*3*/	'b', 'n', 'm', ',', '.', '/', NOP, '*', NOP, ' ', NOP, 0xB3, NOP, NOP, NOP, NOP,

/*4*/	NOP, NOP, NOP, NOP, NOP, NOP, NOP, '7', '8', '9', '-', '4', '5', '6', '+', '1',

/*5*/	'2', '3', '0', '.', NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP

};

char SCtoASCIIplain[0xff] =
{    /*	 0    1	   2    3    4    5    6    7    8    9    A    B    C    D    E     F    */
/*0*/	NOP, ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\'', NOP, '\b', '\t',

/*1*/	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', NOP, '+', '\n', NOP, 'a', 's',

/*2*/	'd', 'f', 'g', 'h', 'j', 'k', 'l', NOP, '{', '}', NOP, '<', 'z', 'x', 'c', 'v',

/*3*/	'b', 'n', 'm', ',', '.', '-', NOP, '*', NOP, ' ', NOP, 0xB3, NOP, NOP, NOP, NOP,

/*4*/	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, 0xC1, NOP, '-', 0xB4, NOP, 0xC3, '+', NOP,

/*5*/	0xC2, NOP, 0xDC, 0xB2, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP

};

char SCtoASCIIshift[0xff] =
{    /*	 0    1	   2    3    4    5    6    7    8    9    A    B    C    D    E     F    */
/*0*/	NOP, ESC, '!', '"', '#', '$', '%', '&', '/', '(', ')', '=', '?', NOP, '\b', NOP,

/*1*/	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', NOP, '*', '\n', NOP, 'A', 'S',

/*2*/	'D', 'F', 'G', 'H', 'J', 'K', 'L', NOP, '[', ']', NOP, '>', 'Z', 'X', 'C', 'V',

/*3*/	'B', 'N', 'M', ';', ':', '_', NOP, NOP, NOP, ' ', NOP, 0xB3, NOP, NOP, NOP, NOP,

/*4*/	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, 0xC1, NOP, '-', 0xB4, NOP, 0xC3, '+', NOP,

/*5*/	0xC2, NOP, 0xDC, 0xB2, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP

};

char SCtoASCIIplainCAPS[0xff] =
{    /*	 0    1	   2    3    4    5    6    7    8    9    A    B    C    D    E     F    */
/*0*/	NOP, ESC, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\'', NOP, '\b', '\t',

/*1*/	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', NOP, '+', '\n', NOP, 'A', 'S',

/*2*/	'D', 'F', 'G', 'H', 'J', 'K', 'L', NOP, '{', '}', NOP, '<', 'Z', 'X', 'C', 'V',

/*3*/	'B', 'N', 'M', ',', '.', '-', NOP, NOP, NOP, ' ', NOP, 0xB3, NOP, NOP, NOP, NOP,

/*4*/	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, 0xC1, NOP, '-', 0xB4, NOP, 0xC3, '+', NOP,

/*5*/	0xC2, NOP, 0xDC, 0xB2, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP
};

char SCtoASCIIshiftCAPS[0xff] =
{    /*	 0    1	   2    3    4    5    6    7    8    9    A    B    C    D    E     F    */
/*0*/	NOP, ESC, '!', '"', '#', '$', '%', '&', '/', '(', ')', '=', '?', NOP, '\b', NOP,

/*1*/	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', NOP, '*', '\n', NOP, 'a', 's',

/*2*/	'd', 'f', 'g', 'h', 'j', 'k', 'l', NOP, '[', ']', NOP, '>', 'z', 'x', 'c', 'v',

/*3*/	'b', 'n', 'm', ';', ':', '_', NOP, NOP, NOP, ' ', NOP, 0xB3, NOP, NOP, NOP, NOP,

/*4*/	NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, 0xC1, NOP, '-', 0xB4, NOP, 0xC3, '+', NOP,

/*5*/	0xC2, NOP, 0xDC, 0xB2, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP, NOP

};

static char buffer[BUFFER_LENGTH];

static char input;

static int buffer_index= START_POS;

static int buffer_start = START_POS;

static int caps = 0;

static int scroll = 0;

static int num = 0;

static int shift = SHIFT_RELEASED;

static int control = 0;

static int specialKey(unsigned char scanCode );

static int isKeypad( unsigned char scanCode );

static int isArrow(unsigned char scanCode );

static char getArrow(unsigned char scanCode );


static void setLEDS( void );

void
keyboardRoutine( unsigned char scanCode )
{
	resetScreenSaver();


	if ( specialKey( scanCode ) )
		return;


	if ( isArrow( scanCode ) )
	{
		/*moveCursor(scanCode);*/
		input = getArrow(scanCode);
		_write(STDIN,&input,1);
		return;
	}

	if ( control && isKeypad(scanCode) && num )
	{
		/*buffer[buffer_index++]= SCtoASCIInum[ scanCode ];*/
		input = SCtoASCIInum[ scanCode ];
		return;
	}
	if ( SCtoASCIIplain[ scanCode ] == NOP )
		return;


	if( shift == SHIFT_PRESSED )
	{
		if ( caps )
		{	/*buffer[ buffer_index++ ] = SCtoASCIIshiftCAPS[ scanCode ];*/
			input  = SCtoASCIIshiftCAPS[ scanCode ];
		}
		else
		{	/*buffer[ buffer_index++ ] = SCtoASCIIshift[ scanCode ];*/
			input = SCtoASCIIshift[ scanCode ];
		}
	}
	else
	{

		if ( caps )
			/*buffer[ buffer_index++ ] = SCtoASCIIplainCAPS[ scanCode ];*/
			input = SCtoASCIIplainCAPS[ scanCode ];
		else
			/*buffer[ buffer_index++ ] = SCtoASCIIplain[ scanCode ];*/
			input = SCtoASCIIplain[ scanCode ];
	}

	if ( buffer_index == BUFFER_LENGTH )
			buffer_index = START_POS;

	_write(STDIN,&input,1);
	return;


}

/*
 * Llamar a getKey solo si  !isBufferEmpty()
 * Si el buffer esta vacío devuelve 0x00
 */

char
getKey()
{

	char key;


	while ( isBufferEmpty());

	key = buffer[buffer_start++];

	if ( buffer_start == BUFFER_LENGTH )
		buffer_start = START_POS;

	return key;
}

int
isBufferEmpty()
{
	return ( buffer_index == buffer_start);
}

int
isBufferReady()
{
	return !isBufferEmpty();
}

void *
_keyGetBuffer()
{
	return (void *) buffer;
}


int(* _keyIsBufferReady())(void){
	return isBufferReady;
}

int
_keyGetBuffSize( )
{
	return BUFFER_LENGTH;
}

int
specialKey(unsigned char scanCode )
{
	switch(scanCode)
	{
		case CAPS_LOCK: caps = 1 - caps;
					setLEDS();
			break;

		case CAPS_LOCKUP :
			break;

		case LSHIFT:
		case RSHIFT:
			shift = SHIFT_PRESSED;
			break;

		case RSHIFTUP:
		case LSHIFTUP:
			shift = SHIFT_RELEASED;
			break;

		case NUM_LOCK:
				num = 1 - num;
				setLEDS();
				break;

		case NUM_LOCKUP:
				break;

		case SCROLL_LOCK:
				scroll = 1 - scroll;
				/*setLEDS();*/
				break;

		case SCROLL_LOCKUP:
				break;

		case CONTROL:
				control = 1;
				break;

		default: return 0;
	}
	return 1;
}


static int
isKeypad(unsigned char scanCode )
{
	return scanCode == 0x35 || scanCode == 0x47 || scanCode == 0x48 || scanCode == 0x49 ||
	       scanCode == 0x4e || scanCode == 0x4b || scanCode == 0x4c || scanCode == 0x4d ||
	       scanCode == 0x4f || scanCode == 0x50 || scanCode == 0x51 || scanCode == 0x52 ||
	       scanCode == 0x53;
}

static int
isArrow( unsigned char scanCode )
{
	return scanCode == ARROW_DOWN || scanCode == ARROW_UP ||
		scanCode == ARROW_LEFT || scanCode == ARROW_RIGHT;
}

static char
getArrow( unsigned char scanCode )
{
	switch( scanCode )
	{
		case ARROW_UP: return UP;
			break;
		case ARROW_DOWN: return DOWN;
			break;
		case ARROW_LEFT: return LEFT;
			break;
		case ARROW_RIGHT: return RIGHT;
			break;

		default: return 0;
	}
	return 0;
}

static void
setLEDS( )
{
	unsigned int led;


	led = ( caps << 2 | num << 1 | scroll );

	_cli();
	_out( KEYBOARD_PORT , SET_INDICATORS );

	while( _in(KEYBOARD_PORT ) != ACK );

	_out( KEYBOARD_PORT, led );
	_sti();

}


