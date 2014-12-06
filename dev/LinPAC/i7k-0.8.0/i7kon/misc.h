/**********************************************************************
 *
 *  misc.h
 *
 *  v 0.00 1999.11.26 by Reed Lai
 *
 *  header of misc.c
 *
 *  History:
 *
 *  v 0.00 1999.11.26 by Reed Lai
 *      create
 *
 **********************************************************************/
#ifndef _I7KON_MISC_H
#define _I7KON_MISC_H

#include <curses.h>
#include <signal.h>

#include "input.h"
#include "i7kon.h"
#include "common.h"

extern char *misc_trim_str(char *str);
extern int misc_stream(char *str);
extern int misc_key_stream(int key);

#endif							/* _I7KON_MISC_H */
