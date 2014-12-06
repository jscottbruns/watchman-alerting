/**********************************************************************
 *
 *  global.c
 *
 *  Since 1999.11.23 by Reed Lai
 *
 **********************************************************************/
/* *INDENT-OFF* */
#include "i7kon.h"
#include "global.h"


/* Space Bar */
/* 0 9 19 29 39 49 59 69 79 */
/* +--------+--------+--------+--------+--------+--------+--------+--------+ */
char sb[] = "                                                                         ";

i7kon_t i7Kon = {
	0,							/* current fd of opened sio */
	"",							/* name of opened port */
	0,							/* current chosen of sio */
	3,							/* current chosen of baudrate */
	3,							/* current chosen of databits */
	0,							/* current chosen of parity */
	0,							/* current chosen of stopbit(s) */
	"",							/* message buffer */
	"",							/* command buffer */
	"",							/* receive buffer */
	3,							/* start baudrate */
	3,							/* stop baudrate */
	"",							/* start address */
	"",							/* stop address */
};

/*
 * System Title and Bottom 
 */
i7kon_TB_t TB;

/* Basic windows */
i7kon_bwin_t *win1;
i7kon_bwin_t *win2;
i7kon_bwin_t *win3;
// i7kon_bwin_t *win4;
i7kon_bwin_t *win5;

/* Functional windows */
device_t Device = {
	0,
};

scope_t Scope = {
	0,
};

menu_t Menu = {
	0,							/* win */
	{							/* list */
	 {MNU_OPEN_PORT, "Open Port", "",},
	 {MNU_SEND_CMD, "Send Command", "",},
	 {MNU_FIND_LEARNNING_KIT, "Find Learning Kit", "",},
	 {MNU_SEARCH_DEVICE, "Search Devices", "",},
	 {MNU_CLOSE_PORT, "Close Port", "",},
	 {MNU_EXIT, "Exit", "",},
	 {MNU_EXIT, 0, 0,},			/* end of list */
	 },
	MENU_DEF_LIST_BEGIN,		/* list_beg */
	MENU_DEF_BAR,				/* bar pointed item in list */
};

// input_t Input = {
// 0,
// "",
// 0,
// };

message_t Message = {
	0,
};

stream_t Stream = {
	"",							/* scope */
	0,							/* scope_len */
};
