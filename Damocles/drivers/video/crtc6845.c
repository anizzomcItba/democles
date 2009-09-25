/* crtc6845.c */

#include "crtc6845.h"
#include "../../include/sys.h"
#include "../../include/sysasm.h"

/*
 * Driver de video estandard para PC.
 *
 * Exporta funciones para el manejo de la pantalla.
 *
 * Funcionamiento básico:
 * Funciona con un buffer, donde se debe copiar lo que se quiere mostrar
 * en la pantalla, luego, llamar a la funcion _vflush para indicarle al
 * driver de video que hay información lista para volcar en la pantalla,
 * en la ultima linea.
 */

/*
 * Definiciones:
 * 	VIDEO_ADDRESS: dirección física donde está mapeada la zona de
 * video.
 * 	VIDEO_COLS: Cantidad de columnas que tiene la pantalla. Tiene
 * 80 posiciones mostrables y otras 80 de attributos de la forma:
 * <caracter><atributo>
 * 	VIDEO_FILS: Cantidad de filas que posee.
 *	VIDEO_SIZE: Memoria total de la zona de video.
 *	BUFFER_SIZE: tamaño del buffer de lectura.
 *	TAB_SPACE: tamaño de los tabs. Se puede cambiar con la funcion
 *	_vsettab;
 */



#define VIDEO_ADDRESS 0xb8000
#define VIDEO_COLS 80
#define VIDEO_ROWS 25
#define VIDEO_SIZE VIDEO_COLS*VIDEO_ROWS*2
#define BUFFER_SIZE VIDEO_SIZE/2
#define TAB_SPACE 4
#define CANT_PAGES 2




/*
 * Asigación de la memoria de video. Permite trabajarhela como un
 * array unidemensional.
 */
static char *video = (char *)VIDEO_ADDRESS;

static int tabs = TAB_SPACE;

/*
 * Posiciones de cursorores.
 * xcursor e ycursor guardan la posición
 * del cursor de hardware.
 */
static char xcursor;
static char ycursor;

/*
 * Posiciones del cursor luminoso
 * xpen e ypen guardan las posiciones.
 */
static char xpen = -1;
static char ypen = -1;

static int activePage = 0;
static char pages[CANT_PAGES][VIDEO_SIZE];
static int lpos[CANT_PAGES] = {0};


/*
 * line, vector utilizado para marcar las lineas que deben actualizarce a la hora
 * de realizar un vuelco de las páginas activas a la zona de video.
 * Si se realiza un scroll, se actualizan TODAS las lineas y se desmarcan.
 */
static char line[VIDEO_ROWS] = {0}; /* Indicador de que linea se debe actualizar */
static char scrolled = 0;	/* indicador si se realizó un scroll */


/* Declaración de funciones privadas. */


/* ---------------------------------------------------------------------------
 * inv
 * ---------------------------------------------------------------------------
 * uso:
 *  newatrr = inv(oldattr);
 * ---------------------------------------------------------------------------
 * Esta función invierte el attributo pasado como parámetro.
 * Por ejemplo, si se tiene fondo negro y letras blancas, devuelve
 * fondo blanco y letras negras.
 *
 */

static void pageScroll();
static void _vtoscreen();
static char inv(char attr);
static void copyLine(int i);


void _vsetpage(int p){
	//TODO: Arreglar el tema del mouse y el lposs

	if(!(0 <= p && p < CANT_PAGES))
		return;

	activePage = p;
	_vsetpen(-1, -1);
	scrolled = 1;
	_vtoscreen();
}


/* Funciones exportadas */

/* ---------------------------------------------------------------------------
 * _vinitscreen
 * ---------------------------------------------------------------------------
 * Inicializa la pantalla, limpiandola completamente y declarando
 * todo los atributos como blancos con fondo negro.
 */
