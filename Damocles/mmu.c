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
	//Como mapeo 4MB para Kernel, 8MB para Stack, entonces mapeo
	//12MB que son 3 entradas del directorio

	for ( i = 0; i < 3; i++)
	{

			for(j = 0; j < 1024; j++)
			{
				//Seteo la entrada, lo que sea de los primeros 4 megas no los meto
				//en el stack de paginas disponibles, porque estas son del kernel.

				if(i == 0 )
				{
					pop();
					page_tables[j] = address | 3;
				}
				else
					page_tables[j] = address | 2;

				address += MEM_PAGE_SIZE;

			}
			page_directory[i] =  (dir_entry)page_tables;
			page_tables = (unsigned int*)((int)page_tables + 4096);
			page_directory[i] = page_directory[i] | 3;


	}

	for(i = 3; i < 1024 ; i++)
		page_directory[i] = 0 | 2;


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


	page_entry * page_table;
	page_table = (page_entry *)(page_directory[dir_index] & 0xFFFFF000);

	//kprintf("%x -> %x\n",page_directory[dir_index], page_table[page_table_index]);

	if((page_directory[dir_index] % 2 )  == 0 )
	{

		return 0;

	}


	if((page_table[page_table_index]& 0x0000000F) == 2)
		page_table[page_table_index ] |= 3;
	else
	//	Ya estaba presente!
		return 0;


	//kprintf("%x -> %x\n",page_directory[dir_index], page_table[page_table_index]);


	return resp;

}

void
freePage(unsigned int id)
{
	int dir_index;
	int page_table_index;

	getDirectoryPageTableIndex(id,&dir_index, &page_table_index);

	page_entry * page_table;
	page_table = (page_entry *)(page_directory[dir_index] & 0xFFFFF000);

	//kprintf("%x -> %x\n",page_directory[dir_index], page_table[page_table_index]);


	if((page_table[page_table_index]& 0x0000000F) == 2)
		//kprintf("Me dieron una pagina que no estaba presente, no la puedo liberar!\n");
		return;
	else
		page_table[page_table_index] &= 0xFFFFFFF2;

	//kprintf("%x -> %x\n",page_directory[dir_index], page_table[page_table_index]);

	return push(id);
}

void enablePage(unsigned int id)
{
	int dir_index;
	int page_table_index;
	getDirectoryPageTableIndex(id,&dir_index,&page_table_index);
	page_entry * page_table;
	page_table = (page_entry *)(page_directory[dir_index] & 0xFFFFF000);
	page_table[page_table_index] |= 3;
	return;

}

void disablePage(unsigned int id)
{
	int dir_index;
	int page_table_index;
	getDirectoryPageTableIndex(id,&dir_index,&page_table_index);
	page_entry * page_table;
	page_table = (page_entry *)(page_directory[dir_index] & 0xFFFFF000);
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


