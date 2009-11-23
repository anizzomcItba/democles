/*
 * cd.c
 */
#include "../include/stdio.h"
#include "../include/string.h"
#include "../include/cd.h"
#include "../include/filesystem.h"
#include "../include/shell.h"

void cd(int argc, char ** argv)
{


	char * cwd = shellGetCWD();
	char temp_dir[LENGTH];

	formatPath(cwd,argv[1],temp_dir,NULL,NULL);
	Directory resp = getDirectoryFromPath(temp_dir);

	/*System Call para cambiar el directorio root de el proceso que me invoco*/
	if(resp != NULL)
	{
		shellSetCWD(temp_dir);
	}
	else
		kprintf("CD: Directory '%s' not found\n",argv[1]);



    return;
}
