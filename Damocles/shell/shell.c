/*
 * shell.c
 *
 */

#include "shell.h"

#include "stdio.h"
#include "string.h"
#include "defs.h"
#include "io.h"
#include "process.h"
#include "timer.h"
#include "video.h"
#include "sysasm.h"
#include "syscall.h"
#include "fortune.h"
#include "mmu.h"
#include "filesystem.h"
#include "tree.h"
#include "ls.h"
#include "cd.h"
#include "cat.h"
#include "open.h"
#include "rm.h"
#include "mkdir.h"
#include "write.h"

#define SHELL_BUFFER_LENGTH 101



//XXX
void debug(void);

/*Longitud maxima de un comando*/
#define MAX_COMM_LENGTH				100
/*Maxima cantidad distinta de flags que puede tener
 *  un comando */
#define MAX_FLAGS_QTY				8
/*Maxima longitud de un flag*/
#define MAX_FLAG_LENGTH				40
/*Maxima longitud de los parametros*/
#define MAX_PARAMS_LENGTH			40


/*Lo tengo definido para interpretar las flechas*/
#define UP 28
#define DOWN 29
#define LEFT 30
#define RIGHT 31

#define PROMPT "user@damocles:"

/*La cantidad de Comandos en Damocles*/
#define COMM_QTY			16

/*Cantidad de flags que tiene screensaver*/
#define SSAVERFLAGS			3
/*Cantidad de flags que tiene shutdown*/
#define SDOWNFLAGS			1
/*Cantidad de flags que tiene la funcion de test*/
#define TESTFLAGS			2
/*Cantidad de flags que tiene help*/
#define HELPFLAGS			COMM_QTY
/*Cantidad de flags que tiene clear*/
#define CLEARFLAGS			1


#define FORTUNEFLAGS		1
#define DEBUGFLAGS			1
#define CAT_FLAGS			1
#define LS_FLAGS			1
#define CD_FLAGS			1
#define TREE_FLAGS			1
#define RM_FLAGS			1
#define OPEN_FLAGS			1
#define MKDIR_FLAGS			1
#define WRITE_FLAGS			4



static char shellBuffer[SHELL_BUFFER_LENGTH];


static int index = 0;
static int posOtroComando;

/*Codigo del comando, este va a representar su offset en
 * el arreglo de comandos myCommand, por eso se tiene que
 * respetar el orden*/
#define COMMAND_SCREENSAVER			0
#define COMMAND_TEST				1
#define COMMAND_HELP				2
#define COMMAND_SHUTDOWN 			3
#define COMMAND_CLEAR				4
#define	COMMAND_FORTUNE				5
#define COMMAND_DEBUG				6
#define COMMAND_CAT					7
#define COMMAND_LS					9
#define COMMAND_CD					10
#define COMMAND_RM					11
#define COMMAND_TREE				12
#define COMMAND_OPEN				13
#define COMMAND_MKDIR				14
#define COMMAND_WRITE				15



static void backSpace(void);
static void getCommandFromPrompt(char * buffer );
static int getFlagsFromPrompt(char * buffer);
static void getParamsFromPrompt( char * buffer, int index );
static void executeCommand(int command);


static void fetchCommand(char flecha);
static int isArrow( unsigned char scanCode );
static void commitCommand( void );



/*Arreglo que contiene el ultimo comando ingresado,
 * este es seteado por la funcion getCommandFromPrompt
 */
static char command[ MAX_COMM_LENGTH ];

#define MAX_COMMANDS_HIST 20

static char history[MAX_COMMANDS_HIST][MAX_COMM_LENGTH];


static int hist_index = 0;
static int hist_pos = 0;
static int unsaved_comm = 1;

/*Arreglo que contiene el flag ingresado
 * este es seteado por getFlagsFromPrompt
 */
static char flags[MAX_FLAG_LENGTH];

/*Arreglo que contiene los parametros
 * separados por espacios
 * este es seteado por getParamsFromPrompt
 */
static char params[MAX_PARAMS_LENGTH];

/* Puntero a funcion del comando*/
typedef void(*commandFnct)(void);

/*Estructura que define un comando:
 * command contiene el comando en texto
 * code es su codigo
 * la matriz de flags tiene todas las flags del comando
 * qtyflags indica la cantidad de flags del comando
 * exec es un puntero de funcion a este comando
 */
