/*
 * stack.h
 *
 */

#ifndef STACK_H_
#define STACK_H_

#define STACK_SIZE 5000

void push(unsigned int id);

unsigned int pop(void);

int isEmpty(void);

int isFull(void);
#endif /* STACK_H_ */
