/*
 * filesystem.h
 *
 */

#ifndef FILESYSTEM_H_
#define FILESYSTEM_H_

#define LENGTH 64

typedef enum  {FILE_TYPE,DIR_TYPE} entryType;
typedef enum {BEGINNING, END } placement;

typedef struct element * File;
typedef File Directory;



/*Empieza el fileSystem y te devuelve el directorio root*/
Directory  startFileSystem();

/*Crea un directorio con el nombre name en el parentDir y te lo devuelve*/
Directory  makeDir(Directory  parentDir, char * name);


/*Retorna el siguiente item en un directorio apuntado por index.
 * La funcion misma modifica index para saber donde esta.
 * Como el item puede ser archivo o directorio, la funcion te devuelve
 * un puntero a void y en type te deja el tipo resultante para que
 * lo castees a su respectivo tipo.
 */
void * getNextItemInDirectory(Directory   Dir, int * index, entryType * type);

/*La cantidad de entradas en un directorio*/
int getNumberOfEntriesInDir(Directory  dir);

/*Escribe en un archivo, placement dice donde se escribira... al principio lo
 * sobreescribe, al final lo apendea*/
int writeToFile(File  dst, void * src, int length, placement where);

/*Abre un archivo, si no existe crea uno y te lo devuelve*/
File  openFile(Directory  directory, char * name);

/*Para que se vea mas llenito*/
void populateFileSystem(Directory root);

Directory getDirectoryFromPath(char * path);

File getFileFromPath(char * path);

char * getDirectoryName(Directory  dir);

char * getFileName(File  file);

/*Devuelve un puntero a los datos en el archivo, y coloca en amount la cantidad de bytes del archivo*/
char * getDataInFile(File  file, int * amount );

void DirDebug(Directory dir);

void getDirectoryPath(Directory dir, char * dst);

void removeDir(Directory dir);

void removeFile(Directory dir,File file);

/*
 * Recibe un path base y un nombre para unirlos en un solo path absoluto
 * el path absoluto queda en dstAbsPath. En caso de recibir los params
 * dstDir y dstName deja ahi el directorio final y el nombre final del archivo
 */

void formatPath(char * base, char * relative,char * dstAbsPath, char * dstDir, char * dstName);



#endif /* FILESYSTEM_H_ */
