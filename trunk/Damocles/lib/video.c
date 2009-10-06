/* video.c */


/* Biblioteca standart de video.
 * Permite el manejo de la pantalla y los elementos relacionados con la misma.
 *
 */

#include "../drivers/video/crtc6845.h"
#include "../include/video.h"
#include "../include/math.h"
#include "../include/string.h"
#include "../include/shell.h"
#include "../include/sysasm.h"
#include  "../include/clipboard.h"
#include  "../include/stdio.h"

/* Funci�n que dice qu� hacer cuando clicke�s el bot�n izquierdo*/

static void whenClicked();

/* Funci�n que dice qu� hacer cuando liber�s el bot�n izquierdo*/

static void whenNotClicked();


/*
static int mouseSensibility = 3;
static int mouseThreshold = 10;
static int mouseThresholdy = 5;*/
/*Variable que almacena la coordenada x de la posici�n del cursor
 * cuando se empez� a clickear con el bot�n izquierdo.
 */

char xStart;

/*Variable que almacena la coordenada y de la posici�n del cursor
 * cuando se empez� a clickear el bot�n izquierdo.
 */

char yStart;

/*Variable que almacena la coordenada x actual de la posici�n
 * del cursor del mouse
 * */

char xTemp;

/*Variable que almacena la coordenada y actual de la posici�n
 * del cursor del mouse
 * */

char yTemp;

/*Flag que almacena si el bot�n izquierdo estaba copiado antes.
 */

int wasClicked=0;

/*Variable que almacena cu�ntos caracteres hay en el buffer de
 * copiado.
 */
int copyQty;


 /* TODO:
 * -Codear la funcion que retorna lo que hay entre 2 vertices.
 * -Pintar lo que hay entre 2 vertices.
 * -Despintar.
 * -Escribir donde está el cursor.
 */

/*
 * Limpia la pantalla, seteando fondo negro y letras blancas.
 */
void clearScreen(){
	_vresetpage(0); //TODO: Obtener la pantalla
}

/*
 * Coloca el cursor en las coordenas indicadas. Si la posicion
 * es inválida, el movimiento es ignorado.
 */
void setCursor(char x, char y)
{
	coord_t t;

	if (x < 0 || x > _vcols()-1 || y < 0 || y > _vrows()-1)
		return;
	t.x = x;
	t.y = y;
	_vtsetcursor(0, t); //TODO: Obtener la pantalla
}

char getXc()
{
	return _vgetcursor(0).x; //TODO: Acá se debería obtener la pantalla
}
char getYc(){
	return _vgetcursor(0).y; //TODO: Acá se debería obtener la pantalla
}

void disablePen(){
	_vdisblepen();
}

void setPen(char x, char y)
{
	coord_t t;
	if (x < 0 )
		x = 0;
	else if (x > _vcols()-1)
		x = _vcols()-1;
	if ( y < 0 )
		y = 0;
	else if ( y > _vrows()-1)
		y = _vrows()-1;

	t.x = x;
	t.y = y;
	_vsetpen(t);
}

char getXpen(){
	return _vgetpen().x;
}
char getYpen(){
	return _vgetpen().y;
}

void resetSelection(){
	_vreset();
}

void selectScreen(char xi, char yi, char xe, char ye){
	coord_t begin, end;
	begin.x = xi;
	begin.y = yi;
	end.x = xe;
	end.y = ye;
	_vpaint(begin, end);

}

void copyScreen(char xi, char yi, char xe, char ye){
	//_vcopy(xi,yi,xe,ye);
}

/**
 * Función que recibe un string y lo imprime en la posición del cursor
 * del video. Si es necesario, realiza un scroll. Ignora los \n pero
 * interpreta correctamente los tabs. El flag moveCursor le indica
 * a la funcion si debe o no correr el cursor. El cursor quedará
 * en el siguiente lugar listo para escribir un nuevo caracter.
 * En tiempo de ejecución, el cursor es realmete corrido, pero luego se
 * restaura a su posición original. Esta función puede tener colisiones
 * con la salida estandard, ya que son File Descriptors diferentes.
 */

void writeToCursor(char *str, char moveCursor)
{
	char x = getXc();
	char y = getYc();

	int i;
	while(*str){
		switch(*str){
		case '\n':
			str++; break;
		case '\t':
			str++;
			for(i = 0 ; i < _vtabs() ; i++)
				putToCursor(' ');
			break;
		default:
		putToCursor(*str);
		break;
		}
		str++;
	}

	if (moveCursor == 0)
		setCursor(x, y);
}


/**
 * Esta funcion coloca 1 caracter en la posicion donde se encuentra el cursor.
 * Si el flag de moveCursor es != 0, el cursor será corrido a la siguiente
 * posición y de ser necesario, se realiza un scroll en la pantalla.
 */
void putToCursor(char c){
	_write(CURSOR, &c, 1);
}


void scroll(){
	 _vscroll(0);
}

void updateMouseCursor(MOUSE_DATA * mData)
{


	char x = getXpen();
	char y = getYpen();
	/*static int acumx;
	static int acumy;*/

	/*kprintf("mDATAx %d, mDATAy %d\n",mData->x, mData->y);*/




		/*
	if ( mData->x > abs(mouseThreshold) )
		mData->x = 0;


	if ( mData->y > abs(mouseThresholdy) )
		mData->y = 0;


	acumx += mData->x;
	acumy += mData->y;
	if ( abs(acumx) > mouseSensibility)
	{
			x+=(acumx/mouseSensibility);
			acumx = 0;
	}

	if (abs(acumy) > mouseSensibility)
	{
		y-= (acumy/mouseSensibility);
		acumy=0;
	}
*/
	if (( x + mData->x) >= 0 && ( x + mData->x) < 80 )
		x += mData->x;
	else if ( ( x + mData->x) < 0)
		x += -1;
	else if ( ( x + mData->x) >= 80 )
		x += 1;


	if (( y - mData->y) >= 0 && ( y - mData->y) < 25 )
		y -= mData->y;
	else if ( (y - mData->y) < 0 )
		y -= 1;
	else if ((y -mData->y) >= 25)
		y -= -1;



	setPen( x,y);

	if ( mData-> leftClick )
		whenClicked();
	else
		whenNotClicked();


	if ( mData->rightClick )
		clipboardPaste(STDIN);


	/*kprintf("x %d, y %d\n",x, y);*/
}

int colsQty(){
	return _vcols();
}

int rowQty(){
	return _vrows();
}

void setPage(int p){
	_vsetpage(p);
}
int currentPage(){
	return _vcurrentpage();
}

int pagesQty(){
	return _vpagesqty();
}


void whenClicked()
{
	if(!wasClicked)
	{
		xStart = getXpen();
		yStart = getYpen();
		wasClicked = 1;
	}
	xTemp = getXpen();
	yTemp = getYpen();

	resetSelection();
	selectScreen (xStart, yStart, xTemp, yTemp);
	return;
}




void whenNotClicked ()
{
	if (wasClicked)
	{
		copyScreen(xStart, yStart, xTemp, yTemp);
		resetSelection();
		wasClicked = 0;
	}
	return;
}
