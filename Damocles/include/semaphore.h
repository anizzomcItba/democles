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

/* Consume un wakeup del semaforo, no lo decrementa a menos de 0, utilizarlo
 * solo en casos en los que no se desee dormir en un semaforo, pero debería
 * consumir el wake.
 * Debería estar dentro de semDec cómo opcion, pero resultó tarde la necesidad
 */
void semConsume(int id);

/* Le retorna al sistema un ID para ser usada en otro lugar */
void semRetID(int id);

#endif
