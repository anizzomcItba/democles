/*
 * mouse.c
 *
 *  Created on: May 31, 2009
 *      Author: Dámocles
 */

#include "defs.h"
#include "sysasm.h"
#include "timer.h"
#include "stdio.h"

/*Puerto donde el mouse deja su estado actual
 * Si esta listo para recibir comandos deja el bit 2 en 0
 * Si tiene nueva información disponible deja el bit 1 en 1
 */

#define PORT_STATUS 0x64

/*Definición de ACK del mouse*/
#define ACK 0xFA

/*Función que escribe al mouse, primero indica que se enviara un
 * comando con el código 0xD4 al puerto de estado
 * y luego se envia el comando
 */

static void mouseWrite(unsigned char data);

/*Función que espera que el mouse tenga información nueva antes
 * de leer de el
 */

static unsigned char mouseRead(void);

/* Función que espera con un timeout hasta que el mouse
 * indique con el puerto de estado que esta listo para recibir
 * informaicón
 */

static void waitForMouseToAccept( void );

/*Función que espera que el Mouse tenga nueva información antes
 * de leer cual es esta.
 */

static void waitForMouseToHaveInfo( void );



/*Iterador que maneja la cantidad de bytes de información
 * que se recibió para formar un paquete
 */

unsigned char mouse_cycle=0;


/*Arreglo que almacena los 3 bytes que van llegando para armar
 * el MOUSE_PACKET
 */

char mouse_byte[3];

/*Función callback que recibe el mouse al inicializarse que le indica que hará
 * este cuando se haya formado un paquete de datos.
 */
mouseCallback callbckFunct;

/*Estructura que representa el paquete de datos del mouse.
 * Este contiene la información de la dirección y los botones
 * presionados de este.
 */

MOUSE_DATA mousePacket;



void
mouseInitialize( mouseCallback thisFunction )
{
	unsigned char compaqByte;


	/*El código 0xA8 Habilita el mouse. Primero espero que este
	 * pueda recibir comandos.
	 */
	waitForMouseToAccept();
	_out(0x64,0xA8);

	/*Para indicarle que las interrupciones vendrán por la
	 * IRQ12 necesito configurar el byte COMPAQ.
	 * El comando 0x20 le pide al mouse su byte COMPAQ.
	 */
	waitForMouseToAccept();
	_out(0x64,0x20);


	/*Recibo el byte compaq del puerto 60h y le seteo el bit de 2 en 1 habilitando
	 * la IRQ12 */
	waitForMouseToHaveInfo();
	compaqByte = (_in(0x60) | 2);

	/*Le informo al mouse que le enviare un byte de seteo
	 * con la instrucción 0x60
	 */
	waitForMouseToAccept();
	_out(0x64,0x60);

	/*Le retransmito el byte compaq alterado al puerto 0x60*/
	waitForMouseToAccept();
	_out(0x60, compaqByte);


	/*Le indico al mouse que use las opciones default*/
	mouseWrite(0xF6);
	if (mouseRead() != ACK)
		kprintf("Error al inicializar el mouse");

	/*Habilitación final del mouse*/
	mouseWrite(0xF4);
	if (mouseRead() != ACK )
		kprintf("Error al inicializar el mouse");

	mousePacket.x = 0;
	mousePacket.y = 0;
	callbckFunct = thisFunction;

	return;
}



void
mouseRoutine( unsigned char new_byte)
{

	resetScreenSaver();
	static unsigned char mouse_cycle= 0;
	  static char mouse_bytes[3];
	  if ( new_byte == ACK )
			 return;
	  if ( mouse_cycle == 0)
		  /* Si el cuarto bit no es cero, hay un desfazaje */
		  if (!(new_byte & 0x08 ))
			  return;

	  mouse_bytes[mouse_cycle++] = new_byte;
	  if (mouse_cycle == 3)
	  {
	    mouse_cycle = 0;

	    if ((mouse_bytes[0] & 0x80) || (mouse_bytes[0] & 0x40))
	    {
	    	mousePacket.x = 0;
	    	mousePacket.y = 0;
	    	return;
	    }


	    mousePacket.x = mouse_bytes[1];
	    mousePacket.y = mouse_bytes[2];

	    if ((mouse_bytes[0] & 0x20))
	     {
	    	mousePacket.y  |= 0xFFFFFF00;
	     }
	    if ((mouse_bytes[0] & 0x10))
	    {
	    	mousePacket.x |= 0xFFFFFF00;
	    }
	    if (mouse_bytes[0] & 0x4)
	    	mousePacket.centerClick = 1;
	    else
	    	mousePacket.centerClick = 0;
	    if (mouse_bytes[0] & 0x2)
	        mousePacket.rightClick = 1;
	    else
	    	mousePacket.rightClick = 0;
	    if (mouse_bytes[0] & 0x1)
	       	mousePacket.leftClick = 1;
	    else
	    	mousePacket.leftClick = 0;

	   if(callbckFunct != NULL)
		   callbckFunct(&mousePacket);

	  }
}





static void
waitForMouseToAccept() {
   int timeout = 100000;
   while(timeout--) {
	   /*kprintf("wAccept in 0x64 %x\n",_in(0x64));*/
	   if ((_in(0x64) & 2) == 0)
	   {
		  /* kprintf("timeout accept%x\n",_in(0x64));*/
		   return;
	   }
   }

  /* kprintf("sali del mouse accept timeout = %d\n",timeout);*/

}

static void
waitForMouseToHaveInfo() {
   int timeout = 10000;
   while(timeout--)
     {
       if((_in(0x64) & 1)==1)
		   return;

     }
     return;
}


static unsigned char
mouseRead()
{

	waitForMouseToHaveInfo();
	return (unsigned char)_in(0x60);
}

static void
mouseWrite(unsigned char data)
{
	waitForMouseToAccept();
	_out(0x64, 0xD4);

	waitForMouseToAccept();
	_out(0x60, data);

}


