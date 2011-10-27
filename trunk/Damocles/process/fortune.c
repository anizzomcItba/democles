#include "fortune.h"
#include "math.h"
#include "stdio.h"

/*
 * Fortune:
 * Pequeña Biblioteca que da un mesaje de la buena suerte, o una cita, etc.
 * Para agregar, simplemente agregar un nuevo elemento al vector. Por
 * convención agrege el comentario de que fortune, para facilitar el
 * debug.
 */


static char *fortunes[] = {
/*1*/ 		"A conservative is a man who believes that nothing\nshould be done for the first time.",
/*2*/		"Captain Hook died of jock itch.",
/*3*/		"640K ought to be enough for anybody. \n\t-Bill Gates",
/*4*/		"If you can't make it good, at least make it look good.\n\t-Bill Gates",
/*5*/		"People everywhere love Windows.\n\t-Bill Gates",
/*6*/		"There are people who don't like capitalism, and people who don't like PCs.\nBut there's no-one who likes the PC who doesn't like Microsoft.\n\t-Bill Gates",
/*7*/		"A person who never made a mistake never tried anything new.\n\t-Albert Einstein",
/*8*/		"All religions, arts and sciences are branches of the same tree.\n\t-Albert Einstein",
/*9*/		"Any man who reads too much and uses his own brain too little falls\ninto lazy habits of thinking.\n\t-Albert Einstein",
/*10*/		"Information is not knowledge.\n\t-Albert Einstein",
/*11*/		"Intellectuals solve problems, geniuses prevent them.\n\t-Albert Einstein",
/*12*/		"Miracles do not, in fact, break the laws of nature.\n\t-C. S. Lewis",
/*13*/		"What saves a man is to take a step. Then another step.\n\t-C. S. Lewis",
/*14*/		"How many Windows programmers does it take to change a light bulb?\n\tFour hundred and seventy two.\n\tOne to write WinGetLightBulbHandle, "
/*15*/		"\n\tone to write WinQueryStatusLightBulb, \n\tone to write WinGetLightSwitchHandle...",
/*16*/		"\tA mathematician, a physicist and an engineer are given an\n\tidentical problem:\n\t"
		"Prove that all odd numbers greater than\n\t2 are prime numbers. They proceed:\n"
		"\n-Mathematician: 3 is a prime, 5 is a prime,\n 7 is a prime, 9 is not a prime -\n"
		" counterexample - claim is false.\n\n-Physicist: 3 is a prime, 5 is a prime,\n 7 is a prime,\n"
		"9 is an experimental error, 11 is a prime, ...\n\n-Engineer: 3 is a prime, 5 is a prime,\n 7 is a prime,"
		"9 is a prime, 11 is a prime, ...",
/*17*/	"Don't force it; get a larger hammer.\n\t-Anthony's Law of Force"
		};

void fortune(void){
	int size = sizeof(fortunes)/sizeof(fortunes[0]);
	kprintf("\n%s\n",fortunes[random(size)]);
}

void fortuneHelp(void){

	kprintf("\n  Alegrese el dia con un mensaje de las galletitas de la fortuna!\n");
}
