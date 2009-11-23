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

	entryType type;
	void * elem;

	int index = 0;
	while(index < numberOfFiles)
	{
		elem = getNextItemInDirectory(dir,&index, &type);
		if(elem != NULL )
		{
			if(type == DIR_TYPE )
			{
				char * name = getDirectoryName((Directory)elem);

				if(strcmp(".",name) && strcmp("..",name))
					kprintf(" %s\n",name);

			}
			else
			{
				kprintf(" %s\n",getFileName((File)elem));

			}

		}

	}

}
