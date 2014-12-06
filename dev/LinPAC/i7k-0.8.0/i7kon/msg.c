/**********************************************************************
 *
 *  msg.c
 *
 *  Since 1999.12.6 by Reed Lai
 *
 **********************************************************************/
/* *INDENT-OFF* */
#include "global.h"
#include "msg.h"

/**********************************************************************
 *  msg_puts
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
int msg_puts(char *str, int erase)
{
	if (erase) {
		werase(Message.win->content);
	}
	waddstr(Message.win->content, str);
	wrefresh(Message.win->content);
	// _i7kon_draw_bwin(Message.win);
	return (0);
}


/**********************************************************************
 *  msg_show
 *
 *  (re)show msg window
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int msg_show()
{
	_i7kon_draw_bwin(Message.win);
	return (0);
}


/**********************************************************************
 *  msg_release
 *
 *  release msg window
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int msg_release()
{
	return (0);
}


/**********************************************************************
 *  msg_init
 *
 *  initiates msg window
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int msg_init()
{
	Message.win = win5;
	Message.win->title = "MESSAGE";
	scrollok(Message.win->content, TRUE);
	return (0);
}
