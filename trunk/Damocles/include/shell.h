/*
 * shell.h
 *
 */

#ifndef SHELL_H_
#define SHELL_H_

void shell(void);
void updateShellBuffer(int qty );

/*TODO estas funciones dberian ser system calls*/
char * shellGetCWD(void);
void shellSetCWD(char * newcwd);

#endif /* SHELL_H_ */