typedef struct{
	char  command[MAX_COMM_LENGTH];
	int code;
	char flags[MAX_FLAGS_QTY][MAX_FLAG_LENGTH];
	int qtyflags;
	commandFnct exec;
}Command;



/*Arreglo que contiene todos los comandos,
 * este es formado por populateCommands
 */
static Command myCommands[COMM_QTY];

/*Funcion encargada de recibir el comando,
 * devuelve el codigo del comando 0 -1 en caso de error
 */
static int getCommand();

/*Inicializa myCommands*/
static void populateCommands();

/*Comandos de DAMOCLES OS*/
static void help();
static void screensaver();
static void shutdown();
static void test();
static void clear();
static void testCommands(void);
static void testText(void);
static void catComm(void);
static void treeComm(void);
static void lsComm(void);
static void cdComm(void);
static void rmComm(void);
static void openComm(void);
static void mkdirComm(void);
static void writeComm(void);


static void screenSaverHelp(void);
static void testHelp(void);
static void helpHelp(void);
static void shutDownHelp(void);
static void clearHelp(void);

void shell(void)
{

	kprintf("\rDAMOCLES v2.0\n\n");
	populateCommands(myCommands);

	while(1)
		executeCommand(getCommand(myCommands));


}

static void populateCommands(Command * c)
{
	int i,j;

	/*Arreglo con los codigos en sus respectivos lugares, esto por conveniencia para poder
	 * iterar despues al cargar myCommands*/
	int commandCodes[COMM_QTY]={COMMAND_SCREENSAVER,COMMAND_TEST, COMMAND_HELP,COMMAND_SHUTDOWN,
			COMMAND_CLEAR,COMMAND_FORTUNE, COMMAND_DEBUG,
			COMMAND_CAT, COMMAND_LS, COMMAND_CD,
			COMMAND_RM,COMMAND_TREE, COMMAND_OPEN, COMMAND_MKDIR, COMMAND_WRITE};

	/*Arreglo con los nombres de los comandos en sus respectivos lugares*/
	char* commands[COMM_QTY];
	commands[COMMAND_SCREENSAVER]="screensaver";
	commands[COMMAND_TEST]="test";
	commands[COMMAND_HELP]="help";
	commands[COMMAND_SHUTDOWN]="shutdown";
	commands[COMMAND_CLEAR] ="clear";
	commands[COMMAND_FORTUNE]= "fortune";
	commands[COMMAND_DEBUG] = "debug";
	commands[COMMAND_CAT] ="cat";
	commands[COMMAND_CD] = "cd";
	commands[COMMAND_LS] = "ls";
	commands[COMMAND_RM] = "rm";
	commands[COMMAND_TREE] = "tree";
	commands[COMMAND_OPEN] = "open";
	commands[COMMAND_MKDIR] = "mkdir";
	commands[COMMAND_WRITE] = "write";



	/*Los punteros de funcion a los comandos y un arreglo que los contenga*/
	commandFnct helpex = help;
	commandFnct screensaverex = screensaver;
	commandFnct testex = test;
	commandFnct shutdownex = shutdown;
	commandFnct clearex = clear;
	commandFnct fortunex = fortune;
	commandFnct debugx = debug;
	commandFnct catx = catComm;
	commandFnct cdx = cdComm;
	commandFnct lsx = lsComm;
	commandFnct rmx = rmComm;
	commandFnct treex = treeComm;
	commandFnct openx = openComm;
	commandFnct mkdirx = mkdirComm;
	commandFnct writex = writeComm;


	/*Arreglo con los punteros a funcion en sus respectivos lugares*/
	commandFnct command_execs[COMM_QTY];
	command_execs[COMMAND_SCREENSAVER]= screensaverex;
	command_execs[COMMAND_TEST]= testex;
	command_execs[COMMAND_HELP]= helpex;
	command_execs[COMMAND_SHUTDOWN]=shutdownex;
	command_execs[COMMAND_CLEAR]=clearex;
	command_execs[COMMAND_FORTUNE] = fortunex;
	command_execs[COMMAND_DEBUG] = debugx;
	command_execs[COMMAND_CAT] = catx;
	command_execs[COMMAND_LS] = lsx;
	command_execs[COMMAND_CD] = cdx;
	command_execs[COMMAND_RM] = rmx;
	command_execs[COMMAND_TREE] = treex;
	command_execs[COMMAND_OPEN] = openx;
	command_execs[COMMAND_MKDIR] = mkdirx;
	command_execs[COMMAND_WRITE] = writex;

	/*Arreglos con los flags de cada comando*/
	char * screenSaverFlags[SSAVERFLAGS]={"s","p","l"};
	char * testFlags[TESTFLAGS]={"e","t"};
	/*En el caso de help respetamos el orden de los comandos*/
	char * helpFlags[HELPFLAGS]={"screensaver","test","help","shutdown","clear","fortune","getPage","retPage"};
	char * shutdownFlags[SDOWNFLAGS]={""};
	char * clearFlags[CLEARFLAGS]={""};
	char * fortuneFlags[FORTUNEFLAGS]={""};
	char * debugflags[DEBUGFLAGS] = {""};
	char * catflags[CAT_FLAGS]={""};
	char * lsflags[LS_FLAGS]={""};
	char * rmflags[RM_FLAGS]={"r"};
	char * treeflags[TREE_FLAGS]={""};
	char * cdflags[CD_FLAGS]={""};
	char * openflags[OPEN_FLAGS]={""};
	char * mkdirflags[MKDIR_FLAGS]={""};
	char * writeflags[WRITE_FLAGS]={"f","s","b","e"};

	/*Arreglo que contiene la cantidad de flags de cada comando
	 * en sus respectivas posiciones
	 */
	int flagsqtys[COMM_QTY];
	flagsqtys[COMMAND_HELP]= HELPFLAGS;
	flagsqtys[COMMAND_SHUTDOWN]= SDOWNFLAGS;
	flagsqtys[COMMAND_TEST]= TESTFLAGS;
	flagsqtys[COMMAND_SCREENSAVER]= SSAVERFLAGS;
	flagsqtys[COMMAND_CLEAR]=CLEARFLAGS;
	flagsqtys[COMMAND_FORTUNE]=FORTUNEFLAGS;
	flagsqtys[COMMAND_DEBUG] = DEBUGFLAGS;
	flagsqtys[COMMAND_CAT] = CAT_FLAGS;
	flagsqtys[COMMAND_LS] = LS_FLAGS;
	flagsqtys[COMMAND_CD] = CD_FLAGS;
	flagsqtys[COMMAND_RM] = RM_FLAGS;
	flagsqtys[COMMAND_TREE] = TREE_FLAGS;
	flagsqtys[COMMAND_OPEN] = OPEN_FLAGS;
	flagsqtys[COMMAND_MKDIR] = MKDIR_FLAGS;
	flagsqtys[COMMAND_WRITE] = WRITE_FLAGS;

	/*Este arreglo de arreglos va a tener todos los flags de cada comando
	 * en sus respectivos lugares.
	 */
	char ** flags[COMM_QTY];

	flags[COMMAND_HELP] = helpFlags;
	flags[COMMAND_SCREENSAVER]=screenSaverFlags;
	flags[COMMAND_TEST]= testFlags;
	flags[COMMAND_SHUTDOWN] = shutdownFlags;
	flags[COMMAND_CLEAR]=clearFlags;
	flags[COMMAND_FORTUNE]=fortuneFlags;
	flags[COMMAND_DEBUG]= debugflags;
	flags[COMMAND_CAT] = catflags;
	flags[COMMAND_CD] = cdflags;
	flags[COMMAND_RM] = rmflags;
	flags[COMMAND_LS] = lsflags;
	flags[COMMAND_TREE] = treeflags;
	flags[COMMAND_OPEN] = openflags;
	flags[COMMAND_MKDIR] = mkdirflags;
	flags[COMMAND_WRITE] = writeflags;

	for ( i = 0; i < COMM_QTY; i++)
	{
		/*Escribo el codigo del comando*/
		myCommands[commandCodes[i]].code = commandCodes[i];

		/*Como los codigos representan su offset, usamos i directamente*/

		/*Copiamos el nombre*/
		memcpy(myCommands[i].command,commands[i],strlen(commands[i]));

		/*Guardamos la cantidad de flags que tiene el comando*/
		myCommands[i].qtyflags = flagsqtys[i];

		/*Guardamos su puntero a funcion*/
		myCommands[i].exec = command_execs[i];

		/*Guardamos todos sus flags*/
		for ( j= 0; j< flagsqtys[i]; j++)
		{
			memcpy(myCommands[i].flags[j],flags[i][j],
					strlen(flags[i][j]));
		}
	}

}

