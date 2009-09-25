/* clipboard.h */


/*
 * Pega el contenido de lo que se encuentre en el portapapeles en el
 * filedescriptor correspondiente.
 * Ejemplo: STDIN, para simular la entrada de teclado.
 */
void clipboardPaste(int fd);

/*
 * Informa cual es la longitud de datos que se encuentran en el
 * portapapeles
 */
int clipboardData();

/*
 * Escribe en el buffer pasado como parámetro, el contenido del
 * portapapeles a partir de un offset hasta un limite.
 */
void clipboardRead(int offset, int limit, void *bufferOut);

/*
 * Retorna el puntero a la función de atención a la copia de información
 * en el clipboard. Recibe como parámetro la cantidad de datos que estan
 * en el.
 */
void(*_cgetcflush())(size_t);