void _vinitscreen(){
	int i;
	for(i = 0 ; i < VIDEO_SIZE - 1 ; i+=2)
	{
		pages[activePage][i] = ' ';
		pages[activePage][i+1] = WHITE_TXT;
	}
	lpos[activePage] = 0;
	scrolled = 1;
	_vtoscreen();

}

/*
 * Resetea la pantalla a el fondo negro y texto blanco.
 */
void _vreset(){
	int i;

	char x = _vgetxpen();
	char y = _vgetypen();

	for(i = 0 ; i < VIDEO_SIZE - 1 ; i+=2)
	video[i+1] = WHITE_TXT;

	_vsetpen(-1, -1);
	_vsetpen(x, y);

}


/*
 * Realiza un scroll hacia arriba de toda la pantalla. Copia
 * la fila i en la fila i-1 y coloca espacios en la ultima.
 */
void _vscroll(){

	pageScroll();
	_vtoscreen();
}

/*
 * Realiza un scroll dentro de la página, pero sin impactarlo en la pantalla.
 */
static void pageScroll(){

		int i, j;

		for (i = 1 ; i < VIDEO_ROWS ; i++)
			for (j = 0 ; j < VIDEO_COLS ; j++)
				pages[activePage][(i-1)*VIDEO_COLS*2 + 2*j] = pages[activePage][i*VIDEO_COLS*2 + 2*j];
		for(i = 0 ; i < VIDEO_COLS ; i++)
			pages[activePage][24*VIDEO_COLS*2 + i*2] = ' ';
		lpos[activePage] = 0;
		scrolled = 1;
}

/*
 * Funcion que se encarga de volcar el buffer de escritura de video
 * en el video mismo.
 *
 * Cuando llega hasta el limite de la pantalla, se escrolea.
 * Los caracteres especiales son salteados y reemplazados por
 * su significado.
 */
void _vflush(size_t qty){

	char buffer[80*25];
	_read(STDOUT, buffer, qty);

	int i;
	int j;
	for (i = 0 ; i < qty ; i++)
	{
		switch(buffer[i]){
			case '\n':
				pageScroll();
				lpos[activePage] = 0;
				break;
			case '\t':
				for(j = 0; j < tabs ; j++){
					pages[activePage][24*VIDEO_COLS*2 + 2*(lpos[activePage]++)] = ' ';
					line[24] = 1;
					if(lpos[activePage] >= VIDEO_COLS){
						pageScroll();
						lpos[activePage] = 0;
					}
				}
				break;
			default:
				line[24] = 1;
				pages[activePage][24*VIDEO_COLS*2 + 2*(lpos[activePage]++)] = buffer[i];
				break;

		}
		if (lpos[activePage] >= VIDEO_COLS){
			pageScroll();
			lpos[activePage] = 0;
		}
	}
	_vtoscreen();
}
/*
 * Retorna la funcion que se encarga de hacer el vuelco de memoria
 * del buffer al video.
 */
void(*_vgetflush())(size_t){
	return _vflush;
}

/*
 * Retorna el tamaño del buffer de video. Si se quiere imprimir
 * algo mayor, se debe realizar multiples escrituras. Caso
 * contrario puede producir errores.
 */
size_t _vgetbuffsize(){
	return BUFFER_SIZE;
}

/*
 * Setéa la posicion del cursor indicada x lugares del margen izquiero
 * e y lugares desde el superior. La posición se calcula como:
 *  p = (y*row) + x.
 *  Luego se carga la parte baja en 0x0F de este numero y la parte
 *  alta en 0x0E
 *  Si se envian coordenadas fuera de la pantalla, el cursor se coloca
 *  en la posición 0;0
 */
void _vsetcursor(char x, char y){

	if(0 > x || VIDEO_COLS-1 < x || y < 0 || VIDEO_ROWS-1 < y){ /* Estoy fuera de la pantalla */
		x = y = 0;
	}
	int res = y*VIDEO_COLS + x;

	xcursor = x;
	ycursor = y;

	_out(0x3D4, 0x0F);
	_out(0x3D5, res & 0xFF);
	_out(0x3D4, 0x0E);
	_out(0x3D5, (res>>8)&0xFF);
}

