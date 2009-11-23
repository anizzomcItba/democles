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
	int deleted;			//Baja-logica
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
static void concatenatePath(char * absolutepath, char * name);
static void getNameFromAbsPath(char * path, char * dst);

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
	elem->deleted = 0;
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
		if(!strcmp(string,elem->name)&& !elem->deleted)
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
	if(entry->deleted)
		return 0x00;
	return entry->name;
}


char * getDirectoryName(Directory dir)
{
	return getNameFromEntry((elementEntry *)dir);
}

void getDirectoryPath(Directory dir, char * dst)
{


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
 * el Iterador en el dir, no alterarlo...
 */



void *
getNextItemInDirectory(Directory  Dir, int * index, entryType * type)
{
	elementEntry * elem;
	int found = 0;
	void * ans;
	if(*index >= getNumberOfEntriesInDir(Dir))
	{
		*index = getNumberOfEntriesInDir(Dir);
		return NULL;
	}
	do
	{
		ans = (void *)getNextEntry((void *)
					getAddressFromEntry((elementEntry *)Dir),index);
		elem = (elementEntry *)ans;
		if(elem)
		{
			if(!elem->deleted)
			{
				found = 1;

			}
			else
				ans = NULL;

		}
				//kprintf("found %d, index %d, number %d, ans %x\n",
					//	found,*index,getNumberOfEntriesInDir(Dir),ans);
	}while(!found && *index < getNumberOfEntriesInDir(Dir));


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
		return 0;
	elementEntry * myentry = (elementEntry *)entry;
	if(myentry->deleted)
		return 0;
	return ( myentry->type == DIR_TYPE );
}




int
getNumberOfEntriesInDir(Directory dir)
{
	dirHeader header;
	if(dir == NULL)
		return 0;
	if(dir->deleted)
		return 0;
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

	if(file == NULL || src == NULL || length <= 0 )
		return -1;

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

	if(path == NULL )
		return NULL;

	int len = strlen(path);

	if(len <= 0)
		return NULL;

	char  subdir[40] = {0};


	/*Quieren ir a root*/
	if(path[0] == '/' && path[1] == '\0')
		return resp;

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
	if(path == NULL )
		return NULL;
	elementEntry * elem = getElementFromPath(path);
	if(elem->deleted)
		return NULL;
	if(elem->type == DIR_TYPE)
		return (Directory)elem;
	else
		return NULL;
}

File
getFileFromPath(char * path)
{
	if(path == NULL )
		return NULL;
	elementEntry * elem = getElementFromPath(path);
	if(elem->deleted)
		return NULL;
		if(elem->type == FILE_TYPE)
			return (File)elem;
		else
			return NULL;
}

static void
getNextElementFromPath(char * nextDir, char * path )
{

	token(nextDir,path,'/',1);


}

File
openFile(Directory directory, char * name)
{
	/*Si ya existe lo busco y lo devuelvo*/

	elementEntry * elem = findEntry(
			getAddressFromEntry((elementEntry *)directory),name);
	if(elem != 0x00 && !elem->deleted)
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
	if(file == NULL )
		return NULL;
	if(file->deleted)
		return NULL;
	fileHeader header = *(fileHeader *)file->page;
	*amount = header.usedOffset - sizeof(header);

	char * resp = (char *)file->page;
	resp += sizeof(header);
	return resp;
}



static char *
deleteDataInFile(File file)
{
	if(file == NULL )
		return NULL;
	if(file->deleted)
		return NULL;
	fileHeader header = *(fileHeader *)file->page;
	header.usedOffset = sizeof(header);

	char * resp = (char *)file->page;
	resp += sizeof(header);
	return resp;

}

static char *
appendDataInFile(File file, int * amount)
{
	if(file == NULL )
		return NULL;
	if(file->deleted)
		return NULL;
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


	londonbeat = openFile(pop,"thinking.txt");
	beatit = openFile(pop,"beatit.txt");




	damoclesimg = openFile(docs,"Damocles.img");
	smiley = openFile(pictures,"Smiley.jpg");

	char * str = "Esta es la cancion de London Beat, que se llama I've been thinking about you!";
	writeToFile(londonbeat,str,strlen(str), BEGINNING);

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

static void concatenatePath(char * absolutepath, char * name)
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

	/*En caso de ser root lo devuelvo no mas*/
	if(path[0] == '/' && path[1] == '\0')
		return;
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

static void getNameFromAbsPath(char * path, char * dst)
{
	/*Si termina con / lo saco*/
	char temp[64]={0};
	strcpy(temp,path);

	if(temp[strlen(temp)-1]=='/')
		temp[strlen(temp)-1] = '\0';
	token(dst,path,'/',0);
	strremove(path,strlen(path)-strlen(dst),strlen(path));


}


void removeFile(Directory dir,File file)
{
	if(file == NULL || dir == NULL)
		return;
	else
	{
		if(file->type == DIR_TYPE || dir->type == FILE_TYPE )
		{
			return;
		}
		else
		{
			/*TODO SYSCALL DE MMU */
			freePage((unsigned int)file->page);
			file->deleted = 1;
		}
	}
}

void removeDir(Directory dir)
{
	int elems, index;
	elementEntry  * elem;
	if(dir == NULL )
		return;
	if(dir->deleted)
		return;
	dirHeader header = *(dirHeader *)dir->page;
	elems = header.items;
	index = 0;
	entryType type;
	while(index < elems)
	{
		elem = (elementEntry *)getNextItemInDirectory(dir,&index,&type);
		if(elem != NULL )
		{

			/*No quiero borrar a mi padre*/
			if(strcmp("..",getNameFromEntry(elem)) && strcmp(".",getNameFromEntry(elem)))
			{

				freePage((unsigned int)elem->page);
				elem->deleted = 1;
			}

		}
	}
	freePage((unsigned int)dir->page);
	dir->deleted = 1;

}


void formatPath(char * base, char * relative,char * dstAbsPath, char * dstDir, char * dstName)
{
	char temp_base[64]={0};
	char temp_relative[64]={0};
	strcpy(temp_base, base);
	strcpy(temp_relative,relative);


	if(!base)
		return;

	/*Primero concateno los dos, base, con relative*/
	if(relative)
		concatenatePath(temp_base,temp_relative);

	if(dstAbsPath)
		strcpy(dstAbsPath,temp_base);

	if(dstDir != NULL && dstName != NULL )
	{
		strcpy(dstDir,temp_base);
		getNameFromAbsPath(dstDir,dstName);
	}
	return;
}
