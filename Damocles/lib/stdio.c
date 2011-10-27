/* stdio.c */

#include "sysasm.h"
#include "string.h"
#include "video.h"
#include "syscall.h"

void kputchar(char c){
	write(STDOUT, &c, 1);
}

char kgetchar()
{
	char c;
	read(STDIN,&c,1);
	return c;
}
void kprint(const char *str)
{
	write(STDOUT, (char *)str, strlen(str));
	flush(STDOUT);
}

/*
 * Fuente:
 * http://www.gnu.org/software/grub/manual/multiboot/multiboot.html
 */

void
kprintf (const char *format, ...)
{
  char **arg = (char **) &format;
  int c;
  char buf[20];

  arg++;

  while ((c = *format++) != 0)
    {
      if (c != '%')
        putToCursor(c);
      else
        {
          char *p;

          c = *format++;
          switch (c)
            {
            case 'd':
            case 'u':
            case 'x':
              itoa (buf, c, *((int *) arg++));
              p = buf;
              goto string;
              break;

            case 's':
              p = *arg++;
              if (! p)
                p = "(null)";

            string:
              while (*p)
                putToCursor(*p++);
              break;

            default:
              putToCursor(*((int *) arg++));
              break;
            }
        }
    }
  flush(CURSOR);
}


void
printf (const char *format, ...)
{
  char **arg = (char **) &format;
  int c;
  char buf[20];

  arg++;

  while ((c = *format++) != 0)
    {
      if (c != '%')
    	  kputchar(c);
      else
        {
          char *p;

          c = *format++;
          switch (c)
            {
            case 'd':
            case 'u':
            case 'x':
              itoa (buf, c, *((int *) arg++));
              p = buf;
              goto string;
              break;

            case 's':
              p = *arg++;
              if (! p)
                p = "(null)";

            string:
              while (*p)
            	  kputchar(*p++);
              break;

            default:
            	kputchar(*((int *) arg++));
              break;
            }
        }
    }
  flush(STDOUT);
}


int getchar(){
	char rta = 0;

	read(STDIN, &rta, 1);
	return rta;
}

