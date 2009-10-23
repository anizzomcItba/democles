/*
 * stack.c para mmu
 *
 */

#include "../include/stack.h"

//Como necesito 20 bits, uso 3 chars.


typedef struct{
	unsigned char first;
	unsigned char second;
	unsigned char third;
}stack_elem;

// 2^20 posibles ids es igual  a  1048576
// Dedico 8MB a partir de los primeros 4MB para Stacks.



static stack_elem stack[STACK_SIZE];
static int top = STACK_SIZE;

void push(unsigned int id)
{

	unsigned int first;
	unsigned int second;
	unsigned int third;

	if(isFull())
		return;

	first = id >> 24;

	second = id;
	second &= 0x00FF0000;
	second = second >> 16;

	third = id;
	third &= 0x0000FC00;
	third = third >> 8;

	stack_elem elem = {(unsigned char)first, (unsigned char)second,(unsigned char)third};

	stack[--top]= elem;
	return;

}

unsigned int pop(void)
{

	if(isEmpty())
		return 0;
	stack_elem elem = stack[top++];
	unsigned int resp = 0;
	resp += (unsigned int)elem.first;
	resp <<= 8;
	resp += (unsigned int)elem.second;
	resp <<= 8;
	resp += (unsigned int)elem.third;
	resp <<= 8;
	return resp;

}

int isEmpty(void){
	if (STACK_SIZE == top)
		return 1;
	else
		return 0;
}

int isFull(void){

	if(top == 0)
		return 1;
	else
		return 0;

}



