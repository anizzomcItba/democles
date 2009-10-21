/*
 * mmu.h
 *
 */

#ifndef MMU_H_
#define MMU_H_

#define DIR_ADD   0x30A000
#define PAGE_TABLES 0x30B000
#define USER_MEM  0x801000

#define MEM_PAGE_SIZE 4096

void startPaging(void);

unsigned int getPage(void);

void freePage(unsigned int id);

void enablePage(unsigned int id);

void disablePage(unsigned int id);
#endif /* MMU_H_ */
