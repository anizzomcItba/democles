/*
 * cat.c
 */

#include "../include/stdio.h"
#include "../include/filesystem.h"
#include "../include/string.h"
#include "../include/shell.h"

void static  catExec(File file);

void cat(int argc, char ** argv)
{

	char * cwd = shellGetCWD();
	char temp[LENGTH];
	strcpy(temp, cwd);
	concatenatePath(temp,argv[1]);
	File file = getFileFromPath(temp);
	if(file!=NULL)
		catExec(file);
	else
		kprintf("CAT: File '%s' not found\n",argv[1]);
}

void static
catExec(File file)
{
	int amount,i;

	char * data = getDataInFile(file,&amount);

	kprintf("CAT -- %s -- \n",getFileName(file));
	for(i=0;i<amount;i++)
	{
		kprintf("%c",*data);
		data++;
	}
	kprintf("\n");
}

