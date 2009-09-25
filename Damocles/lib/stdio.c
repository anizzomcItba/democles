/* stdio.c */

#include "../include/sysasm.h"
#include "../include/string.h"
#include "../include/video.h"

void kputchar(char c){
	_write(STDOUT, &c, 1);
}

char kgetchar()
{
	char c;
	_read(STDIN,&c,1);
	return c;
}
void kprint(const char *str)
{
	_write(STDOUT, (char *)str, strlen(str));
	_flush(STDOUT);
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
  _flush(CURSOR);
}

int getchar(){
	char rta = 0;

	_read(STDIN, &rta, 1);
	return rta;
}

