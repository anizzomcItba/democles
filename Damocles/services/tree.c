/*
 * tree.c
 *
 */

#include "../include/stdio.h"
#include "../include/filesystem.h"
#include "../include/string.h"

void
tree(Directory  root, int height )
{
	/*int i,k,j;
	i = 0;

	void * elem;
	entryType myType;
	for(j=0;j<height;j++)
	{
		if(j == height - 1)
			kprintf("|---");
		else
			kprintf("    ");

	}

	kprintf("%s \n",getDirectoryName(root));

	for(k= getNumberOfEntriesInDir(root); k > 0; k-- )
	{
		//elem = getNextEntry((void *)root->page,&i);

		elem = getNextItemInDirectory(root,&i,&myType);

		char * name;
		if(myType == FILE_TYPE )
		{
			File  file = (File )elem;
			name = getFileName(file);

			for(j=0;j<height+1;j++)
						kprintf("    ");
					kprintf("%s\n",name);

		}
		else
			name = getDirectoryName((Directory)elem);

		if(myType == DIR_TYPE )
		{
			if(strcmp(name,".")&&strcmp(name,".."))
			{
				tree((Directory)elem,height+1);
			}

		}

	}*/
	kprintf("tree %x,%d\n",root,height);

}
