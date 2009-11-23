/*
 * tree.c
 *
 */

#include "../include/stdio.h"
#include "../include/filesystem.h"
#include "../include/string.h"

static void treeExec(Directory  root, int height );

void
tree(int argc, char ** argv)
{
	Directory root = getDirectoryFromPath(argv[1]);
	treeExec(root, 0);

}

static void
treeExec(Directory  root, int height )
{
	int i,k,j;
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


		elem = getNextItemInDirectory(root,&i,&myType);

		if(!elem)
			continue;

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
				treeExec((Directory)elem,height+1);
			}

		}

	}


}
