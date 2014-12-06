/**********************************************************************
 *
 *  forms.c
 *
 *  Since 1999.11.25 by Reed Lai
 *
 **********************************************************************/
/* *INDENT-OFF* */
#include "codes.h"
#include "forms.h"
#include "msg.h"
#include "string.h"

/*
 * Common function to allow ^T to toggle trace-mode in the middle of a
 * test so that trace-files can be made smaller. 
 */
int wGetchar(WINDOW * win)
{
	int c;
#ifdef TRACE
	while ((c = wgetch(win)) == ENCTRL('T')) {
		if (_nc_tracing) {
			save_trace = _nc_tracing;
			_tracef("TOGGLE-TRACING OFF");
			_nc_tracing = 0;
		} else {
			_nc_tracing = save_trace;
		}
		trace(_nc_tracing);
		if (_nc_tracing)
			_tracef("TOGGLE-TRACING ON");
	}
#else
	c = wgetch(win);
#endif
	return c;
}

// #define Getchar() wGetchar(stdscr)

FIELD *make_label(int frow, int fcol, NCURSES_CONST char *label)
{
	FIELD *f = new_field(1, strlen(label), frow, fcol, 0, 0);

	if (f) {
		set_field_buffer(f, 0, label);
		set_field_opts(f, field_opts(f) & ~O_ACTIVE);
	}
	return (f);
}

FIELD *make_field(int frow, int fcol, int rows, int cols, bool secure)
{
	FIELD *f = new_field(rows, cols, frow, fcol, 0, secure ? 1 : 0);

	if (f) {
		set_field_back(f, A_UNDERLINE);
		set_field_userptr(f, (void *) 0);
	}
	return (f);
}

void display_form(FORM * f)
{
	WINDOW *w;
	int rows, cols;
	int y, x;

	scale_form(f, &rows, &cols);
	y = (stdscr->_maxy - stdscr->_begy - rows) / 2;
	x = (stdscr->_maxx - stdscr->_begx - cols) / 2;

	if ((w = newwin(rows + 2, cols + 4, y, x)) != (WINDOW *) 0) {
		set_form_win(f, w);
		set_form_sub(f, derwin(w, rows, cols, 1, 2));
		box(w, 0, 0);
		keypad(w, TRUE);
	}

	if (post_form(f) != E_OK) {
		wrefresh(w);
	}
}

void erase_form(FORM * f)
{
	WINDOW *w = form_win(f);
	WINDOW *s = form_sub(f);

	unpost_form(f);
	werase(w);
	wrefresh(w);
	delwin(s);
	delwin(w);
}

int edit_secure(FIELD * me, int c)
{
	int rows, cols, frow, fcol, nbuf;

	if (field_info(me, &rows, &cols, &frow, &fcol, (int *) 0, &nbuf) ==
		E_OK && nbuf > 0) {
		char temp[80];
		long len;

		strcpy(temp, field_buffer(me, 1));
		len = (long) (char *) field_userptr(me);
		if (c <= KEY_MAX) {
			if (isgraph(c)) {
				temp[len++] = c;
				temp[len] = 0;
				set_field_buffer(me, 1, temp);
				c = '*';
			} else {
				c = 0;
			}
		} else {
			switch (c) {
			case REQ_BEG_FIELD:
			case REQ_CLR_EOF:
			case REQ_CLR_EOL:
			case REQ_DEL_LINE:
			case REQ_DEL_WORD:
			case REQ_DOWN_CHAR:
			case REQ_END_FIELD:
			case REQ_INS_CHAR:
			case REQ_INS_LINE:
			case REQ_LEFT_CHAR:
			case REQ_NEW_LINE:
			case REQ_NEXT_WORD:
			case REQ_PREV_WORD:
			case REQ_RIGHT_CHAR:
			case REQ_UP_CHAR:
				c = 0;			/* we don't want to do inline editing */
				break;
			case REQ_CLR_FIELD:
				if (len) {
					temp[0] = 0;
					set_field_buffer(me, 1, temp);
				}
				break;
			case REQ_DEL_CHAR:
			case REQ_DEL_PREV:
				if (len) {
					temp[--len] = 0;
					set_field_buffer(me, 1, temp);
				}
				break;
			}
		}
		set_field_userptr(me, (void *) len);
	}
	return c;
}

