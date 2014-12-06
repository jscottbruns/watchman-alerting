/**********************************************************************
 *
 *  scope.h
 *
 *  Header of scope.c
 *
 *  Since 1999.12.13 by Reed Lai
 *      create
 *
 **********************************************************************/
/* *INDENT-OFF* */
#ifndef _I7KON_SCOPE_H
#define _I7KON_SCOPE_H

#include <curses.h>
#include "i7kon.h"
#include "codes.h"

#define CLEAR   TRUE
#define NOCLEAR FALSE

extern int scope_puts(char *, int);
extern int scope_show();
extern int scope_release();
extern int scope_init();

#endif							/* _I7KON_SCOPE_H */