static int
getCommand()
{

	char letra;
	int i;

	for ( i= 0; i < SHELL_BUFFER_LENGTH; i++)
	{
		shellBuffer[i]=0x00;
	}
	index = 0;

	do{

		kprintf("%s",PROMPT);
		kprintf("%s$ ",shellGetCWD());

		do{

			letra = getchar();

			if( isArrow(letra))
			{
				fetchCommand(letra);

			}
			else if ( letra == '\b')
			{

				if ( index > 0)
				{	backSpace();
				index--;
				}
			}
			else if( letra == '\t')
			{
				//autoFill();
			}
			else
			{
				shellBuffer[index++] = letra;
				kprintf("%c",letra);
			}

		}while (letra != '\n' && index < MAX_COMM_LENGTH);

		/*Esta condicion se cumple si solo presiono enter
		 * asi no muestro error y muestro el prompt otra vez
		 */
		if( index <= 1 )
			index = 0;
		if(index >= (MAX_COMM_LENGTH - 1))
			kprintf("\n");
	}while( index == 0);


	/*Guarda el comando en el historial*/
	commitCommand();

	/*Recupero la primera palabra del shell y la trato como si fuera un comando*/
	getCommandFromPrompt(shellBuffer);

	/*Recupero los flags, estos son de la forma
	 * -flag
	 * Si no hay flags, recupero los parametros
	 */


	if(getFlagsFromPrompt(shellBuffer) == 0)
		getParamsFromPrompt(shellBuffer,strlen(command));


	for ( i = 0; i < COMM_QTY; i++ )
	{

		if ( !strcmp(command,myCommands[i].command))
			return myCommands[i].code;
	}

	return -1;

}

