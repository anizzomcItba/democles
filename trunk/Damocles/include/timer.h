/*****************************************************************************
			timer.h

******************************************************************************/

#ifndef _timer_
#define _timer_

/*
 * saverInit () 
 *
 * inicializa las variables que necesita el protector de pantalla
 * para funcionar. Setea todo en el default. Es llamada en el kernel
 * cuando inicializa el resto de las cosas.
 * */

void saverInit();


/*
 * setScreensaver (int seconds)  
 * 
 * programa al protector de pantalla 
 * para aparecer reci�n luego de seconds segundos de inactividad.
 *
 * */
void setScreensaver (int seconds);

/*
 * getScreensaver ()  
 * 
 * devuelve la cantidad de segundos de inactividad tras la cual 
 * aparecer� el protector de pantalla.
 * */
int getScreensaver ();

/*
 * resetWaiter ()
 *
 * funci�n que vuelve el contador de segundos a cero e indica que no se est�
 * en modo de protecci�n de pantalla.
 * */
void resetWaiter ();

/*
 * screenSaver()
 *
 * funci�n encargada de entrar en modo protecci�n de pantalla.
 * Guarda el estado de la pantalla, setea el indicador de estado 
 * y muestra el protector.
 * */
void screenSaver ();

/*
 * resetScreen ()
 *
 * funci�n encargada de reestablecer el estado de la pantalla anterior 
 * a entrar en modo protecci�n de pantalla y llamar a resetWaiter.
 * */
void resetScreenSaver();


/*
 * previewSaver()
 *
 * funci�n que entra en modo screenSaver a pedido del usuario.
 * Sirve para que tenga una vista previa y r�pida de c�mo se
 * ver�a el screensaver sin tener que esperar el tiempo determinado.
 * */

void previewSaver();


/*
 * saverLayout ( char * layout)
 *
 * funci�n que modifica el patr�n del salvapantallas. Reemplaza
 * al patr�n anterior por el string apuntado por layout.
 * */

void saverLayout ( char * layout);


/*
 * setCounter(int count)
 *
 * funci�n que modifica el contador de tiempo que pas�. Sirve
 * a fines de implementar previewSaver.
 * */

void setCounter (int count);

#endif
