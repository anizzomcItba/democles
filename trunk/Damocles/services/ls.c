/*
 * ls.c
 *
 */

#include "../include/stdio.h"
#include "../include/filesystem.h"
#include "../include/string.h"

/*TODO ESTAS FUNCIONES DE FILESYSTEM DEBERIAN SER SYSCALLS */

void
ls(int argc, char ** argv)
{

	Directory dir = getDirectoryFromPath(argv[1]);
	int numberOfFiles = getNumberOfEntriesInDir(dir);
	int i;

	entryType type;
	void * elem;

	int index = 0;
	for(i=0;i < numberOfFiles; i++)
	{
		elem = getNextItemInDirectory(dir,&index, &type);
		if(type == DIR_TYPE )
		{
			kprintf(" %s\n",getDirectoryName((Directory)elem));

		}
		else
		{
			kprintf(" %s\n",getFileName((File)elem));

		}

	}

}
