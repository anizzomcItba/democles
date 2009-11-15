/*
 * filesystem.c
 */


#include "include/mmu.h"
#include "include/filesystem.h"
#include "include/string.h"
#include "include/stdio.h"

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
static void getNextElementFromPath(char * nextDir, char * path);
static void reducePath(char * path);
static elementEntry * getElementFromPath(char * path);

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

void getDirectoryPath(Directory dir, char * dst)
{
	char * root = "/";

	Directory actual = (Directory)findEntry(dir->page,".");
	Directory parent = (Directory)findEntry(dir->page,"..");

	if(actual->page != parent->page)
	{
		getDirectoryPath(parent,dst);
	}
	if(strcmp(dir->name,".."))
		stradd(dst,dir->name);
	if(dst[strlen(dst)]!= '/')
		stradd(dst,"/");

	return;
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

static elementEntry *
getElementFromPath(char * path)
{
	/*Siempre empiezo en root*/
	elementEntry * resp = &root;
	elementEntry * tempdir;
	int len = strlen(path);
	char  subdir[40] = {0};


	/*Si es dir absoluta empiezo a partir del '/' */
	if(path[0] == '/')
	{
		path = path +1;
		len -= 1;
	}

	/*Elimino el trailing '/' si lo tiene */
	if(path[strlen(path)]== '/')
	{
		path[strlen(path)]='\0';

	}

	while(len > 0){
		getNextElementFromPath(subdir, path);

		tempdir = findEntry(resp->page,subdir);


		if(tempdir != NULL )
			resp = tempdir;
		else
			return NULL;
		len -= strlen(subdir)+1;
		path += strlen(subdir)+1;
	}

	return resp;
}


Directory
getDirectoryFromPath(char * path)
{
	elementEntry * elem = getElementFromPath(path);
	if(elem->type == DIR_TYPE)
		return (Directory)elem;
	else
		return NULL;
}

File
getFileFromPath(char * path)
{
	elementEntry * elem = getElementFromPath(path);
		if(elem->type == FILE_TYPE)
			return (File)elem;
		else
			return NULL;
}

static void
getNextElementFromPath(char * nextDir, char * path )
{

	token(nextDir,path,'/');


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
	Directory  bin, home, music, docs, pictures, salsa, tecno, tango, cumbia, pop;
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

	salsa= makeDir(music,"salsa");
	tecno = makeDir(music,"tecno");
	tango = makeDir(music,"tango");
	cumbia = makeDir(music,"cumbia");
	pop = makeDir(music,"pop");


	londonbeat = openFile(pop,"tkngabtu.txt");
	beatit = openFile(pop,"beatit.txt");




	damoclesimg = openFile(docs,"Damocles.img");
	smiley = openFile(pictures,"Smiley.jpg");

	char * str = "Esta es la cancion de London Beat, que se llama I've been thinking about you!";
	writeToFile(londonbeat,str,strlen(str), BEGINNING);
	int amount;

	char * str2 = "Esta es la cancion de Michael Jackson, Beat It, de su segundo album como solista Thriller.";
	writeToFile(beatit,str2,strlen(str2),END);

}

void DirDebug(Directory dir)
{
	kprintf("El nombre es %s, la pag es %x\n",dir->name,dir->page);
	int i = getNumberOfEntriesInDir(dir);
	void * elem;
	int j=0;
	entryType type;
	while(i>0)
	{
		elem = getNextItemInDirectory(dir,&j,&type);
		if(type == DIR_TYPE)
		{
			Directory newdir = (Directory)elem;
			kprintf("Dir: nombre %s, pagina %x\n",
					getDirectoryName(elem),newdir->page);
		}

		i--;
	}

}

void
concatenatePath(char * absolutepath, char * name)
{

	if(absolutepath[strlen(absolutepath)-1] != '/')
		stradd(absolutepath,"/");

	if(name[0]== '/')
		strcpy(absolutepath,name);
	else
		stradd(absolutepath,name );

	reducePath(absolutepath);
}

static void reducePath(char * path)
{
	int i=strlen(path)-1;

	if(path[i] == '/')
	{
		path[i] = '\0';
		i--;
	}

	char temp[64];
	int j=0;
	int separators = 0;
	while(i>=0)
	{


		if(path[i] == '.' &&( path[i+1]=='/' || path[i+1]=='\0')&& path[i-1]== '/')
			separators += 1;


		if(path[i-1]=='.' && path[i]=='.'&&( path[i+1]=='/' || path[i+1] == '\0') && path[i-2]=='/')
			separators += 2;

		if(path[i] == '/' && separators)
			separators--;
		if(separators==0)
		{
			if(path[i] != '/')
				temp[j++]=path[i];
			else if(path[i] == '/' && temp[j-1] != '/' )
				temp[j++]=path[i];

		}
		i--;

	}
	int len = strlen(temp) - 1;

	for(i=0; i < len/2 +1;i++)
	{
		j= temp[i];
		temp[i] = temp[len - i];
		temp [len - i] = j;
	}
	strcpy(path,temp);


}


