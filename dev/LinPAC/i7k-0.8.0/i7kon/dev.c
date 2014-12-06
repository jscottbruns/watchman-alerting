/**********************************************************************
 *
 *  dev.c
 *
 *  Since 1999.12.14 by Reed Lai
 *
 **********************************************************************/
/* *INDENT-OFF* */
#include "i7kon.h"
#include "global.h"
#include "codes.h"


/**********************************************************************
 *  dev_puts
 *
 *  put string
 *
 *  Arguments:
 *    str      point to string
 *    erase    erase message window before put string if TRUE (CLEAR)
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int dev_puts(char *str, int erase)
{
	if (erase) {
		werase(Device.win->content);
	}
	if (*str != 0) {
		waddstr(Device.win->content, str);
		waddstr(Device.win->content, "\n");
		wrefresh(Device.win->content);
	}

	return (0);
}


/**********************************************************************
 *  dev_addmod
 *
 *  add module to list
 *
 *  Arguments:
 *    str      point to string
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int dev_addmod(char *str)
{
	char *s, *d;
	char sbuf[32];

	s = str;
	d = sbuf;

	if (s && (*s++ == '!')) {
		*d++ = ' ';
		*d++ = *s++;
		*d++ = *s++;
		*d++ = ' ';
		*d++ = 'i';
		while (*s != '\0' && *s != '\r') {
			*d++ = *s++;
		}
		*d++ = '\n';
		*d = 0;
		waddstr(Device.win->content, sbuf);
	}
	wrefresh(Device.win->content);

	return (0);
}


/**********************************************************************
 *  dev_show
 *
 *  (re)show dev window
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int dev_show()
{
	_i7kon_draw_bwin(Device.win);
	return (0);
}


/**********************************************************************
 *  dev_release
 *
 *  release dev window
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int dev_release()
{
	return (0);
}


/**********************************************************************
 *  dev_init
 *
 *  initiates dev window
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int dev_init()
{
	Device.win = win1;
	Device.win->title = "DEV";
	scrollok(Device.win->content, TRUE);
	return (0);
}
