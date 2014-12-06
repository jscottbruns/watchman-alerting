/**********************************************************************
 *
 *  misc.c
 *
 *  Since 1999.11.26 by Reed Lai
 *
 **********************************************************************/
/* *INDENT-OFF* */
#include "i7kon.h"
#include "input.h"
#include "misc.h"

/**********************************************************************
 *  _misc_scroll_str
 *
 *  scroll left
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int _misc_scroll_str(char *str, int n)
{
	char *s;
	char *d;

	s = str + n;
	d = str;

	while ((*d++ = *s++) != 0);

	return (0);
}


/**********************************************************************
 *  misc_trim_str
 *
 *  trim space codes of string
 *
 *  Arguments:
 *    str      string to trim
 *
 *  Returned:  point to trimed string
 *
 **********************************************************************/
char *misc_trim_str(char *str)
{
	char *p;

	p = str + strlen(str);

	while (isspace(*--p));
	*++p = 0;

	p = str;
	while (isspace(*p)) {
		++p;
	}

	return (p);
}


/**********************************************************************
 *  misc_stream
 *
 *  show data stream
 *
 *  Arguments: integer key code
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int misc_stream(char *str)
{
	int len;
	int scroll;
	int width;
	int y;

	width = stdscr->_maxx - stdscr->_begx + 1;
	len = Stream.scope_len + strlen(str);
	scroll = len - width;

	if (scroll > 0) {
		_misc_scroll_str(Stream.scope, scroll);
		len = width;
	}
	strcat(Stream.scope, str);

	Stream.scope_len = len;

	y = stdscr->_maxy - I7KON_BOTTOM_HEIGHT;
	mvwaddstr(stdscr, y, 0, Stream.scope);
	touchline(stdscr, y, 1);
	// untouchwin(stdscr);
	wrefresh(stdscr);
	return (0);
}


/**********************************************************************
 *  misc_key_stream
 *
 *  show keying stream
 *
 *  Arguments: integer key code
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int misc_key_stream(int key)
{
	char str[16];

	if ((key < 127) && (key > 31)) {
		sprintf(str, "%c", key);
	} else {
		sprintf(str, "<%u>", key);
	}
	misc_stream(str);
	return (0);
}
