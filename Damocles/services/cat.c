/*
 * cat.c
 */

#include "../include/stdio.h"
#include "../include/filesystem.h"


void
cat(File file)
{
	int amount,i;

	char * data = getDataInFile(file,&amount);

	printf("CAT -- %s -- \n",getFileName(file));
	for(i=0;i<amount;i++)
	{
		printf("%c",*data);
		data++;
	}
	printf("\n");
}

