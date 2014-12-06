/**********************************************************************
 *
 *  forms.h
 *
 *  Since 1999.11.25 by Reed Lai
 *
 **********************************************************************/
#ifndef _FORMS_H
#define _FORMS_H

#include <form.h>

#define FORM_MAX_FIELD 256

#define QUIT		CTRL('Q')
#define ESCAPE		CTRL('[')
#define BLANK		' '			/* this is the background character */

typedef struct item {
	unsigned int value;			/* item's integer value */
	char *name;					/* item's string value */
} item_t;

#define FORM_FIELD_NORMAL  0
#define FORM_FIELD_DEFAULT 1
#define FORM_FIELD_CHOSING 2
typedef struct userptr {
	int type;					/* this field's type (FORM_FIELD_ ...) */
	char *dstr;					/* field's default value(string) */
	char *result;				/* buffer to put the result of field-input */
	unsigned int chosen;		/* current item which is selected */
	item_t (*list)[];	/* point to an array of items */
} userptr_t;

#define SECURE   TRUE
#define NOSECURE FALSE
typedef struct field {
	char *label;				/* label on field */
	int y, x, h, w;				/* field's location */
	int secure;					/* SECURE or NOSECURE */
	userptr_t *ptr;		/* hook user extension here */
} field_t;

extern int wGetchar(WINDOW * win);
#define Getchar() wGetchar(stdscr);
extern FIELD *make_label(int frow, int fcol, NCURSES_CONST char *label);
extern FIELD *make_field(int frow, int fcol, int rows, int cols,
						 bool secure);
extern void display_form(FORM * f);
extern void erase_form(FORM * f);
extern int edit_secure(FIELD * me, int c);
extern int form_virtualize(FORM * f, WINDOW * w, int *last_key);
extern int my_form_driver(FORM * form, int c);
extern void demo_forms(void);

#endif							/* _FORMS_H */
