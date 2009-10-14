/*
 * mmu.c
 *
 */

#include "include/mmu.h"
#include "include/stack.h"
#include "include/sysasm.h"
#include "include/stdio.h"


typedef unsigned int dir_entry;
typedef unsigned int page_entry;


//dir_entry  page_directory[1024];
//page_entry page_tables[1024][1024];

//El directorio empieza despues del kernel
dir_entry * page_directory = (dir_entry *) DIR_ADD;
//La primera tabla de paginas
page_entry * page_tables = (page_entry * ) PAGE_TABLES;


unsigned long address = 0;


static void startAllocator(void);
static void getDirectoryPageTableIndex(unsigned int id, int* dirPtr, int * pagePtr);

void startPaging(void)
{

	int i, j;

	startAllocator();
	for ( i = 0; i < 1024; i++)
	{
		for(j = 0; j < 1024; j++)
		{
			//Seteo la entrada, todo lo que sea de los primeros 8 megas no los habilito
			if(i <= 1)
			{
				page_tables[j] = address | 3;
				pop();
			}
			else
				page_tables[j] = address | 2;

			address += MEM_PAGE_SIZE;

		}
		page_directory[i] =  (dir_entry)page_tables;
		page_tables += MEM_PAGE_SIZE;
		if(i == 0 || i == 1)
			page_directory[i] = page_directory[i] | 3;
		else
			page_directory[i] = page_directory[i] | 2;
	}


	_write_cr3(page_directory); // put that page directory address into CR3
	_write_cr0(_read_cr0() | 0x80000000); // set the paging bit in CR0 to 1


}

static void startAllocator(void)
{
	 int id = MEM_PAGE_SIZE*(STACK_SIZE -1);
	int i;
	for(i= 0;i < STACK_SIZE; i++)
	{
		push(id);
		id -= MEM_PAGE_SIZE;
	}

}

unsigned int
getPage(void)
{
	unsigned int resp;

	int dir_index;
	int page_table_index;

	resp =  pop();

	getDirectoryPageTableIndex(resp,&dir_index,&page_table_index);

	if(page_directory[dir_index] | 2 )
		page_directory[dir_index] |= 3;

	page_entry * page_table;
	page_table = (page_entry *)page_directory[dir_index];

	if((page_table[page_table_index] & 3 ) == 3){
		//kprintf("Esa entrada de la tabla de pagina ya estaba presente!!!!");
	}
	else
		page_table[page_table_index] |= 3;


//	kprintf("dir index = %d, page table = %d\n",dir_index,page_table_index);
//	kprintf("La entrada en el dir %x\n",page_directory[dir_index]);
//	kprintf("La entrada en la tabla de pag %x\n",page_table[page_table_index]);
	return resp;


	/*Todo, setear la pagina en el directorio correspondiendte*/
}

void
freePage(unsigned int id)
{
	int dir_index;
	int page_table_index;

	getDirectoryPageTableIndex(id,&dir_index, &page_table_index);

	page_entry * page_table;
	page_table = (page_entry *)page_directory[dir_index];

	if((page_table[page_table_index] & 3 ) == 2)
		kprintf("Me dieron una pagina que no estaba presente, no la puedo liberar!");
	else
		page_table[page_table_index] &= 0xFFFFFFF2;

	kprintf("dir index = %d, page table = %d\n",dir_index,page_table_index);
	kprintf("La entrada en el dir %x\n",page_directory[dir_index]);
	kprintf("La entrada en la tabla de pag %x\n",page_table[page_table_index]);
	return push(id);
}

void enablePage(unsigned int id)
{
	int dir_index;
	int page_table_index;
	getDirectoryPageTableIndex(id,&dir_index,&page_table_index);
	page_entry * page_table;
	page_table = (page_entry *)page_directory[dir_index];
	page_table[page_table_index] |= 3;
	return;

}

void disablePage(unsigned int id)
{
	int dir_index;
	int page_table_index;
	getDirectoryPageTableIndex(id,&dir_index,&page_table_index);
	page_entry * page_table;
	page_table = (page_entry *)page_directory[dir_index];
	page_table[page_table_index] &= 0xFFFFFFF2;
	return;

}

static void
getDirectoryPageTableIndex(unsigned int id, int* dirPtr, int * pagePtr)
{
	*dirPtr = id >> 22;
	*pagePtr = id & 0x003FF000;
	*pagePtr  >>= 12;
	return ;
}


