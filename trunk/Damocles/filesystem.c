/*
 * filesystem.c
 */


#include "include/mmu.h"
#include "include/filesystem.h"
#include "include/string.h"

struct element{
	entryType type;
	char name[LENGTH];
	void * page;
};

typedef struct element elementEntry;

typedef struct{
	int items;
	int usedOffset;
}dirHeader;

typedef struct{
	void * nextBlock;
	int usedOffset;
}fileHeader;

static elementEntry root;
static int started = 0;



/*FUNCIONES DEL FS*/

static elementEntry * insertEntry(unsigned int * dir, char * name, unsigned int * address, entryType type );
static void makeDirHeader(unsigned int * dir, unsigned int * parent );
static elementEntry * findEntry(unsigned int * dir, char * string);
static elementEntry * getNextEntry(void * dir, int * index);
static char * getNameFromEntry(elementEntry * entry);
static unsigned int * getAddressFromEntry(elementEntry * entry);
static void makeFileHeader(unsigned int * file);
static int isEntryADirectory(void * entry);
static char * appendDataInFile(File,  int * amount);
static char * deleteDataInFile(File);


Directory
startFileSystem()
{

	if(!started )
	{
		root.page = (void *)getPage();
		strcpy(root.name,"/");
		root.type = DIR_TYPE;

		makeDirHeader(root.page,root.page);
		started = 1;
		return (Directory)&root;

	}
	else
		return (Directory)&root;

}




static elementEntry *
insertEntry(unsigned int * dir, char * name, unsigned int * address, entryType type )
{

	dirHeader header = *(dirHeader * )dir;

	elementEntry * elem = (elementEntry *)((char *)dir + header.usedOffset);

	strcpy(elem->name, name);
	elem->page = address;
	elem->type = type;

	header.items++;
	header.usedOffset+=sizeof(*elem);


	memcpy(dir,&header,sizeof(dirHeader));


	return elem;

}




static void
makeDirHeader(unsigned int * dir, unsigned int * parent )
{
	dirHeader newHeader;

	newHeader.items = 0;
	newHeader.usedOffset = sizeof(dirHeader);

	memcpy(dir,&newHeader,sizeof(dirHeader));


	insertEntry(dir,".",dir,DIR_TYPE);
	insertEntry(dir,"..",parent,DIR_TYPE);

	newHeader = *(dirHeader *)dir;



	return;
}



Directory
makeDir(Directory parentDir, char * name)
{
	unsigned int * newEntryAddress = (unsigned int *)getPage();
	unsigned int * dir = parentDir->page;
	elementEntry *  newDir = insertEntry(dir,name,newEntryAddress,DIR_TYPE);
	makeDirHeader(newEntryAddress,dir);
	return (Directory)newDir;
}



static elementEntry *
findEntry(unsigned int * dir, char * string)
{
	dirHeader header;
	header = *(dirHeader * )dir;

	elementEntry * elem = (elementEntry *)((char *)dir + sizeof(dirHeader));
	int i = header.items;

	while(i > 0)
	{
		if(!strcmp(string,elem->name))
			return elem;

		elem++;
		i--;
	}
	return 0x00;


}



/*Devuelvo el elemento siguiente a partir de lo que indique elem*/
static elementEntry *
getNextEntry(void * dir, int * index)
{

	dirHeader header;
	header = *(dirHeader * )dir;

	if(*index < 0 || *index > header.items )
		return 0x00;
	else
	{

		elementEntry * elemEntry = (elementEntry *)((char *)dir + sizeof(dirHeader));

		elemEntry += *index;

		(*index)++;
		return elemEntry;

	}
}




static char *
getNameFromEntry(elementEntry * entry)
{
	if(entry == 0x00 )
		return 0x00;
	return entry->name;
}


char * getDirectoryName(Directory dir)
{
	return getNameFromEntry((elementEntry *)dir);
}

char * getFileName(File file)
{
	return getNameFromEntry((elementEntry *)file);
}



static unsigned int *
getAddressFromEntry(elementEntry * entry)
{
	if(entry == 0x00 )
		return 0x00;
	return entry->page;
}



/*Como al pedir el siguiente puedo recibir Archivo o Directorio este
 * devuelve un void* , int * index es un param out para saber donde esta
 * el en el dir, no alterarlo...
 */