int form_virtualize(FORM * f, WINDOW * w, int *last_key)
{
	static const struct {
		int code;
		int result;
	} lookup[] = {
		{ENCTRL('A'), REQ_NEXT_CHOICE},
		{ENCTRL('B'), REQ_PREV_WORD},
		{ENCTRL('C'), REQ_CLR_EOL},
		{ENCTRL('D'), REQ_DOWN_FIELD},
		{ENCTRL('E'), REQ_END_FIELD},
		{ENCTRL('F'), REQ_NEXT_PAGE},
		{ENCTRL('G'), REQ_DEL_WORD},
		{ENCTRL('H'), REQ_DEL_PREV},
		{ENCTRL('I'), REQ_NEXT_FIELD},	/* Tab */
		{ENCTRL('K'), REQ_CLR_EOF},
		{ENCTRL('L'), REQ_LEFT_FIELD},
		{ENCTRL('M'), REQ_NEW_LINE},
		{ENCTRL('N'), REQ_NEXT_FIELD},
		{ENCTRL('O'), REQ_INS_LINE},
		{ENCTRL('P'), REQ_PREV_FIELD},
		{ENCTRL('R'), REQ_RIGHT_FIELD},
		{ENCTRL('S'), REQ_BEG_FIELD},
		{ENCTRL('U'), REQ_UP_FIELD},
		{ENCTRL('V'), REQ_DEL_CHAR},
		{ENCTRL('W'), REQ_NEXT_WORD},
		{ENCTRL('X'), REQ_CLR_FIELD},
		{ENCTRL('Y'), REQ_DEL_LINE},
		{ENCTRL('Z'), REQ_PREV_CHOICE},
		{ESCAPE, MAX_FORM_COMMAND + 1},
		/* { KEY_IC, REQ_INS_MODE }, */
		/* Insert */
		{KEY_BACKSPACE, REQ_DEL_PREV},
		{KEY_DOWN, REQ_DOWN_CHAR},
		{KEY_END, REQ_LAST_FIELD},
		{KEY_HOME, REQ_FIRST_FIELD},
		{KEY_LEFT, REQ_LEFT_CHAR},
		{KEY_LL, REQ_LAST_FIELD},
		{KEY_FIND, REQ_BEG_FIELD},	/* Home */
		{KEY_SELECT, REQ_END_FIELD},	/* End */
		{KEY_NEXT, REQ_NEXT_FIELD},
		{KEY_NPAGE, REQ_NEXT_PAGE},
		{KEY_PPAGE, REQ_PREV_PAGE},
		{KEY_PREVIOUS, REQ_PREV_FIELD},
		{KEY_RIGHT, REQ_RIGHT_CHAR},
		{KEY_UP, REQ_UP_CHAR},
		{QUIT, MAX_FORM_COMMAND + 1}
	};

	int key_req;
	static int mode = REQ_INS_MODE;
	int c = wGetchar(w);
	unsigned n;
	FIELD *me = current_field(f);
	userptr_t *uptr;
	int rows, cols, frow, fcol, nbuf;

	field_info(me, &rows, &cols, &frow, &fcol, (int *) 0, &nbuf);
	uptr = field_userptr(me);

	*last_key = c;

	/* special key precessing here */
	switch (c) {
	case ENCTRL('J'):
	case ENCTRL('M'):
		if (rows = 1) {
			c = MAX_FORM_COMMAND + 1;
			key_req = FALSE;
		} else {
			key_req = TRUE;
		}
		break;
	case KEY_UP:
		if (uptr->type == FORM_FIELD_CHOSING) {
			if (uptr->chosen > 0) {
				set_field_buffer(me, 0,
								 (*uptr->list)[--uptr->chosen].name);
			}
			c = REQ_END_FIELD;
			key_req = FALSE;
		} else {
			key_req = TRUE;
		}
		break;
	case KEY_DOWN:
		if (uptr->type == FORM_FIELD_CHOSING) {
			if ((*uptr->list)[uptr->chosen + 1].name) {
				set_field_buffer(me, 0,
								 (*uptr->list)[++uptr->chosen].name);
			}
			c = REQ_END_FIELD;
			key_req = FALSE;
		} else {
			key_req = TRUE;
		}
		break;
	case ENCTRL(']'):
	case KEY_IC:
		if (mode == REQ_INS_MODE) {
			mode = REQ_OVL_MODE;
		} else {
			mode = REQ_INS_MODE;
		}
		c = mode;
		key_req = FALSE;
		break;
	default:
		key_req = TRUE;
		break;
	}

	if (key_req) {
		for (n = 0; n < sizeof(lookup) / sizeof(lookup[0]); n++) {
			if (lookup[n].code == c) {
				c = lookup[n].result;
				break;
			}
		}
	}

	/*
	 * Force the field that the user is typing into to be in reverse video,
	 * while the other fields are shown underlined.
	 */

	if (c <= KEY_MAX) {
		c = edit_secure(me, c);
		set_field_back(me, A_REVERSE);
	} else {
		if (c <= MAX_FORM_COMMAND) {
			c = edit_secure(me, c);
		}
		set_field_back(me, A_UNDERLINE);	/* make field available for read */
		switch (uptr->type) {
		case FORM_FIELD_NORMAL:
		case FORM_FIELD_DEFAULT:
			strcpy(uptr->result, field_buffer(me, 0));	/* copy field's result */
			break;
		default:
			break;
		}
	}
	return c;
}

