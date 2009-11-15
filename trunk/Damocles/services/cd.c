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
    /*Directory newDir = getDirectoryFromPath(argv[1]);*/

	char * cwd = shellGetCWD();
	char temp[LENGTH];
	strcpy(temp, cwd);
	concatenatePath(temp,argv[1]);
	Directory resp = getDirectoryFromPath(temp);

	if(resp != NULL)
	{
		shellSetCWD(temp);
	}
	else
		kprintf("CD: Directory '%s' not found\n",argv[1]);


    /*System Call para cambiar el directorio root de el proceso que me invoco*/
    return;
}
