/* clipboard.h */


/*
 * Pega el contenido de lo que se encuentre en el portapapeles en el
 * filedescriptor correspondiente.
 * Ejemplo: STDIN, para simular la entrada de teclado.
 */
void clipboardPaste(void);

/*
 * Informa cual es la longitud de datos que se encuentran en el
 * portapapeles
 */
int clipboardData();

/*
 * Devuelve el puntero al clipBuffer
 */
void * getClipBuffer();

/*
 * Setea el tamaño del contenido del buffer del clipboard
 */
void setDataSize(int size);