int my_form_driver(FORM * form, int c)
{
	if (c == (MAX_FORM_COMMAND + 1)
		&& form_driver(form, REQ_VALIDATION) == E_OK)
		return (TRUE);
	else {
		beep();
		return (FALSE);
	}
}
int do_form(field_t (*frm)[], int *last_key)
{
	unsigned int n, m;
	int finished = 0, c;
	WINDOW *win;
	FORM *form;
	FIELD *fld[FORM_MAX_FIELD], *secure;
	userptr_t *ptr;

	/*
	 * describe the form 
	 */
	n = 0;
	m = 0;
	while (((*frm)[m].label)
		   && (n < (FORM_MAX_FIELD - 1))) {
		fld[n++] = make_label((*frm)[m].y, (*frm)[m].x, (*frm)[m].label);
		fld[n++] =
			make_field((*frm)[m].y + 1, (*frm)[m].x, (*frm)[m].h,
					   (*frm)[m].w, (*frm)[m].secure);
		ptr = (*frm)[m].ptr;
		if (ptr) {
			set_field_userptr(fld[n - 1], ptr);
			switch (ptr->type) {
			case FORM_FIELD_NORMAL:	/*
										   * nothing special 
										 */
				break;
			case FORM_FIELD_DEFAULT:	/*
										   * general field with 
										   * default 
										 */
				set_field_buffer(fld[n - 1], 0, ptr->dstr);	/*
															   * default 
															   * *
															   * in
															   * *
															   * field 
															 */
				strcpy(ptr->result, ptr->dstr);	/*
												   * also default in
												   * result 
												 */
				// set_field_buffer(fld[n - 1], 0, "$01M"); /*
				// default in
				// field */
				// strcpy(ptr->result, "$01M"); /* also default in
				// result
				// */
				break;
			case FORM_FIELD_CHOSING:	/*
										   * chosing-field for
										   * preseted * items 
										 */
				set_field_opts(fld[n - 1],
							   field_opts(fld[n - 1]) & ~O_EDIT);
				set_field_buffer(fld[n - 1], 0,
								 (*ptr->list)[ptr->chosen].name);
				break;
			default:
				break;
			}
		}
		++m;
	}
	fld[n] = (FIELD *) 0;

	form = new_form(fld);
	display_form(form);
	win = form_win(form);
	raw();

	while (!finished) {
		switch (form_driver
				(form, c = form_virtualize(form, win, last_key))) {
		case E_OK:
			// mvaddstr(5, 57, field_buffer(secure, 1));
			// clrtoeol();
			// refresh();
			break;
		case E_UNKNOWN_COMMAND:
			finished = my_form_driver(form, c);
			break;
		default:
			beep();
			break;
		}
	}

	erase_form(form);
	free_form(form);
	for (c = 0; fld[c] != 0; c++) {
		free_field(fld[c]);
	}
	// noraw();

	return (TRUE);
}

void demo_forms(void)
{
	WINDOW *w;
	FORM *form;
	FIELD *f[12], *secure;
	int finished = 0, c, *last_key = 0;
	unsigned n = 0;

	move(18, 0);

	addstr("Defined form-traversal keys:   ^Q/ESC- exit form\n");


	addstr("^N   -- go to next field       ^P  -- go to previous field\n");


	addstr("Home -- go to first field      End -- go to last field\n");


	addstr("^L   -- go to field to left    ^R  -- go to field to right\n");
	addstr
		("^U   -- move upward to field   ^D  -- move downward to field\n");


	addstr("^W   -- go to next word        ^B  -- go to previous word\n");


	addstr("^S   -- go to start of field   ^E  -- go to end of field\n");

	addstr("^H   -- delete previous char   ^Y  -- delete line\n");


	addstr("^G   -- delete current word    ^C  -- clear to end of line\n");

	addstr("^K   -- clear to end of field  ^X  -- clear field\n");

	addstr("Arrow keys move within a field as you would expect.");

	mvaddstr(4, 57, "Forms Entry Test");

	refresh();

	/*
	 * describe the form 
	 */
	f[n++] = make_label(0, 15, "Sample Form");
	f[n++] = make_label(2, 0, "Last Name");
	f[n++] = make_field(3, 0, 1, 18, FALSE);
	f[n++] = make_label(2, 20, "First Name");
	f[n++] = make_field(3, 20, 1, 12, FALSE);
	f[n++] = make_label(2, 34, "Middle Name");
	f[n++] = make_field(3, 34, 1, 12, FALSE);
	f[n++] = make_label(5, 0, "Comments");
	f[n++] = make_field(6, 0, 4, 46, FALSE);
	f[n++] = make_label(5, 20, "Password:");
	secure = f[n++] = make_field(5, 30, 1, 9, TRUE);
	f[n++] = (FIELD *) 0;

	form = new_form(f);

	display_form(form);

	w = form_win(form);
	raw();
	while (!finished) {
		switch (form_driver(form, c = form_virtualize(form, w, last_key))) {
		case E_OK:
			mvaddstr(5, 57, field_buffer(secure, 1));
			clrtoeol();
			refresh();
			break;
		case E_UNKNOWN_COMMAND:
			finished = my_form_driver(form, c);
			break;
		default:
			beep();
			break;
		}
	}
	erase_form(form);

	free_form(form);
	for (c = 0; f[c] != 0; c++)
		free_field(f[c]);
	noraw();
}
