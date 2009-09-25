#include "include/sys.h"
#include "drivers/video/crtc6845.h"
#include "include/string.h"
#include "include/defs.h"
#include "include/clipboard.h"
#include "include/sysasm.h"

/*
 * Módulo encargado del manejo de los filedescriptors
 * El manejo es por buffers, donde estos se pueden leer y escribir. Hay de 2
 * tipos.
 * Un manejo lineal y un manejo circular.
 * El manejo lineal es usado para aquellos datos que no tiene sentido de forma
 * fragmentada, ya mezclar datos perdería el sentido y dificultaria notablemente
 * el recupero de la información con integridad. Este es el ejemplo de un porta-
 * papeles, donde mezclar las diferentes copias de los procesos simplemente
 * llevaría al caos y a la complejidad inneceseria de código. La vida útil de
 * este tipo de buffer es hasta que se realiza una nueva escritura, sobreescribiendo
 * la información anterior y seteando una marca hasta donde llega la información.
 * El otro tipo, el circular, es aquel en el cual los datos deben ser almacenados
 * hasta que sean leidos y es impresindible que esten ordenados en el orden que
 * llegaron y se mantengan, por lo tanto la vida util es hasta que se realice una
 * lectura del mismo.
 *
 * Luego de realizar una escritura, se le informa al dispositivo/módulo relaciónado
 * con el buffer a traves de un puntero a funcion que hay nueva información
 * disponible en el buffer. Permitiendo en futuras extensiones, que está funcion
 * sea cambiada y se le informe a un dispositivo distinto que hay información.
 */

/*
 * Cantidad de File Descriptors existen el sistema.
 */

#define MAX_FDS 5

/*
 * Tamaño de cada buffer.
 */
#define BUFFER_SIZE	 25*80+25*40

/*
 * Estructura del FileDescriptor:
 * -buffer: la zona temporal en la cual se van a escribir los datos.
 * -bsize: tamaño del buffer. Más allá de que el tamaño de todos los buffers sen
 * iguales, futuras extensiones permitirían que no sea así, por lo tanto
 * es necesario que haya una relación unica con el buffer y su tamaño.
 * -head: offeset desde el comienzo del buffer al primer dato que tenga sentido
 * en el buffer.
 * -tail: puntero al proximo lugar libre dentro del buffer, en el caso del buffer
 * linal, esto representa la cantidad de información contenido en el mismo.
 * -flush: función de noticificación de que hay información disponible en el buffer,
 * si esta es nula, no se toma ninguna acción.
 * -circular: flag que informa con que tipo de buffer se está trabajando.
 * -init: flag de inicialización, indica si un FD está inicializado o no.
 *
 */

typedef struct{
	char buffer[BUFFER_SIZE];	//Buffer
	int bsize; //Tanaño del buffer
	int head; //Cabeza del buffer. Apunta al primer caracter
	int tail;	//Cola del buffer. A la proxima posicion libre | Tamaño de datos.
	void (*flush)(size_t);	//Funcion de flush
	char circular;
	char init;
} fdT;


static fdT fdTable[MAX_FDS];
static char *err = "This file descriptor is NOT INITIALIZED!\n";


/* Funciones privadas del módulo */

static int isBufferFull(int fd);
static int isBufferEmpty(int fd);
static char bufferRead(int fd);
static void bufferAdd(int fd, char c);
static void bufferFlush(int fd);

