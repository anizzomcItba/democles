/*
 * rm.c
 */

#include "stdio.h"
#include "filesystem.h"
#include "string.h"
#include "shell.h"

void rm(int argc, char ** argv)
{
	/*Si el flag es -r, entonces se borra un dir*/
	char temp_abs[64]={0};
	char temp_dir[64]={0};
	char temp_name[64]={0};

	/*Formateo las entradas, y tendre directorio final, nombre final
	 * y el path absoluto
	 */
	formatPath(shellGetCWD(),argv[2],temp_abs,temp_dir,temp_name);


	Directory dir;
	/*Si quiero borrar un dir*/

	if(argv[1][0] == 'r' )
	{
		/*formatPath reduce el path y saca los . y .., entonces
		 * si temp_name me queda vacio, es porque el usuario
		 * quiso borrar . o ..
		 */
		if(temp_name[0] == '\0')
		{
			kprintf("No such Directory %s\n",argv[2]);
				return;
		}


		dir = getDirectoryFromPath(temp_abs);
		if(dir)
			removeDir(dir);
		else
		{
			kprintf("No such Directory %s\n",temp_name);
			return;
		}

	}
	else
	{
		dir = getDirectoryFromPath(temp_dir);
		if(dir)
		{
			File file = getFileFromPath(temp_abs);
			if(file)
				removeFile(dir,file);
			else
			{
				dir = getDirectoryFromPath(temp_abs);
				if(!dir)
					kprintf("rm: No such File %s\n",temp_name);
				else
					kprintf("rm: %s is a Directory, use flag -r\n",temp_name);
				return;
			}
		}
		else
		{
			kprintf("rm: No such Directory %s\n",temp_dir);
		}
	}

}

