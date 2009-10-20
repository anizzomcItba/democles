/* semaphore.h */

#ifndef __SEMAPHORE_H_
#define __SEMAPHORE_H_

/* Setea la configuración de semaforos */
void semSetup();

/* Decrementa un semaforo. Si se llega a < 0 el proceso se bloquea
 * y cede el micro.
 * Si no se ha podido dormir, retorna un valor de error.
 */
int semDec(int id);

/* Incrementa el valor del semaforo, si hay procesos durmiendo en
 * este semaforo, son despertados en el orden que se durmieron */
void semInc(int id);

/* Retorna una ID libre para usar el semaforo con un valor inicial */
int semGetID(int value);

/* Le retorna al sistema un ID para ser usada en otro lugar */
void semRetID(int id);

#endif