/*Saco la primera palabra completa del shell
 * la coloco en el arreglo static command */

static void
getCommandFromPrompt(char * buffer )
{
	int i;
	int comm_i;

	char letra = buffer[0];

	for ( i = 0 ; i < MAX_COMM_LENGTH; i++  )
		command[i]= 0x00;
	i= 0;
	comm_i = 0;
	while(letra == 0x20)
		letra=buffer[++i];

	while(letra != 0x20 && letra != 0x00 )
	{
		command[comm_i++]=buffer[i++];
		letra = buffer[i];

	}

	return;

}

static int
getFlagsFromPrompt(char * buffer)
{
	int i;

	for ( i = 0 ; i < MAX_FLAG_LENGTH; i++  )
		flags[i]= 0x00;

	int cadenaValidaDeFlags = 0;

	/*empiezo a leer a partir del comando*/
	int index = strlen(command);

	int flagindex = 0;
	while ( shellBuffer[index] != 0x00 && shellBuffer[index] != '\n'
			&& index < SHELL_BUFFER_LENGTH )
	{
		if( shellBuffer[index] == 0x20 && cadenaValidaDeFlags )
		{	cadenaValidaDeFlags = 0;
		getParamsFromPrompt(shellBuffer, index);
		return flagindex;
		}

		if (cadenaValidaDeFlags)
			flags[flagindex++]=shellBuffer[index];


		if ( shellBuffer[index ] == '-' && !cadenaValidaDeFlags )
			cadenaValidaDeFlags = 1;

		else if ( shellBuffer[index ] == '-' && cadenaValidaDeFlags )
		{
			cadenaValidaDeFlags = 0;
			return flagindex;
		}

		index++;
	}
	return flagindex;

}

static void
getParamsFromPrompt( char * buffer, int index )
{
	int i;
	int myindex= 0;
	int validParam = 0;
	char previous=0x00;

	for ( i = 0 ; i < MAX_PARAMS_LENGTH; i++  )
		params[i]= 0x00;

	while ( index < SHELL_BUFFER_LENGTH)
	{
		if ( buffer[index]=='\n' || buffer[index] == 0x00 )
			return;

		if( buffer[index] == 0x20 && previous != 0x20 )
			previous = 0x20;

		if(buffer[index] == 0x20 && validParam)
		{
			validParam = 0;
			previous = 0x20;
			params[myindex++]=' ';
		}

		if ( buffer[index] != 0x20 && previous == 0x20 )
		{
			validParam = 1;
			params[myindex++] = buffer[index];

		}

		index++;
	}
}

