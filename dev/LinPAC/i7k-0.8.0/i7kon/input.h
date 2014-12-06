/**********************************************************************
 *
 *  input.h
 *
 *  Header of input.c
 *
 *  History:
 *
 *  Since 1999.11.25 by Reed Lai
 *      create
 *
 **********************************************************************/
/* *INDENT-OFF* */
#ifndef _I7KON_INPUT_H
#define _I7KON_INPUT_H

#include <curses.h>
#include "init.h"
#include "i7kon.h"

extern int input_port();
extern int input_show();
extern int input_release();
extern int input_init();

#endif							/* _I7KON_INPUT_H */
