/*
 * mouse.h
 *
 *  Created on: May 31, 2009
 *      Author: dgrandes
 */

#ifndef MOUSE_H_
#define MOUSE_H_

/*Función que inicializa el mouse para habilitarlo a que reciba paquetes
 * por el puerto 60 a través de la IRQ12. Recibe como parametro una función
 * callback que este ejecutará cuando se haya completado el paquete de datos
 * del mouse. El prototipo de la función callback se encuentra definido en defs.h
 */
void mouseInitialize( mouseCallback callbckAction );

/*El handler de la interrupción de la IRQ12. La rutina de atención 0x74 de la IDT
 * encargada del mouse.
 */
void mouseRoutine(int input );





#endif /* MOUSE_H_ */
