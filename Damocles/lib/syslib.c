/* syslib.c */

#include "../include/syslib.h"
#include "../include/sysasm.h"
#include "../drivers/video/crtc6845.h"
#include "../include/string.h"
#include "../include/stdio.h"

int disableInts() {
	int flags = gFlags();
	/* Retorna si el I flag está on o no */
	_cli();
	return (flags & (0x1 << 9));
}

void restoreInts(int iflag) {

	/* Activo el iflag si no estab!a activado */
	if (iflag)
		_sti();
	return;
}

void uprintf(const char* format, ...) {
	char **arg = (char **) &format;
	int c, l_pos = 0, lines = 1, j;
	char buf[20] = { '\0' }, line[81] = { '\0' };

	arg++;

	while ((c = *format++) != 0) {
		if (c != '%') {
			switch (c) {
			case '\n':
				line[l_pos]='\0';
				_vuprint(line, lines);
				l_pos = 0;
				lines++;
				break;
			case '\r':
				l_pos = 0;
				break;
			case '\t':
				for (j = 0; j < TAB_SPACE; j++) {
					line[l_pos++] = ' ';
					if (l_pos == 80) {
						line[l_pos]='\0';
						_vuprint(line, ++lines);
						l_pos = 0;
					}
				}
				break;

			default:
				line[l_pos++] = c;
				break;
			}
		} else {
			char *p;

			c = *format++;
			switch (c) {
			case 'd':
			case 'u':
			case 'x':
				itoa(buf, c, *((int *) arg++));
				p = buf;
				goto string;
				break;

			case 's':
				p = *arg++;
				if (!p)
					p = "(null)";

				string: while (*p) {
					line[l_pos++] = *p++;
					if (l_pos == 80) {
						line[l_pos]='\0';
						_vuprint(line, lines++);
						l_pos = 0;
					}
				}
				break;

			default:
				line[l_pos++] = *((int *) arg++);
				break;
			}
		}
		if (l_pos == 80) {
			line[l_pos]='\0';
			_vuprint(line, lines++);
			l_pos = 0;
		}
	}
	line[l_pos]='\0';
	_vuprint(line, lines++);
	l_pos = 0;
}



