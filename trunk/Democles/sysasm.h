#include "defs.h"

void            lidt (IDTR *idtr);

void		mascaraPIC1 (byte mascara);  /* Escribe mascara de PIC1 */
void		mascaraPIC2 (byte mascara);  /* Escribe mascara de PIC2 */

void		Cli(void);        /* Deshabilita interrupciones  */
void		Sti(void);	 /* Habilita interrupciones  */

void		int_08_handler();      /* Timer tick */
void		int_80_handler(); 