/*
 * Retorna la coordenada x del cursor de hardware.
 */
char _vgetxcursor(){
	return xcursor;
}

/*
 * Retorna la coordenada y del cursor de hardware.
 */
char _vgetycursor(){
	return ycursor;
}

/*
 * Retorna la coordenada x del puntero luminoso.
 */
char _vgetxpen(){
	return xpen;
}

/*
 * Retorna la coordenada y del puntero luminoso.
 */
char _vgetypen(){
	return ypen;
}

/*
 * Cambia la posición del puntero luminoso indicando su posición
 * invirtiendo los colores en la pantalla.
 * Si se manda el light pen fuera de la pantalla, el mismo se desactiva
 */
void _vsetpen(char x, char y){
	if (y < 0 || y > VIDEO_ROWS-1 || x < 0 || x > VIDEO_COLS - 1){ /* Light pen fuera de los límites */
		if (ypen != -1)
			video[ypen*VIDEO_COLS*2 + xpen*2 + 1] = WHITE_TXT;

		ypen = xpen = -1;
	}
	else
	{
		if(ypen != -1)
			video[ypen*VIDEO_COLS*2 + xpen*2 + 1] = inv(video[ypen*VIDEO_COLS*2 + xpen*2 + 1]);
		video[y*VIDEO_COLS*2 + x*2 + 1] = inv(video[y*VIDEO_COLS*2 + x*2 + 1]);
		xpen = x;
		ypen = y;
	}
}


/**
 * Pinta la pantalla invirtiendo los colores desde el punto xi;yi hasta el punto
 * xe, yi
 */
void _vpaint(char xi, char yi, char xe, char ye)
{
	char xstart, xend, ystart, yend;

	char xorg;

	(xi <= xe)? (xstart = xi, xend = xe) : (xstart = xe, xend = xe);
	(yi <= ye)? (ystart = yi, yend = ye) : (ystart = ye, yend = yi);

	for(; ystart <= yend ; ystart++ )
		for(xorg = xstart ; xorg <= xend ; xorg++)
			video[ystart*VIDEO_COLS*2 + xorg*2 + 1] = inv(video[ystart*VIDEO_COLS*2 + xorg*2 + 1]);
}


/**
 * Pinta el rectangulo. NO INVIERTE LOS COLORES.
 */
void _vselect(char xi, char yi, char xe, char ye)
{

	char xstart, xend, ystart, yend;

	char xorg;

	(xi < xe)? (xstart = xi, xend = xe) : (xstart = xe, xend = xi);
	(yi < ye)? (ystart = yi, yend = ye) : (ystart = ye, yend = yi);

	for(; ystart <= yend ; ystart++ )
		for(xorg = xstart ; xorg <= xend ; xorg++)
			video[ystart*VIDEO_COLS*2 + xorg*2 + 1] = INV;
}

/*
 * Invierte los attributos.
 * Salva los datos de parpadeo y luminosidad, pero intercambia
 * el color de fondo con el color de la letra.
 */
static char inv(char attr){
	char low;
	char high;
	char bb;	//Parpadeo y Brillo


	/* Salva el parpadeo y el brillo */

	bb = attr & 0x88;

	/*Toma la parte alta y la baja del atributo. */

	low = attr & 0x07;

	high = attr & 0x70;

	/* Las intercambia y las vuelve a reeconstruir */

	low = low<<4;
	high = high>>4;

	return bb | low | high;
}

/**
 * Copia lo que hay en la caja delimitada por los vertices xi;yi y xe:ye en
 * el buffer de salida estandart. Retorna la cantidad de bytes copiados y
 * finaliza la cadena con un NULL TERMINATED.
 * TODO: Como hacer que no ponga un '\n' al final?
 */
