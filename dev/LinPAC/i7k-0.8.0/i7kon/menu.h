/**********************************************************************
 *
 *  menu.h
 *
 *  v 0.00 1999.11.26 by Reed Lai
 *
 *  header of menu.c
 *
 *  History:
 *
 *  v 0.00 1999.11.26 by Reed Lai
 *      create
 *
 **********************************************************************/
#ifndef _I7KON_MENU_H
#define _I7KON_MENU_H

#include <termios.h>
#include <curses.h>
#include "init.h"
#include "forms.h"
#include "msg.h"
#include "i7kon.h"
#include "i7k.h"

extern int _menu_close_port();
extern int _menu_open_port();
extern int _menu_exec();
extern int _menu_bar();
extern int _menu_list();

extern int menu_main();
extern int menu_show();
extern int menu_release();
extern int menu_init();

#endif							/* _I7KON_MENU_H */