void *
getNextItemInDirectory(Directory  Dir, int * index, entryType * type)
{
	void * ans = (void *)getNextEntry((void *)
			getAddressFromEntry((elementEntry *)Dir),index);
	if(isEntryADirectory(ans))
		*type = DIR_TYPE;
	else
		*type = FILE_TYPE;

	return ans;
}






static int
isEntryADirectory(void * entry)
{
	if(entry == 0x00 )
		return 0x00;
	elementEntry * myentry = (elementEntry *)entry;
	return ( myentry->type == DIR_TYPE );
}




int
getNumberOfEntriesInDir(Directory dir)
{
	dirHeader header;
	header = *(dirHeader * )getAddressFromEntry((elementEntry *)dir);

	return header.items;
}



static void
makeFileHeader(unsigned int * file)
{

	fileHeader header = *(fileHeader * )file;

	header.nextBlock = 0x00;
	header.usedOffset = sizeof(header);

	memcpy(file,&header,sizeof(fileHeader));
	return;
}


int
writeToFile(File file, void * src, int length, placement where )
{
	int dataPresent, dataWritten;
	void * dst;
	dataWritten = length;

	fileHeader * header = (fileHeader *)file->page;

	if(where == BEGINNING)
		dst= deleteDataInFile(file);
	else
		dst = appendDataInFile(file, &dataPresent);

	while(length > 0)
	{
		/*Caso 1, lo que tengo que escribir es mas corto que lo que
		 * me queda de espacio en la pagina
		 */
		if(length < (4096 - dataWritten ) )
		{
			 memcpy(dst,src,length);
			 length -= length;
		}

		else
		{
			/*TODO implementar varias paginas*/
		}

	}


	header->usedOffset += dataWritten;

	return 0;
}

Directory
getDirectoryFromPath(char * path)
{
	return (Directory)&root;
}

File
openFile(Directory directory, char * name)
{
	/*Si ya existe lo busco y lo devuelvo*/

	elementEntry * elem = findEntry(
			getAddressFromEntry((elementEntry *)directory),name);
	if(elem != 0x00 )
		return elem;

	unsigned int * newPage = (unsigned int *)getPage();

	elementEntry * newFile = insertEntry(
			getAddressFromEntry((elementEntry *)directory),
			name,newPage,FILE_TYPE);

	makeFileHeader(getAddressFromEntry(newFile));

	return (File)newFile;

}




char *
getDataInFile(File file, int * amount)
{
	fileHeader header = *(fileHeader *)file->page;
	*amount = header.usedOffset - sizeof(header);

	char * resp = (char *)file->page;
	resp += sizeof(header);
	return resp;
}



static char *
deleteDataInFile(File file)
{
	fileHeader header = *(fileHeader *)file->page;
	header.usedOffset = sizeof(header);

	char * resp = (char *)file->page;
	resp += sizeof(header);
	return resp;

}

static char *
appendDataInFile(File file, int * amount)
{
	fileHeader header = *(fileHeader *)file->page;

	char * resp = (char *)file->page;
	resp += header.usedOffset;
	*amount = header.usedOffset;
	return resp;
}




void
populateFileSystem(Directory root)
{
	Directory  bin, home, music, docs, pictures;
	File londonbeat,  beatit, damoclesimg, smiley;

	bin = makeDir(root,"bin");
	makeDir(root,"boot");
	makeDir(root,"dev");
	makeDir(root,"etc");
	home = makeDir(root,"home");
	makeDir(root,"media");
	makeDir(root,"root");
	makeDir(root,"var");

	File shell = openFile(bin,"shell.bin");
	File apache = openFile(bin,"apache.bin");

	writeToFile(shell,"1010101",8,BEGINNING);
	char * apstr = "1111000101010101010101";
	writeToFile(apache,apstr,strlen(apstr),BEGINNING);
	music = makeDir(home,"Music");
	docs = makeDir(home,"Docs");
	pictures = makeDir(home,"Pictures");

	londonbeat = openFile(music,"Ive Been Thinking About you.mp3");
	beatit = openFile(music,"Beat It.mp3");
	damoclesimg = openFile(docs,"Damocles.img");
	smiley = openFile(pictures,"Smiley.jpg");

	char * str = "Esta es la cancion de London Beat, que se llama I've been thinking about you!";
	writeToFile(londonbeat,str,strlen(str), BEGINNING);
	int amount;
	char * src = getDataInFile(londonbeat,&amount);
	writeToFile(beatit,src,amount,BEGINNING);
	char * str2 = "Esta es la cancion de Michael Jackson";
	writeToFile(beatit,str2,strlen(str2),END);

}


