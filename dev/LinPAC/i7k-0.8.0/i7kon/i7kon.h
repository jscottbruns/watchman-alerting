/**********************************************************************
 *
 *  i7kon.h
 *
 *  Since 1999.11.23 by Reed Lai
 *      create
 *
 **********************************************************************/
#ifndef _I7KON_I7KON_H
#define _I7KON_I7KON_H

#include <curses.h>

#define I7KON_DEF_TITLE  "i7000 Console v0.1.0"
#define I7KON_DEF_BOTTOM ""

#define I7KON_DEF_TIMEOUT    1	/* default time-out 0.1 second */

#define I7KON_SCR_WIDTH      80
#define I7KON_SCR_HEIGHT     24
#define I7KON_TITLE_HEIGHT   1
#define I7KON_BOTTOM_HEIGHT  1
#define I7KON_WIN1_WIDTH     13
#define I7KON_WIN1_HEIGHT    15
#define I7KON_WIN3_WIDTH     22
#define I7KON_WIN4_HEIGHT    4

#define I7KON_KEY_TAB   9
#define I7KON_KEY_ENTER 10
#define I7KON_KEY_ESC   27
#define I7KON_KEY_Q     81
#define I7KON_KEY_q     113
#define I7KON_KEY_UP    KEY_UP
#define I7KON_KEY_DOWN  KEY_DOWN
#define I7KON_KEY_LEFT  KEY_LEFT
#define I7KON_KEY_RIGHT KEY_RIGHT

#define MNU_OPEN_PORT          1
#define MNU_CLOSE_PORT         2
#define MNU_SEND_CMD           3
#define MNU_FIND_LEARNNING_KIT 4
#define MNU_SEARCH_DEVICE      5
#define MNU_EXIT              -1

#define WIN_WIDTH(a)   ((a)->_maxx - (a)->_begx + 1)
#define WIN_HEIGHT(a)  ((a)->_maxy - (a)->_begy + 1)

enum {
	COLOR_SCOPE_TX,
	COLOR_SCOPE_TX_HEX,
	COLOR_SCOPE_RX,
	COLOR_SCOPE_RX_HEX,
	COLOR_LAST_OBJECT
};

#endif							/* _I7KON_I7KON_H */
