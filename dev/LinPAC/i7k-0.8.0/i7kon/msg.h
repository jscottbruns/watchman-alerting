/**********************************************************************
 *
 *  msg.h
 *
 *  v 0.00 1999.12.6 by Reed Lai
 *
 *  header of msg.c
 *
 *  History:
 *
 *  v 0.00 1999.12.6 by Reed Lai
 *      create
 *
 **********************************************************************/
#ifndef _I7KON_MSG_H
#define _I7KON_MSG_H

#include <termios.h>
#include <curses.h>
#include "i7kon.h"

#define CLEAR   TRUE
#define NOCLEAR FALSE

extern int msg_puts(char *str, int erase);
extern int msg_show();
extern int msg_release();
extern int msg_init();

#endif							/* _I7KON_MSG_H */
