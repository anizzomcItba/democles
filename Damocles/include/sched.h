/*
 * sched.h
 *
 *  Created on: Oct 7, 2009
 */

#ifndef SCHED_H_
#define SCHED_H_

/* Retorna el indice de la tabla global de descriptores que tiene el proceso
 * que la llama.
 */
int schedGetGlobalFd(int fd);
/* Retorna la terminal a la cual está "atacheado" el proceso que la llama */
int schedAttachedTTY();

/* Retorna el pid del proceso que está corriendo en este momento */



int schedCurrentProcess();

void schedExitProcess();

int schedCreateProcess();

void schedSetUp();

void schedTicks();

#endif /* SCHED_H_ */