static void
executeCommand(int commandCode)
{
	if ( commandCode < 0 || commandCode >= COMM_QTY )
	{
		kprintf("shell: %s: command not found\n",command);
		return;
	}
	if (myCommands[commandCode].exec != NULL)
		myCommands[commandCode].exec();
	return;
}

static void
help()
{
	int i;



	if ( strlen(flags) == 0 )
	{
		kprintf("               ,\n");
		kprintf("             (@|\n");
		kprintf(",,           ,)|_____________________________________\n");
		kprintf("//\\\\8@8@8@8@8@8 / _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ \\\n");
		kprintf("\\\\//8@8@8@8@8@8 \\_____________________________________/\n");
		kprintf("``           `)|\n");
		kprintf("             (@|   DAMOCLES OS  \n");
		kprintf("               '\n");

		kprintf("Commands:\n");
		kprintf("\nType help -command\n");
		for ( i = 0; i< COMM_QTY; i++)
		{
			kprintf("    %s\n",myCommands[i].command);

		}


		return;
	}

	for ( i = 0; i  < myCommands[COMMAND_HELP].qtyflags; i++)
	{

		if ( !strcmp(flags,myCommands[COMMAND_HELP].flags[i]))
		{
			switch(i)
			{
			case COMMAND_SCREENSAVER : screenSaverHelp();
			break;
			case COMMAND_TEST : testHelp();
			break;
			case COMMAND_HELP : helpHelp();
			break;
			case COMMAND_SHUTDOWN : shutDownHelp();
			break;
			case COMMAND_CLEAR : clearHelp();
			break;
			case COMMAND_FORTUNE : fortuneHelp();
			break;
			default: ;

			}
			return;
		}
	}
	kprintf("Invalid Option\n");
	return;

}

static void
screenSaverHelp(void)
{
	kprintf("\nSCREENSAVER\n");
	/*kprintf("    Permite interactuar con el salvapantallas de DAMOCLES\n");
	kprintf("\nAdmite tres opciones:\n\n -s Segundos \n\n -p\n\n -l String\n");
	kprintf("\nSeteo del Screensaver: -s Segundos: Setea la cantidad de segundos que esperara");
	kprintf("\n                                    el Screen Saver antes de dispararse.");
	kprintf("\n\nPreview: -p : Muestra un preview del Screen Saver.\n");
	kprintf("\n\nLayout: -l String: Setea la cadena, (layout) que se mostrara en el Screen Saver\n");*/
	kprintf("Utility Removed\n");


}

static void
testHelp(void)
{
	kprintf("\nTEST\n");
	kprintf("Displays test screens for mouse copy.\n");
	kprintf("To copy executable commands: -e\n");
	kprintf("To copy text: -t\n\n");

}

static void
helpHelp(void)
{
	kprintf("\nIm already helping you!\n");
}

static void
shutDownHelp(void)
{
	kprintf("\nOFF button to the right...\n");
}


static void
clearHelp(void)
{
	kprintf("\ntype clear\n");
}



static void
screensaver()
{
	int i;

	for ( i = 0; i  < myCommands[COMMAND_SCREENSAVER].qtyflags; i++)
	{

		if ( !strcmp(flags,myCommands[COMMAND_SCREENSAVER].flags[i]))
		{
			switch(i)
			{
			case 0:{

				int secs = atoi(params);
				if ( secs <= 0 )
				{
					kprintf("Invalid seconds\n\n");
					return;
				}
				setScreensaver(secs);
				kprintf("Screensaver will activate after %d seconds.\n",secs);
			}
			break;
			case 1:{

				previewSaver();
			}
			break;

			case 2:{

				if ( strlen(params) == 0 )
				{
					kprintf("The layout string is invalid!");
					return;
				}

				/*Antepone un espacio para que no se vea junto,
				 * lo pone al principio de la cadena que envia
				 * a la funcion setLayout
				 */
				char layout[MAX_PARAMS_LENGTH];
				for ( i = 0; i< strlen(params); i++)
					layout[i+1]=params[i];
				layout[i+1]=0x00;
				layout[0]=0x20;

				for (i = 0 ; i  < strlen( layout ) + 1 ; i++ )
					params[i] = layout[i];
				saverLayout(params);

				kprintf("The new layout is %s!\n",params);}
			break;


			default: ;

			}
			return;
		}

	}
	kprintf("Screen Saver: Invalid Options!\n");
}

