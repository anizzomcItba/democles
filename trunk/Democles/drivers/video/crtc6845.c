/* crtc6845.c */

/*
 * Driver de video estandart para PC.
 */


#define VIDEO_ADDRESS 0xb8000
#define VIDEO_COLS 80*2
#define VIDEO_ROWS 25
#define VIDEO_SIZE VIDEO_COLS*VIDEO_ROWS
#define BUFFER_SIZE 128


static char *video = VIDEO_ADDRESS;
static char *buffer[128];

/*
 * Pinta toda la pantalla de negro.
 */
void _vinitscreen(){
		int i;

		for(i = 0 ; i < VIDEO_SIZE - 1 ; i+=2)
		{
			video[i] = ' ';
			video[i+1] = WHITE_TXT;
		}
}

/*
 * Realiza un scroll hacia arriba de toda
 * la pantalla.
 */
void _vscroll(){
	int i, j;

	for (i = 1 ; i < VIDEO_ROWS ; i++)
		for (j = 0 ; j < VIDEO_COLS ; j++)
			buffer[(i-1)*VIDEO_COLS + j] = buffer[i*VIDEO_COLS + j];
}


void * _vgetbuff(){
	return buffer;
}

/*
 * Funcion que se encarga de volcar el buffer de escritura de video
 * en el video mismo.
 */
void _vflush(int qty);


/*
 * Retorna la funcion que se encarga de hacer el vuelco de memoria
 * del buffer al video.
 */
void(*_vgetflush())(int){
	return _vflush;
}
