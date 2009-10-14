#include "drivers/video/crtc6845.h"
#include "include/string.h"
#include "include/defs.h"
#include "include/sched.h"
#include "include/clipboard.h"
#include "include/io.h"
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
 * este tipo de buffer es hasta que se realiza una nueva escritura,
 * sobreescribiendo la información anterior y seteando una marca hasta donde
 * llega la información.
 * El otro tipo, el circular, es aquel en el cual los datos deben ser
 * almacenados hasta que sean leidos y es impresindible que esten ordenados en
 * el orden que llegaron y se mantengan, por lo tanto la vida util es hasta que
 * se realice una lectura del mismo.
 *
 * Luego de realizar una escritura, se le informa al dispositivo/módulo
 * relaciónado con el buffer a traves de un puntero a funcion que hay nueva
 * información disponible en el buffer. Permitiendo en futuras extensiones, que
 * está funcion sea cambiada y se le informe a un dispositivo distinto que hay
 * información.
 */

/*
 * Cantidad de File Descriptors existen el sistema.
 */

#define MAX_FDS 30

/*
 * Tamaño de cada buffer.
 */
#define BUFFER_SIZE	 25*80+25*40
#define KEYBOARD 8


typedef enum {TTY, FILE, TTY_CURSOR} fd_t;

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
	fd_t type;
	char circular;
	char init;
} fdT;


static fdT fdTable[MAX_FDS];



/* Funciones privadas del módulo */

static int isBufferFull(int fd);
static int isBufferEmpty(int fd);
static char bufferRead(int fd);
static void bufferFlush(int fd);



void fdTableInit(){

	fdTable[schedGetGlobalFd(STDOUT)].head = 0;
	fdTable[schedGetGlobalFd(STDOUT)].tail = 0;
	fdTable[schedGetGlobalFd(STDOUT)].bsize = BUFFER_SIZE;
	fdTable[schedGetGlobalFd(STDOUT)].type = TTY;
	fdTable[schedGetGlobalFd(STDOUT)].init = 1;
	fdTable[schedGetGlobalFd(STDOUT)].circular = 1;

	fdTable[KEYBOARD].bsize = BUFFER_SIZE;
	fdTable[KEYBOARD].head = 0;
	fdTable[KEYBOARD].tail = 0;
	fdTable[KEYBOARD].init = 1;
	fdTable[KEYBOARD].circular = 1;

	fdTable[schedGetGlobalFd(CURSOR)].bsize = BUFFER_SIZE;
	fdTable[schedGetGlobalFd(CURSOR)].head = 0;
	fdTable[schedGetGlobalFd(CURSOR)].tail = 0;
	fdTable[schedGetGlobalFd(CURSOR)].type = TTY_CURSOR;
	fdTable[schedGetGlobalFd(CURSOR)].init = 1;
	fdTable[schedGetGlobalFd(CURSOR)].circular = 1;

	fdTable[schedGetGlobalFd(CLIPBOARD)].bsize = BUFFER_SIZE;
	fdTable[schedGetGlobalFd(CLIPBOARD)].head = 0;
	fdTable[schedGetGlobalFd(CLIPBOARD)].tail = 0;
	fdTable[schedGetGlobalFd(CLIPBOARD)].circular = 0;
	fdTable[schedGetGlobalFd(CLIPBOARD)].init = 1;

}

void syswrite(int fd, char *buffIn, size_t qty){

	int i;
	int globalfd = schedGetGlobalFd(fd);


	if(!fdTable[globalfd].init){
		//This should kill the caller
		return;
	}

	if(fdTable[globalfd].circular){
		for(i = 0 ; i < qty ; i++){
			if(buffIn[i] == '\n' || isBufferFull(globalfd))
				bufferFlush(globalfd);
			bufferAdd(globalfd, buffIn[i]);
		}
	}else
	{
		i = (qty < fdTable[globalfd].bsize)? qty : fdTable[globalfd].bsize;
		memcpy(fdTable[globalfd].buffer, buffIn, i);
		fdTable[globalfd].tail = i;
		bufferFlush(globalfd);
	}
}

void sysread(int fd, char *buffOut, size_t qty){
	int i;

	int globalfd = schedGetGlobalFd(fd);

	if(fd > MAX_FDS || !fdTable[globalfd].init){
		//This should kill the caller
		return;
	}


	if (fdTable[globalfd].circular){
		for(i = 0 ; i < qty ; i++){
			while(isBufferEmpty(globalfd)){ /*_sleep(); */};
			buffOut[i] = bufferRead(globalfd);
		}
	} else
	{
		i = (qty < i)? qty : fdTable[globalfd].tail;
		memcpy(buffOut, fdTable[globalfd].buffer, i);
	}

}

int sysflush(int fd){
	bufferFlush(schedGetGlobalFd(fd));
	return 1;
}

static int isBufferFull(int fd){
	return (fdTable[fd].tail + 1)%fdTable[fd].bsize
		== fdTable[fd].head%fdTable[fd].bsize;
}

void bufferAdd(int fd, char c){

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
	int i = 0;
	char buffer[BUFFER_SIZE];

	if(fdTable[fd].circular){
		while(!isBufferEmpty(fd))
			buffer[i++] = bufferRead(fd);
	}
	else
		memcpy(buffer, fdTable[fd].buffer, fdTable[fd].tail);


	switch(fdTable[fd].type){
		case TTY:
			_vtflush(schedAttachedTTY(), buffer, i);
			break;
		case TTY_CURSOR:
			_vtcflush(schedAttachedTTY(), buffer, i);
			break;
		case FILE:
			//TODO: Implementar.
			break;
	}
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