static void
test()
{
	int i;

	for ( i = 0; i  < myCommands[COMMAND_TEST].qtyflags; i++)
	{

		if ( !strcmp(flags,myCommands[COMMAND_TEST].flags[i]))
		{
			switch(i)
			{
			case 0: testCommands();
			break;
			case 1: testText();
			break;
			default: ;

			}
			return;
		}

	}
	kprintf("Test: Invalid Options!\n");
}

static void
testCommands(void)
{
	kprintf("\nCopy with left mouse click and paste with right mouse click\n");
	kprintf("clear\n");
	kprintf("test -t\n");
	kprintf("screensaver -l ARQUITECTURAS\n");
	kprintf("screensaver -s 1\n");
	kprintf("fortune");

}


static void
testText(void)
{
	kprintf("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aenean d");
	kprintf("apibus placerat augue et laoreet. Integer vestibulum augue quis or");
	kprintf("ci feugiat commodo malesuada sem molestie. Nullam eu lobortis quam.");
	kprintf("Donec elementum adipiscing ornare. Nullam sit amet urna a metus rh");
	kprintf("oncus suscipit sed eget lectus. Donec sit amet ante et nibh dapibus");
	kprintf(" consequat. Ut condimentum eleifend pharetra. Nunc sit amet erat tel");
	kprintf("lus, sed posuere sem. Sed scelerisque, sem eu aliquam cursus, mi ");
	kprintf("metus vestibulum sem, a semper tortor lacus eget lacus. Fusce dict");
	kprintf("um lacinia porta. Morbi lobortis purus ac orci dictum a fermentum");
	kprintf(" nibh condimentum. Nunc eget nibh at risus eleifend porttitor nec");
	kprintf(" ut diam. Sed dapibus consectetur nibh, ut volutpat mi dignissim id.\n");
	kprintf("Etiam porta augue a diam commodo vel vehicula sem consequat. Aliqu");
	kprintf("am venenatis facilisis velit eu fermentum. Suspendisse accumsan b");
	kprintf("ibendum purus at posuere. Aenean porttitor sollicitudin tortor et ");
	kprintf("porta. Proin mauris lectus, molestie non mattis id, lobortis ac le");
	kprintf("o. Fusce vel mauris mi, a elementum nulla. Nam varius, ipsum vel f");
	kprintf("aucibus dapibus, purus erat aliquet ante, vel suscipit ante ligula");
	kprintf(" dignissim nisi. Vivamus malesuada turpis id nisi convallis porta.");
	kprintf(" Vestibulum ipsum ante, adipiscing eget consectetur eget, accumsa");
	kprintf("n in risus. Nunc venenatis nibh sit amet risus gravida a faucibus");
	kprintf(" nibh fermentum. Proin nec ultricies sapien. In bibendum euismod n");
	kprintf("isi at vestibulum. Ut feugiat libero in risus rhoncus luctus. Fusc");
	kprintf("e commodo ");
	kprintf("diam a felis condimentum non mollis dui cursus. Donec non elementum");
	kprintf(" justo. Quisque dictum nibh id purus hendrerit in facilisis neque va");
	kprintf("rius. Nullam a felis id nisl pretium vestibulum. Fusce commodo ul");
	kprintf("trices mauris, at ornare eros mattis ac. Maecenas at mattis lacus.\n");
}
static void
shutdown()
{
	kprintf("\nIts off! :(\n");

}



/*
 * Utilizando el cursor, realiza un backspace.
 * Simplemente retrocede una posicion del mismo y coloca un blanco en su
 * lugar. Sin moverse
 */
static void backSpace(){
	char x = getXc()-1;
	char y = getYc();
	setCursor(x, y);
	putToCursor(' ');
	flush(CURSOR);
	setCursor(x, y);
}


static void clear(){

	clearScreen();
	setCursor(0,2);
}

void
updateShellBuffer(int qty )
{
	int i;
	char mybuff[2000];

	read(STDOUT,mybuff,qty);

	for(i = 0; i< qty; i++)
	{

		if ( mybuff[i] == '\n' )
		{
			if ( mybuff[i] == '\n' )
			{
				posOtroComando = i;
				shellBuffer[i] = 0x00;
			}
			else
				shellBuffer[i]=mybuff[i];

		}

		kprintf("%s",shellBuffer);
		index = qty;

		return;
	}
}


