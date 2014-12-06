/**********************************************************************
 *
 *  main.c
 *
 *  Since 1999.11.23 by Reed Lai
 *      create, blah blah...
 *
 **********************************************************************/
#include "input.h"
#include "menu.h"
#include "misc.h"
#include "init.h"
#include "i7kon.h"

int _main_key(int key)
{
	switch (key) {
	case I7KON_KEY_TAB:
		break;
	case I7KON_KEY_UP:
		break;
	case I7KON_KEY_DOWN:
		break;
	case I7KON_KEY_LEFT:
		break;
	case I7KON_KEY_RIGHT:
		break;
	default:
		break;
	}
	return (3);
}


main(int argc, char *argv[])
{
	unsigned int key;
	int focus;

	i7kon_init();

	key = 0;
	focus = 3;
	while ((key != I7KON_KEY_Q) &&
		   (key != I7KON_KEY_q) && (key != I7KON_KEY_ESC)) {
		switch (focus) {
		case 1:				/* device */
			break;
		case 2:				/* scope */
			break;
		case 3:				/* menu */
			key = menu_main();
			focus = _main_key(key);
			break;
		case 4:				/* input */
			break;
		case 5:				/* message */
			break;
		default:
			focus = 3;
			break;
		}
	}

	i7kon_finish(0);			/* we're done */
}