void fdTableInit(){

	fdTable[STDOUT].head = 0;
	fdTable[STDOUT].tail = 0;
	fdTable[STDOUT].bsize = BUFFER_SIZE;
	fdTable[STDOUT].flush =_vgetflush();
	fdTable[STDOUT].init = 1;
	fdTable[STDOUT].circular = 1;

	fdTable[STDIN].bsize = BUFFER_SIZE;
	fdTable[STDIN].head = 0;
	fdTable[STDIN].tail = 0;
	fdTable[STDIN].flush = NULL;
	fdTable[STDIN].init = 1;
	fdTable[STDIN].circular = 1;

	fdTable[CURSOR].bsize = BUFFER_SIZE;
	fdTable[CURSOR].head = 0;
	fdTable[CURSOR].tail = 0;
	fdTable[CURSOR].flush = _vgetcflush();
	fdTable[CURSOR].init = 1;
	fdTable[CURSOR].circular = 1;

	fdTable[CLIPBOARD].bsize = BUFFER_SIZE;
	fdTable[CLIPBOARD].head = 0;
	fdTable[CLIPBOARD].tail = 0;
	fdTable[CLIPBOARD].flush = _cgetcflush();
	fdTable[CLIPBOARD].circular = 0;
	fdTable[CLIPBOARD].init = 1;

	fdTable[MOUSE].bsize = BUFFER_SIZE;
	fdTable[MOUSE].head = 0;
	fdTable[MOUSE].tail = 0;
	fdTable[MOUSE].flush = NULL;
	fdTable[MOUSE].circular = 0;
	fdTable[MOUSE].init = 0;


}

void syswrite(int fd, char *buffIn, size_t qty){

	int i;

	if(!fdTable[fd].init){
		_write(STDOUT, err, strlen(err));
		return;
	}

	if(fdTable[fd].circular != 0){
		for(i = 0 ; i < qty ; i++){
			if(buffIn[i] == '\n' || isBufferFull(fd))
				bufferFlush(fd);
			bufferAdd(fd, buffIn[i]);
		}
	}else
	{
		i = (qty < fdTable[fd].bsize)? qty : fdTable[fd].bsize;
		memcpy(fdTable[fd].buffer, buffIn, i);
		fdTable[fd].tail = i;
		bufferFlush(fd);
	}
}

void sysread(int fd, char *buffOut, size_t qty){
	int i;

	if(fd > MAX_FDS || !fdTable[fd].init){
		_write(STDOUT, err, strlen(err));
		return;
	}


	if (fdTable[fd].circular != 0){
		for(i = 0 ; i < qty ; i++){
			while(isBufferEmpty(fd));
			buffOut[i] = bufferRead(fd);
		}
	} else
	{
		i = (qty < i)? qty : fdTable[fd].tail;
		memcpy(buffOut, fdTable[fd].buffer, i);
	}

}

void sysflush(int fd){
	bufferFlush(fd);
}

static int isBufferFull(int fd){
	return (fdTable[fd].tail + 1)%fdTable[fd].bsize == fdTable[fd].head%fdTable[fd].bsize;
}

static void bufferAdd(int fd, char c){

	/* Me posiciono en la proxima posicion */
	int t = fdTable[fd].tail;

	/* Si el buffer esta lleno, no debo escribir */
	/* Escribo en la proxima posicion y actualizo el fd */
	if(!isBufferFull(fd)){
		fdTable[fd].buffer[t++] = c;
		fdTable[fd].tail = t%fdTable[fd].bsize;
	}
	return;
}


static void bufferFlush(int fd){

	int qty;
	int h;
	int t;

	if(fdTable[fd].circular) {
		qty = 0;
		h = fdTable[fd].head % fdTable[fd].bsize;
		t = fdTable[fd].tail % fdTable[fd].bsize;

		if(!isBufferEmpty(fd))
			qty = (t < h)? fdTable[fd].bsize -(h-t) : (t-h);

	}
	else{
		qty = fdTable[fd].tail;
	}

	if (fdTable[fd].flush != NULL)
		fdTable[fd].flush(qty);
}

static int isBufferEmpty(int fd){
	return fdTable[fd].head == fdTable[fd].tail;
}

static char bufferRead(int fd){

	int h = fdTable[fd].head % fdTable[fd].bsize;
	char ret;

	ret = ((char*)fdTable[fd].buffer)[h];
	fdTable[fd].head = (h+1)%fdTable[fd].bsize;

	return ret;
}