static void
fetchCommand(char letra)
{
	int i;

	if( letra == UP )
	{

		for(i=0;i<index;i++)
		{
			backSpace();
		}
		index = 0;
		if ( hist_pos == 0 )
			hist_pos = MAX_COMMANDS_HIST;
		//write(STDIN,history[hist_pos-1],strlen(history[hist_pos-1])); //XXX

		hist_pos--;
	}
	if( letra == DOWN  )
	{

		for(i=0;i<index;i++)
		{
			backSpace();
		}
		index=0;
		if(hist_pos == (MAX_COMMANDS_HIST -1))
			hist_pos = -1;
		//write(STDIN,history[hist_pos+1],strlen(history[hist_pos+1])); //XXX

		hist_pos++;

	}

}

static void
commitCommand( void )
{


	if (index <= 1)
		return;

	if(shellBuffer[index-1]=='\n')
		shellBuffer[index-1]='\0';

	memcpy(history[hist_index],shellBuffer,index );
	hist_index++;


	if(hist_index == MAX_COMMANDS_HIST)
		hist_index = 0;

	hist_pos = hist_index;
	unsaved_comm = 1;

}

static int
isArrow( unsigned char scanCode )
{
	return scanCode == DOWN || scanCode == UP ||
			scanCode == LEFT || scanCode == RIGHT;
}

static void catComm(void)
{

	processApi_t catC = getContext("cat",(process_t)cat, 0);
	contextAddArg(catC,params);
	contextCreate(catC);
	int status, retval;
	waitpid(-1,(exitStatus_t *)&status,&retval,0);
}
static void treeComm(void)
{

	char * cwd = shellGetCWD();
	processApi_t treeC = getContext("tree",(process_t)tree, 0);
	contextAddArg(treeC,cwd);
	contextCreate(treeC);
	int status, retval;
	waitpid(-1,(exitStatus_t *)&status,&retval,0);


}
static void lsComm(void)
{
	char * cwd = shellGetCWD();
	processApi_t lsC = getContext("ls",(process_t)ls, 0);
	contextAddArg(lsC,cwd);
	contextCreate(lsC);
	int status, retval;

	waitpid(-1,(exitStatus_t *)&status,&retval,0);
}


static void cdComm(void)
{
	/*TODO como no me cambia el cwd todavia lo hago a mano
	 * pero esto es lo que se deberia hacer*/
	processApi_t cdC = getContext("cd",(process_t)cd, 0);
	contextAddArg(cdC,params);
	contextCreate(cdC);
	int status, retval;
	waitpid(-1,(exitStatus_t *)&status,&retval,0);

}

static void rmComm(void)
{
	kprintf("RM: flags %s, params %s\n",flags, params);
	processApi_t rmC = getContext("rm",(process_t)rm, 0);
	contextAddArg(rmC,flags);
	contextAddArg(rmC,params);
	contextCreate(rmC);
	int status, retval;
	waitpid(-1,(exitStatus_t *)&status,&retval,0);
}

static void openComm(void)
{
	processApi_t openC = getContext("open",(process_t)open, 0);
	contextAddArg(openC,params);
	contextCreate(openC);
	int status, retval;
	waitpid(-1,(exitStatus_t *)&status,&retval,0);
}

static void mkdirComm(void)
{
	processApi_t mkdirC = getContext("mkdir",(process_t)mkdir, 0);
	contextAddArg(mkdirC,params);
	contextCreate(mkdirC);
	int status, retval;
	waitpid(-1,(exitStatus_t *)&status,&retval,0);
}


static void writeComm(void)
{
	processApi_t writeC = getContext("write",(process_t)writeFile, 0);
	contextAddArg(writeC,flags);
	contextAddArg(writeC,params);
	contextCreate(writeC);
	int status, retval;
	waitpid(-1,(exitStatus_t *)&status,&retval,0);
}










/*TODO este es el wrapper por ahora, CAMBIAR!!!!*/
/*TODO IMPORTANTE Hacer que sea del proceso*/
static char  currentWorkingDir[50] = "/";

 char * shellGetCWD(void)
{
	return currentWorkingDir;
}

void shellSetCWD(char * newcwd)
{

        strcpy(currentWorkingDir,newcwd);
        return;
}

