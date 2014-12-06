/**********************************************************************
 *
 *  dev.h
 *
 *  v 0.00 1999.12.13 by Reed Lai
 *
 *  header of dev.c
 *
 *  History:
 *
 *  v 0.1.0 26 Jun by Reed Lai
 *    Clean up.
 *
 *  v 0.00 1999.12.14 by Reed Lai
 *      create
 *
 **********************************************************************/
#ifndef _I7KON_DEV_H
#define _I7KON_DEV_H

#define CLEAR   TRUE
#define NOCLEAR FALSE

extern int dev_puts(char *str, int erase);
extern int dev_addmod(char *str);
extern int dev_show();
extern int dev_release();
extern int dev_init();

#endif							/* _I7KON_DEV_H */
