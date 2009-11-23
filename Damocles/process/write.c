/*
 * write.c
 */

#include "../include/stdio.h"
#include "../include/filesystem.h"
#include "../include/string.h"
#include "../include/shell.h"

void writeFile(int argc, char ** argv)
{
	/*
	 * El primer argumento son los flags, -f para que el contenido
	 * sea traido de un archivo, -s para un string,
	 * -b para que se ponga desde el principio,
	 * -e para el final.
	 *
	 * Como parametros los archivos fuente y destino.
	 *
	 * Ejemplo
	 *
	 * write -fe src.txt dst.txt
	 *
	 * escribe desde src.txt a dst.txt a partir del final del archivo.
	 */

	/*flags de que flags vinieron*/
	int file=0;
	int string=0;
	int end=0;
	int beginning=0;

	int i, char_flag;

	for(i=0; i < strlen(argv[1]); i++)
	{
		char_flag = argv[1][i];

		switch(char_flag){
		case 'f': file =1 ; string = 0; break;

		case 's': string = 1; file = 0; break;

		case 'e': end = 1; beginning = 0; break;

		case 'b': end = 0; beginning = 1; break;

		default : file = beginning = end = string = 0;
				kprintf("write: Invalid Flag!");
				break;

		}
	}

	/*veo donde esta parada la shell*/
	char * cwd = shellGetCWD();

	/*Aqui dejare la info respecto del archivo fuente*/
	char temp_dir_src[LENGTH];
	char temp_abs_src[LENGTH];
	char temp_name_src[LENGTH];

	/*Aqui dejare la info respecto del archivo destino*/
	char temp_dir_dst[LENGTH];
	char temp_abs_dst[LENGTH];
	char temp_name_dst[LENGTH];

	char temp_argv_src[64];
	char temp_argv_dst[64];

	/*Los nombres vienen separados por un espacio, los obtengo, en caso
	 * que quieran que el src sea lo que escribio en la linea de comandos
	 * entonces estoy guardando eso en temp_argv_src*/
	token(temp_argv_src,argv[2],' ',1);
	token(temp_argv_dst,argv[2]+strlen(temp_argv_src)+1,' ',1);

	/*formateo los dos nombres*/

	kprintf("file %d, string %d, temp_argv_src %s, temp dst %s\n",file,string, temp_argv_src, temp_argv_dst);
	if(file){
		formatPath(cwd,temp_argv_src,temp_abs_src,temp_dir_src,temp_name_src);
	}

	formatPath(cwd,temp_argv_dst,temp_abs_dst,temp_dir_dst,temp_name_dst);

	kprintf("tempo dir dist %s\n", temp_dir_dst);

	Directory dir_src=NULL;
	if(file)
		dir_src = getDirectoryFromPath(temp_dir_src);

	Directory dir_dst = getDirectoryFromPath(temp_dir_dst);

	File file_src;
	File file_dst;


	if(dir_dst == NULL )
	{
		kprintf("Write: Error no se pudo abrir el archivo destino\n");
	}

	if(dir_src != NULL && dir_dst != NULL && file)
	{

		if(file)
			file_src = openFile(dir_src,temp_name_src);

		file_dst = openFile(dir_dst,temp_name_dst);

	}
	else if( dir_dst != NULL && !file )
		file_dst = openFile(dir_dst,temp_name_dst);
	else
		kprintf("Write: Error\n");

	kprintf("asdf");
	int amount;
	char * temp_data;
	if(file)
		temp_data = getDataInFile(file_src,&amount);
	else
	{
		temp_data = temp_argv_src;
		amount = strlen(temp_data);
	}
	placement pl;
	if(end)
		pl = END;
	else
		pl = BEGINNING;

	writeToFile(file_dst,temp_data,amount,pl);

}
