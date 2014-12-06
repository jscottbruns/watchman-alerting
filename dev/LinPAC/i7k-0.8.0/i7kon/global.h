/**********************************************************************
 *
 *  global.h
 *
 *  Header of global.c
 *
 *  History:
 *
 *  Since 1999.11.23 by Reed Lai
 *      create
 *
 **********************************************************************/
#ifndef _I7KON_GLOBAL_H
#define _I7KON_GLOBAL_H

#include <curses.h>

extern char sb[];

#define I7KON_MSG_SIZE 80
#define I7KON_CMD_SIZE 48
#define I7KON_RCV_SIZE 32
#define I7KON_ADR_SIZE 5		/* address size 3 + 1 character for work around the bug of overkeying from field + the '\0' of ending stringwhen strcpy() then total size = 5 */
typedef struct i7kon {
	int sio_fd;					/* current fd of opened sio */
	char *port_name;
	unsigned int port;			/* current chosen of sio */
	unsigned int baud;			/* current chosen of baudrate of sio */
	unsigned int data;			/* current chosen of databits of sio */
	unsigned int parity;		/* current chosen of parity of sio */
	unsigned int stop;			/* current chosen of stopbits of sio */
	char msg[I7KON_MSG_SIZE];	/* message buffer (last parsed message) */
	char cmd[I7KON_CMD_SIZE];	/* last command buffer */
	char rcv[I7KON_RCV_SIZE];	/* receive buffer */
	unsigned int beg_baud;		/* the beginning of baudrate to search devices */
	unsigned int end_baud;		/* the stopping of baudrate to search  devices */
	char beg_addr[I7KON_ADR_SIZE];	/* the beginning of * address of module to search */
	char end_addr[I7KON_ADR_SIZE];	/* the stopping of address of module to search */
} i7kon_t;
extern i7kon_t i7Kon;

/* bar */
typedef struct bar {
	char *content;				/* content (buffer) of bar */
	int size;					/* size of bar in bytes (characters) */
	int x, y;					/* coords of location on screen */
} bar_t;

/* System title and bottom */
typedef struct i7kon_TB {
	bar_t *title;		/* title bar */
	bar_t *bottom;		/* bottom bar */
} i7kon_TB_t;

extern i7kon_TB_t TB;

/* Layout of basic windows 
   0    5   10   15   20   25   30   35   40   45   50   55   60   65   70   75   80
   +----+----+----+----+----+----+----+----+----+----+----+----+----+----+----+---++

   =Title=bar===================================0==================================
   +----------++--------------------------------1-----------++--------------------+
   |          ||                                            ||                    |
   |          ||                                            ||                    |
   |          ||                                            ||                    |
   |          ||                                            ||                    |
   |   win1   ||                   win2                     ||        win3        |
   |          ||                                            ||                    |
   |          ||                                            ||                    |
   |          ||                                            ||                    |
   |          ||                                            ||                    |
   |          ||                                            ||                    |
   |          ||                                            ||                    |
   |          ||                                            ||                    |
   |          |+--------------------------------14----------+|                    |
   |          |+================================15==========+|                    |
   |          ||                   win4                     ||                    |
   +----------++--------------------------------17----------++--------------------+
   +--------------------------------------------18--------------------------------+
   |                                                                              |
   |                               win5                                           |
   |                                                                              |
   |                                                                              |
   +--------------------------------------------23--------------------------------+
   =============================================24===================Bottom=bar====
*/
/* Basic window type of i7kon */
typedef struct i7kon_bwin {
	char *title;
	WINDOW *frame;
	WINDOW *content;
	int x, y;					/* coords of this window */
	int w, h;					/* size, includes frame */
} i7kon_bwin_t;

extern i7kon_bwin_t *win1;
extern i7kon_bwin_t *win2;
extern i7kon_bwin_t *win3;
// extern i7kon_bwin_t *win4;
extern i7kon_bwin_t *win5;

/* Functional windows */
typedef struct device {
	i7kon_bwin_t *win;
} device_t;

typedef struct scope {
	i7kon_bwin_t *win;
} scope_t;

typedef struct item_pair {
	int id;
	char *name;
	char *verbose;
} item_pair_t;

#define MENU_LIST_SIZE      256
#define MENU_DEF_LIST_BEGIN 0
#define MENU_DEF_BAR        0
typedef struct menu {
	i7kon_bwin_t *win;
	item_pair_t list[MENU_LIST_SIZE];
	int list_beg;
	int bar;					/* selection bar */
} menu_t;

// #define INPUT_SCOPE_SIZE 256
// typedef struct input{
// i7kon_bwin_t *win;
// char scope[INPUT_SCOPE_SIZE]; /* keyboard input scope */
// int scope_len; /* current string length in scope */
// } input_t;

typedef struct message {
	i7kon_bwin_t *win;
} message_t;

extern device_t Device;
extern scope_t Scope;
extern menu_t Menu;
// extern struct input_t input;
extern message_t Message;

#define STREAM_SCOPE_SIZE 256
typedef struct stream {
	char scope[STREAM_SCOPE_SIZE];	/* keyboard input scope */
	int scope_len;				/* current string length in scope */
} stream_t;

extern stream_t Stream;

#endif							/* _I7KON_GLOBAL_H */
