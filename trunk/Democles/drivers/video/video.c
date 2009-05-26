#include "video.h"
#include "../../defs.h"


/**
 * Driver de video a medias.
 * Funciona con un buffer, donde se hacen las operaciones y luego se vuelca en la
 * zona de memoria de video con un "flush". La idea es que no se acceda directamente
 * al video.
 * Cada zona del video tiene la configuración <atributo><letra> así que se tienen
 * 80 columnas visibles pero hay 160.
 */

static void flush();
static void scroll();


#define VIDEO_ADDRESS 0xb8000
#define VIDEO_COLS 80*2
#define VIDEO_ROWS 25
#define VIDEO_SIZE VIDEO_COLS*VIDEO_ROWS

static char buffer[VIDEO_SIZE];
static char *video = (char *) VIDEO_ADDRESS;

/**
 * Llena el buffer de video de espacios con fondo negro.
 */

void clearScreen(){
	int i;

	for(i = 0 ; i < VIDEO_SIZE - 1 ; i+=2)
	{
		buffer[i] = ' ';
		buffer[i+1] = WHITE_TXT;
	}
	flush();
}

/**
 * Agrega una linea al final de la pantalla. Si el largo de la linea supera los 80
 * caracteres, el resto se escribe en una linea nueva.
 */

void writeLine(char *str, int qty)
{
	int i;

	for (i = 0 ; i < qty ; i++){
		if (!(i % VIDEO_COLS))
			scroll();

		buffer[24*VIDEO_COLS + (i*2)%48] = str[i];
	}

	flush();
}

/**
 * Vuelca en contenido del buffer en la zona de video.
 */
static void flush(){
	int i;

	for (i = 0 ; i < VIDEO_SIZE ; i++)
		video[i] = buffer[i];
}

/**
 * Realiza un scroll de la pantalla.
 */
static void scroll(){
	int i, j;

	for (i = 1 ; i < VIDEO_ROWS ; i++)
		for (j = 0 ; j < VIDEO_COLS ; j++)
			buffer[(i-1)*VIDEO_COLS + j] = buffer[i*VIDEO_COLS + j];
}
