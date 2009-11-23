/*
 * mkdir.c
 *
 */

#include "../include/stdio.h"
#include "../include/filesystem.h"
#include "../include/string.h"
#include "../include/shell.h"

void mkdir(int argc, char ** argv)
{
	/*Si el path es absoluto, entonces no
	 * tomo en cuenta el cwd de la shell y deduzco el
	 * nombre a partir del path absoluto
	 */
	char * cwd = shellGetCWD();
	char temp_dir[LENGTH];
	char temp_abs[LENGTH];
	char temp_name[LENGTH];

	formatPath(cwd,argv[1],temp_abs,temp_dir,temp_name);
	Directory dir = getDirectoryFromPath(temp_dir);
	if(dir != NULL)
		makeDir(dir,temp_name);
	else
		kprintf("Open: Directory %s not found!\n",temp_dir);
}
