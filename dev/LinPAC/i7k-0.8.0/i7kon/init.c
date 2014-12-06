/**********************************************************************
 *
 *  init.c
 *
 *  Since 1999.11.23 by Reed Lai
 *
 **********************************************************************/
/* *INDENT-OFF* */
#include "init.h"
#include "i7kon.h"

/**********************************************************************
 *  _i7kon_init_curses
 *
 *  initiate NCURSES
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int _i7kon_init_curses()
{
	(void) initscr();			/* initialize the curses library */
	keypad(stdscr, TRUE);		/* enable keyboard mapping */
	// (void) nonl();  /* tell curses not to do NL->CR/NL on output */
	(void) cbreak();			/* take input chars one at a time, no wait for \n */
	(void) noecho();			/* don't echo input */

	if (has_colors()) {
		start_color();

		/* Simple color assignment, often all we need. */
		init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
		init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
		init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
		init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
		init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
		init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
		init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
		init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
	}
	return (0);
}


/**********************************************************************
 *  _i7kon_create_bar
 *
 *  create the bar what the width as given window
 *
 *  Arguments:
 *    win      point to WINDOW
 *
 *  Returned:  point to bar which just created, 0 failed
 *
 **********************************************************************/
bar_t *_i7kon_create_bar(WINDOW * win)
{
	bar_t *bar;
	int size;

	bar = (bar_t *) malloc(sizeof(bar_t));
	if (!bar) {
		return (0);
	}

	size = win->_maxx - win->_begx + 1;
	bar->content = (char *) malloc(size + 1);
	if (!bar->content) {
		return (0);
	}

	memset(bar->content, ' ', size);
	bar->content[size] = 0;

	bar->size = size;
	bar->x = win->_maxx - win->_begx;
	bar->y = win->_maxy - win->_begy;

	return (bar);
}


/**********************************************************************
 *  _i7kon_release_TB
 *
 *  set and show Title/Bottom bar
 *
 *  Arguments:
 *    title    point to a string which contains title message
 *             "" do not change
 *    bottom   point to a string which contains bottom message
 *             "" do not change
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int _i7kon_release_TB()
{
	free(TB.title->content);
	free(TB.title);
	TB.title = 0;

	free(TB.bottom->content);
	free(TB.bottom);
	TB.bottom = 0;

	return (0);
}


/**********************************************************************
 *  _i7kon_release_bwins
 *
 *  release basic windows of i7kon system
 *
 *  Arguments: point to basic window
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int _i7kon_release_bwin(i7kon_bwin_t *win)
{
	delwin(win->content);
	delwin(win->frame);
	free(win);
	win = 0;
	return (0);
}


/**********************************************************************
 *  _i7kon_draw_bwin
 *
 *  (re)draw basic windows of i7kon system
 *
 *  Arguments:
 *    win      point to basic window
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int _i7kon_draw_bwin(i7kon_bwin_t *win)
{
	wattrset(win->frame, A_REVERSE);
	mvwaddstr(win->frame, 0, (win->frame->_maxx - strlen(win->title)),
			  win->title);

	wrefresh(win->frame);
	wrefresh(win->content);

	/*
	 * dock the cursor 
	 */
	wmove(stdscr, (stdscr->_maxy), (stdscr->_maxx));
	untouchwin(stdscr);
	wrefresh(stdscr);

	return (0);
}


/**********************************************************************
 *  _i7kon_create_bwin
 *
 *  create basic windows of i7kon system
 *
 *  Arguments:
 *    x, y, w, h
 *
 *  Returned:  point to window, 0 failed
 *
 **********************************************************************/
i7kon_bwin_t *_i7kon_create_bwin(int x, int y, int w, int h)
{
	i7kon_bwin_t *win;

	win = (i7kon_bwin_t *) malloc(sizeof(i7kon_bwin_t));
	if (!win) {
		return (0);
	}

	win->frame = newwin(h, w, y, x);
	if (!win->frame) {
		return (0);
	}
	leaveok(win->frame, TRUE);
	scrollok(win->frame, FALSE);
	keypad(win->frame, TRUE);	/* enable keyboard mapping */
	// wmove(win->frame, h, w);
	// wborder(win->frame, '|', '|', '=', '-', '+', 'X', '*', '*');
	// wattrset(win->frame, A_NORMAL);
	wborder(win->frame, 0, 0, 0, 0, 0, 0, 0, 0);
	// wcolor_set(win->frame, COLOR_WHITE, "");

	win->content = derwin(win->frame, h - 2, w - 2, 1, 1);
	if (!win->content) {
		return (0);
	}
	leaveok(win->content, TRUE);
	scrollok(win->content, FALSE);
	keypad(win->content, TRUE);	/* enable keyboard mapping */
	// wmove(win->content, h, w);

	win->x = x;
	win->y = y;
	win->w = w;
	win->h = h;

	return (win);
}