void _vcopy(char xi, char yi, char xe, char ye)
{
	char xstart, xend, ystart, yend;

	char xorg;
	char bufferOut[VIDEO_SIZE/2 + VIDEO_ROWS + 1]; //Solo copiamos caracteres, pero agregamos \n
	int i;

	(xi <= xe)? (xstart = xi, xend = xe) : (xstart = xe, xend = xi);
	(yi <= ye)? (ystart = yi, yend = ye) : (ystart = ye, yend = yi);

	for(i = 0; ystart <= yend ; ystart++ )
	{
		for(xorg = xstart ; xorg <= xend ; xorg++, i++)
			bufferOut[i] = pages[activePage][ystart*VIDEO_COLS*2 + xorg*2];
		bufferOut[i++] = '\n';
	}


	_write(CLIPBOARD, bufferOut, i);
}

/*
 * Realiza una escritura a la posición donde se encuentre el cursor en ese momento.
 * Saltea los caracteres especiales.
 */


void _vcflush(size_t qty){

	char bufferIn[VIDEO_SIZE];
	int x = _vgetxcursor();
	int y = _vgetycursor();
	int i, j;

	_read(CURSOR, bufferIn, qty);

	for(i = 0 ; i < qty ; i++){

		switch(bufferIn[i]){
		case '\n':
			x = 0;
			y++;
			break;
		case '\r':
			x = 0;
			break;
		case '\t':
			for(j = 0 ; j < TAB_SPACE ; j++){
				line[y] = 1;
				pages[activePage][(y*VIDEO_COLS+ x)*2] = ' ';
				x++;
				//TODO: Mejorar este código
				if(x >= VIDEO_COLS){
					x = 0;
					y++;
				}

				if(y >= VIDEO_ROWS){
					pageScroll();
					y = VIDEO_ROWS -1;
				}

				_vsetcursor(x, y);
			}

			break;


		default:
			line[y] = 1; //Marco la linea y para actualizarla
			pages[activePage][(y*VIDEO_COLS+ x)*2] = bufferIn[i];
			x++;
			break;
		}

			if(x >= VIDEO_COLS){
				x = 0;
				y++;
			}

			if(y >= VIDEO_ROWS){
				pageScroll();
				y = VIDEO_ROWS -1;
			}

			_vsetcursor(x, y);

	}
	_vtoscreen();

}


void(*_vgetcflush())(size_t){
	return _vcflush;
}


int _vcols(){
	return VIDEO_COLS;
}

int _vrows(){
	return VIDEO_ROWS;
}

int _vtabs(){
	return tabs;
}

/**
 * Realiza una copia de la linea i, de la página activa en la zona de memoria
 * del video.
 */
static void copyLine(i){
	int j;

	for(j = 0 ; j < VIDEO_COLS*2 ; j++)
		video[i*VIDEO_COLS*2 + j] = pages[activePage][i*VIDEO_COLS*2 +j];
}

/* ---------------------------------------------------------------------------
 * Realiza la actualización de el video deacuerdo al contenido de la
 * página activa.
 * Si se realizó un scroll, es preciso actualizar TODAS las lineas, ya que
 * el contenido cambio para toda la pantalla.
 * Sinó, unicamente se realiza una actualización de las líneas marcadas.
 */
static void _vtoscreen(){
	int i;

	if(scrolled){
		for(i = 0 ; i < VIDEO_ROWS ; i++)
			copyLine(i);
	}
	else
	{
		for (i = 0 ; i < VIDEO_ROWS ; i++){
			if(line[i]){
				copyLine(i);
				line[i] = 0;
			}
		}
	}
	for(i = 0 ; i < VIDEO_ROWS ; i++) line[i] =0 ;

	if(ypen != -1)
		video[ypen*VIDEO_COLS*2 + xpen*2 + 1] = INV; //Coloco el cursor donde estaba
	scrolled = 0;
}

int _vcurrentpage(){
	return activePage;
}

int _vpagesqty(){
	return CANT_PAGES;
}
