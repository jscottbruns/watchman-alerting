/**********************************************************************
 *
 *  init.h
 *
 *  v 0.00 1999.11.23 by Reed Lai
 *
 *  header of init.c
 *
 *  Since 1999.11.23 by Reed Lai
 *      create
 *
 **********************************************************************/
/* *INDENT-OFF* */
#ifndef _I7KON_INIT_H
#define _I7KON_INIT_H

#include <curses.h>
#include <signal.h>

#include "global.h"
#include "scope.h"
#include "menu.h"
#include "msg.h"
#include "common.h"

extern int _i7kon_init_curses();
extern bar_t * _i7kon_create_bar(WINDOW *);
extern int _i7kon_release_TB();
extern int _i7kon_release_bwin(i7kon_bwin_t *);
extern int _i7kon_draw_bwin(i7kon_bwin_t *);
extern i7kon_bwin_t *_i7kon_create_bwin(int, int, int, int);

extern int i7kon_TB(char *, char *);

extern void i7kon_finish(int);
extern int i7kon_init();

#endif							/* _I7KON_INIT_H */