/**********************************************************************
 *  i7kon_TB
 *
 *  set and show Title/Bottom bar
 *
 *  Arguments:
 *    title    point to a string which contains title message
 *             "" do not change
 *    bottom   point to a string which contains bottom message
 *             "" do not change
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int i7kon_TB(char *title, char *bottom)
{
	void *p;

	if (title) {				/* change the title */
		if (!TB.title) {
			TB.title = _i7kon_create_bar(stdscr);
			if (!TB.title)
				return (-1);	/* failed */
			TB.title->x = 0;
			TB.title->y = 0;	/* put bar at top of window */
		}
		memcpy(TB.title->content, title,
			   MIN(strlen(title), TB.title->size));
		wattrset(stdscr, A_REVERSE);
		mvwaddstr(stdscr, TB.title->y, TB.title->x, TB.title->content);
		wattrset(stdscr, A_NORMAL);
	}

	if (bottom) {				/* change the bottom */
		if (!TB.bottom) {
			TB.bottom = _i7kon_create_bar(stdscr);
			if (!TB.bottom)
				return (-1);	/* failed */
			TB.bottom->x = 0;	/* put bar at bottom of window */
		}
		memcpy(TB.bottom->content, bottom,
			   MIN(strlen(bottom), TB.bottom->size));
		wattrset(stdscr, A_REVERSE);
		mvwaddstr(stdscr, TB.bottom->y, TB.bottom->x, TB.bottom->content);
		wattrset(stdscr, A_NORMAL);
	}

	wrefresh(stdscr);
	return (0);
}


/**********************************************************************
 *  i7kon_finish
 *
 *  finish i7kon system
 *
 *  Arguments:
 *    sig      integer signal
 *
 *  Returned:  none
 *
 **********************************************************************/
void i7kon_finish(int sig)
{

	msg_release();
	// input_release();
	menu_release();
	scope_release();
	dev_release();

	/*
	 * release basic windows 
	 */
	_i7kon_release_bwin(win5);
	// _i7kon_release_bwin(win4);
	_i7kon_release_bwin(win3);
	_i7kon_release_bwin(win2);
	_i7kon_release_bwin(win1);
	_i7kon_release_TB();

	endwin();

	exit(0);
}


/**********************************************************************
 *  i7kon_init
 *
 *  initiate i7kon system
 *
 *  Arguments: none
 *
 *  Returned:  0 ok
 *
 **********************************************************************/
int i7kon_init()
{
	int x, y, w, h;
	int scr_w, scr_h;

	(void) signal(SIGINT, i7kon_finish);	/* arrange interrupts to  terminate */

	/* handle the change of size of window */
	// (void) signal(SIGWINCH, i7kon_winch);

  /**********************************************
   *  initiates NCURSES and root window stdscr  *
   **********************************************/
	_i7kon_init_curses();
	scr_w = WIN_WIDTH(stdscr);
	scr_h = WIN_HEIGHT(stdscr);

	i7kon_TB(I7KON_DEF_TITLE, I7KON_DEF_BOTTOM);	/* set Title and Bottom */

  /**************************
   *  create basic windows  *
   **************************/
	/* win1 */
	x = 0;
	y = I7KON_TITLE_HEIGHT;
	w = (I7KON_WIN1_WIDTH * scr_w) / I7KON_SCR_WIDTH;
	h = (I7KON_WIN1_HEIGHT * scr_h) / I7KON_SCR_HEIGHT;
	win1 = _i7kon_create_bwin(x, y, w, h);

	/* win3 */
	w = (I7KON_WIN3_WIDTH * scr_w) / I7KON_SCR_WIDTH;
	x = scr_w - w;
	y = win1->y;
	h = win1->h;
	win3 = _i7kon_create_bwin(x, y, w, h);

	/* win2 */
	x = win1->w;
	y = win1->y;
	w = scr_w - (win1->w + win3->w);
	h = win1->h;				/* - I7KON_WIN4_HEIGHT; */
	win2 = _i7kon_create_bwin(x, y, w, h);

	/* win4 */
	/* disabled x = win1->w; y = win1->y + win2->h; w = win2->w; h =
	   I7KON_WIN4_HEIGHT; win4 = _i7kon_create_bwin(x, y, w, h); */

	/* win5 */
	x = win1->x;
	y = win1->y + win1->h;
	w = scr_w;
	h = scr_h - (I7KON_TITLE_HEIGHT + win1->h + I7KON_BOTTOM_HEIGHT) - 1;
	win5 = _i7kon_create_bwin(x, y, w, h);

  /*****************************
   *  init functional windows  *
   *****************************/
	dev_init();
	scope_init();
	menu_init();
	// input_init();
	msg_init();

	dev_show();
	scope_show();
	menu_show();
	// input_show();
	msg_show();

	return (0);
}
