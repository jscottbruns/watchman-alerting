/**********************************************************************
 *
 *  input.c
 *
 *  v 0.00 1999.11.25 by Reed Lai
 *
 *  functions of input window
 *
 *  History:
 *
 *  v 0.00 1999.11.25 by Reed Lai
 *      create
 *
 **********************************************************************/
#include "input.h"

/**********************************************************************
 *  input_show
 *
 *  (re)show input window
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int input_show()
{
	_i7kon_draw_bwin(input.win);
	return (0);
}


/**********************************************************************
 *  input_release
 *
 *  release input window
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int input_release()
{
	return (0);
}


/**********************************************************************
 *  input_init
 *
 *  initiates input window
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int input_init()
{
	input.win = win4;
	input.win->title = "INPUT";
	return (0);
}
