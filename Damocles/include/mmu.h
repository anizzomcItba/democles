/*
 * mmu.h
 *
 */

#ifndef MMU_H_
#define MMU_H_

#define DIR_ADD   0x400000
#define PAGE_TABLES 0x401000
#define USER_MEM  0x801000

void startPaging(void);

unsigned int getPage(void);

void freePage(unsigned int id);

#endif /* MMU_H_ */
