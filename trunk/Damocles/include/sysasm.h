#include "defs.h"

#ifndef _SYSASM_H_
#define _SYSASM_H_

void        _lidt (IDTR *idtr);

void		mascaraPIC1 (byte mascara);  /* Escribe mascara de PIC1 */
void		mascaraPIC2 (byte mascara);  /* Escribe mascara de PIC2 */

void		_cli(void);        /* Deshabilita interrupciones  */
void		_sti(void);	 /* Habilita interrupciones  */

void	_out(dword port, dword value);
dword	_in(dword port);

void	_write( int fd, void * buffer, size_t qty );
void 	_read( int fd, void * buffer, size_t qty );

/*
 * Realiza una bajada de los buffers al dispositivo correspondiente. Si esto
 * no se hace, los buffers solamente se actualizan cuando se llenan o cuando
 * les llega un \n.
 */
void	_flush(int fd);



void		int_08_handler();      /* Timer tick */
void		int_80_handler();
void		new_int_80_handler();
void 		int_09_handler();
void		int_74_handler();

int _read_cr0();
int  _read_cr3();
void _write_cr3();
void _write_cr0();

#endif
