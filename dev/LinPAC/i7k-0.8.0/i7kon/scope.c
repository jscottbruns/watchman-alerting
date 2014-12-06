/**********************************************************************
 *
 *  Scope.c
 *
 *  Since 1999.12.13 by Reed Lai
 *
 **********************************************************************/
/* *INDENT-OFF* */
#include "global.h"
#include "scope.h"

/**********************************************************************
 *  scope_puts
 *
 *  put string to message window
 *
 *  Arguments:
 *    str      point to string
 *    erase    erase message window before put string if TRUE (CLEAR)
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int scope_puts(char *str, int erase)
{
	char c, cL, cH;
	char *s, *d;
	char sbuf[80];

	s = str;
	d = sbuf;

	if (erase) {
		werase(Scope.win->content);
	}
	if (*s) {
		while (*s != 0) {
			if (isgraph(*s)) {
				*d++ = *s++;
			} else {
				c = *s++;
				cL = ASCII(c & 0xf);
				cH = ASCII((c >> 4) & 0xf);
				*d++ = '<';
				*d++ = cH;
				*d++ = cL;
				*d++ = '>';
			}
		}
		*d++ = ' ';
		*d = 0;
		waddstr(Scope.win->content, sbuf);
	}
	wrefresh(Scope.win->content);
	return (0);
}


/**********************************************************************
 *  scope_show
 *
 *  (re)show scope window
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int scope_show()
{
	_i7kon_draw_bwin(Scope.win);
	return (0);
}


/**********************************************************************
 *  scope_release
 *
 *  release scope window
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int scope_release()
{
	return (0);
}


/**********************************************************************
 *  scope_init
 *
 *  initiates scope window
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int scope_init()
{
	Scope.win = win2;
	Scope.win->title = "SCOPE";
	scrollok(Scope.win->content, TRUE);
	return (0);
}
