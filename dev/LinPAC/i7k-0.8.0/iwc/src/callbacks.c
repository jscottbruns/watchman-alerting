// #define CB_DEBUG /* unmark this to show callbacks.c debug message */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include <gnome.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "network.h"
#include "beat.h"

#include "lemp0_12.xpm"
#include "lemp1_yellow_12.xpm"
#include "lemp2_yellow_12.xpm"
#include "lemp3_yellow_12.xpm"

#include "lemp0_24.xpm"
#include "lemp1_yellow_24.xpm"
#include "lemp2_yellow_24.xpm"
#include "lemp3_yellow_24.xpm"

#define BUF_SIZE 64

GdkPixmap      *pixmap_cnt_lemp0;
GdkPixmap      *pixmap_cnt_lemp1;
GdkPixmap      *pixmap_cnt_lemp2;
GdkPixmap      *pixmap_cnt_lemp3;
GdkBitmap      *pixmap_cnt_lemp0_mask;
GdkBitmap      *pixmap_cnt_lemp1_mask;
GdkBitmap      *pixmap_cnt_lemp2_mask;
GdkBitmap      *pixmap_cnt_lemp3_mask;
GtkStyle       *style;

GdkPixmap      *beat_lemp_ON;
GdkBitmap      *beat_lemp_ON_mask;
GdkPixmap      *beat_lemp_OFF;
GdkBitmap      *beat_lemp_OFF_mask;

GdkPixmap      *pixmap_di_lemp0;
GdkPixmap      *pixmap_di_lemp1;
GdkPixmap      *pixmap_di_lemp2;
GdkPixmap      *pixmap_di_lemp3;
GdkBitmap      *pixmap_di_lemp0_mask;
GdkBitmap      *pixmap_di_lemp1_mask;
GdkBitmap      *pixmap_di_lemp2_mask;
GdkBitmap      *pixmap_di_lemp3_mask;

GdkPixmap      *lemp_H;
GdkPixmap      *lemp_L;
GdkPixmap      *lemp_H_mask;
GdkPixmap      *lemp_L_mask;

int             connected = 0;

int             beat_on = 0;
int             lemp_inverted = 0;	/* switch for inverting lemps */
int             draw_on = 0;	/* switch of drawing area */
int             di_on = 0;	/* swithc of digital input */
int             do_align = 0;	/* switch to align digital output */
unsigned char   di_data = 0;	/* data for digital input */
unsigned char   do_data = 0;	/* data for digital output */

int             fg_a_on = 0;	/* switch of function generator analog out 
				 */
int             fg_d_on = 0;	/* switch of function generator digital
				 * out */
double          (*fg_a) ();	/* pointer to powered function generator */
unsigned int    (*fg_d) ();	/* pointer to powered function generator */

char            r_buf[BUF_SIZE + 1];	/* receive buffer */
char            s_buf[BUF_SIZE + 1];	/* send buffer */
unsigned int    attempted;	/* attempted counter when read response */

GtkAdjustment  *gadj;

GtkObject      *gtk_obj[LAST_OBJECT];

char           *
trim_space(char *str)
{
    char           *p;

    p = str + strlen(str);
    while (isspace(*--p));
    *++p = '\0';
    while (isspace(*str)) {
	++str;
    }
    return str;
}

gboolean
quit(GtkWidget * widget, GdkEvent * event, gpointer user_data)
{
    beat_close();
    net_close();
    gtk_exit(0);
    return FALSE;
}


static GdkPixmap *pixmap = NULL;

#define READ_HIS_LAST 399
static unsigned int read_his[READ_HIS_LAST + 1];

#define READ_MAX      10.000	/* maximum readable value 10 voltage */
static double   draw_scale;

gboolean
on_draw_configure_event(GtkWidget * widget,
			GdkEventConfigure * event, gpointer user_data)
{
  /***************************
   * create the drawing area *
   ***************************/
    if (pixmap) {
	gdk_pixmap_unref(pixmap);
    }
    pixmap = gdk_pixmap_new(widget->window,
			    widget->allocation.width,
			    widget->allocation.height, -1);

  /**************************
   * clear the drawing area *
   **************************/
    gdk_draw_rectangle(pixmap,
		       widget->style->white_gc,
		       TRUE,
		       0, 0,
		       widget->allocation.width,
		       widget->allocation.height);

    return TRUE;
}


gboolean
on_draw_expose_event(GtkWidget * widget,
		     GdkEventExpose * event, gpointer user_data)
{
  /********************
   * refresh (redraw) *
   ********************/
    gdk_draw_pixmap(widget->window,
		    widget->style->fg_gc[GTK_WIDGET_STATE(widget)],
		    pixmap,
		    event->area.x, event->area.y,
		    event->area.x, event->area.y,
		    event->area.width, event->area.height);
    return FALSE;
}

static void
draw_point(GtkWidget * widget, gdouble x, gdouble y)
{
    GdkRectangle    rect;

    rect.x = x;
    rect.y = y;
    rect.width = 1;
    rect.height = 1;
    gdk_draw_point(pixmap, widget->style->black_gc, x, y);
    gtk_widget_draw(widget, &rect);
}

static void
draw_brush(GtkWidget * widget, gdouble x, gdouble y)
{
    GdkRectangle    rect;

    rect.x = x - 5;
    rect.y = y - 5;
    rect.width = 10;
    rect.height = 10;
    gdk_draw_rectangle(pixmap,
		       widget->style->black_gc,
		       TRUE, rect.x, rect.y, rect.width, rect.height);
    gtk_widget_draw(widget, &rect);
}

void
on_draw_beat(GtkWidget * widget, gpointer user_data)
{
    GdkRectangle    rect;
    unsigned int    i,
                    x;
    double          f;
    char           *addr;
    char            s_att[8];

    // static unsigned int j;
    // g_print( "beat on_draw_beat %d\n", j++ );

    addr = trim_space(gtk_entry_get_text(GTK_ENTRY(gtk_obj[DRAW_ENT])));

    if (draw_on && connected) {
    /***********************************************
     * ok, everybody move! move! we got new friend *
     ***********************************************/
	for (i = 0; i < READ_HIS_LAST; i++) {
	    read_his[i] = read_his[i + 1];
	}

    /**************************
     * this is our new friend *
     **************************/
	if (!net_lock()) {
	    if (!net_send_read(addr, r_buf, BUF_SIZE, &attempted)) {
		sprintf(s_att, "%u", attempted);
		gtk_entry_set_text(GTK_ENTRY(gtk_obj[DRAW_ENT3]), s_att);
		gtk_entry_set_text(GTK_ENTRY(gtk_obj[DRAW_ENT2]), r_buf);
		f = atof(r_buf + 2);
		read_his[READ_HIS_LAST] =
		    widget->allocation.height - f * draw_scale;
		// gtk_toggle_button_set_active(
		// GTK_TOGGLE_BUTTON(gtk_obj[DRAW_BTN]), FALSE );
	    }
	    net_unlock();
#ifdef CB_DEBUG
	} else {
	    g_print("net_lock failed: on_draw_beat\n");
#endif				/* CB_DEBUG */
	}

    /*************
     * show time *
     *************/
	gdk_draw_rectangle(pixmap,
			   widget->style->white_gc,
			   TRUE,
			   0, 0,
			   widget->allocation.width,
			   widget->allocation.height);
	for (x = 0; x <= READ_HIS_LAST; x++) {
	    gdk_draw_point(pixmap,
			   widget->style->black_gc, x, read_his[x]);
	}

	rect.x = 0;
	rect.y = 0;
	rect.width = widget->allocation.width;
	rect.height = widget->allocation.height;
	gtk_widget_draw(widget, &rect);
    }
}

gboolean
on_draw_button_press_event(GtkWidget * widget,
			   GdkEventButton * event, gpointer user_data)
{
    if (event->button == 1 && pixmap != NULL) {
	// draw_brush( widget, event->x, event->y );
	draw_point(widget, event->x, event->y);
	// g_print( "event->x %f, event->x %f\n", event->x, event->y );
    }
    return FALSE;
}


gboolean
on_draw_motion_notify_event(GtkWidget * widget,
			    GdkEventMotion * event, gpointer user_data)
{
    int             x,
                    y;
    GdkModifierType state;

    if (event->is_hint) {
	gdk_window_get_pointer(event->window, &x, &y, &state);
    } else {
	x = event->x;
	y = event->y;
	state = event->state;
    }
    if (state & GDK_BUTTON1_MASK && pixmap != NULL) {
	// draw_brush( widget, x, y );
	draw_point(widget, x, y);
    }
    return TRUE;
}


void
on_man_btn_released(GtkButton * button, gpointer user_data)
{
    GtkWidget      *command,
                   *response,
                   *attemp;
    char           *snd,
                   *p;
    char            s_attemp[16];

    attempted = 0;
    command = GTK_WIDGET(gtk_obj[MAN_ENT]);
    response = GTK_WIDGET(gtk_obj[MAN_ENT2]);
    attemp = GTK_WIDGET(gtk_obj[MAN_ENT3]);

    snd = gtk_entry_get_text(GTK_ENTRY(command));
    p = snd + strlen(snd);

    while (isspace(*--p));
    *++p = '\0';

    while (isspace(*snd)) {
	++snd;
    }

    if (connected) {
	if (strlen(snd)) {
	    if (!net_lock()) {
		if (net_send_read(snd, r_buf, BUF_SIZE, &attempted)) {
		    r_buf[0] = '\0';
		    gtk_widget_set_sensitive(response, FALSE);
		    gtk_widget_set_sensitive(attemp, FALSE);
		} else {
		    gtk_widget_set_sensitive(response, TRUE);
		    gtk_widget_set_sensitive(attemp, TRUE);
		}
#ifdef CB_DEBUG
	    } else {
		g_print("net_lock failed: on_man_btn_released\n");
#endif				/* CB_DEBUG */
	    }
	}
    }
    gtk_entry_set_text(GTK_ENTRY(command), snd);
    gtk_entry_set_text(GTK_ENTRY(response), r_buf);
    sprintf(s_attemp, "%d", attempted);
    gtk_entry_set_text(GTK_ENTRY(attemp), s_attemp);
    net_unlock();
}

void
on_man_ent_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[MAN_ENT] = GTK_OBJECT(widget);
    // gtk_object_set_data( GTK_OBJECT(widget), "response", user_data );
    gtk_entry_set_text((GtkEntry *) widget, "#0200.000");
}


gboolean
on_man_ent_key_press_event(GtkWidget * widget,
			   GdkEventKey * event, gpointer user_data)
{
    if (connected) {
	if (event->keyval == GDK_Return) {
	    on_man_btn_released(GTK_BUTTON(gtk_obj[MAN_BTN]), widget);
	}
    }
    return TRUE;
}


gboolean
on_hscale1_button_release_event(GtkWidget * widget,
				GdkEventButton * event, gpointer user_data)
{
    float           f;
    char           *addr;

    addr = trim_space(gtk_entry_get_text(GTK_ENTRY(gtk_obj[HS_ENT])));

    if (connected) {
	f = gadj->value;
	if (!net_lock()) {
	    sprintf(s_buf, "%s%06.3f", addr, f);
	    net_send_read(s_buf, r_buf, BUF_SIZE, &attempted);
	    net_unlock();
#ifdef CB_DEBUG
	} else {
	    g_print("net_lock failed\n");
#endif				/* CD_DEBUG */
	}
    }

    return TRUE;
}

gboolean
on_hscale1_key_press_event(GtkWidget * widget,
			   GdkEventKey * event, gpointer user_data)
{
    float           f;
    char           *addr;

    addr = trim_space(gtk_entry_get_text(GTK_ENTRY(gtk_obj[HS_ENT])));

    if (connected) {
	switch (event->keyval) {
	case GDK_quoteleft:
	    gtk_adjustment_set_value(gadj, 0);
	    gtk_adjustment_value_changed(gadj);
	    break;
	case GDK_1:
	    gtk_adjustment_set_value(gadj, 1);
	    gtk_adjustment_value_changed(gadj);
	    break;
	case GDK_2:
	    gtk_adjustment_set_value(gadj, 2);
	    gtk_adjustment_value_changed(gadj);
	    break;
	case GDK_3:
	    gtk_adjustment_set_value(gadj, 3);
	    gtk_adjustment_value_changed(gadj);
	    break;
	case GDK_4:
	    gtk_adjustment_set_value(gadj, 4);
	    gtk_adjustment_value_changed(gadj);
	    break;
	case GDK_5:
	    gtk_adjustment_set_value(gadj, 5);
	    gtk_adjustment_value_changed(gadj);
	    break;
	case GDK_6:
	    gtk_adjustment_set_value(gadj, 6);
	    gtk_adjustment_value_changed(gadj);
	    break;
	case GDK_7:
	    gtk_adjustment_set_value(gadj, 7);
	    gtk_adjustment_value_changed(gadj);
	    break;
	case GDK_8:
	    gtk_adjustment_set_value(gadj, 8);
	    gtk_adjustment_value_changed(gadj);
	    break;
	case GDK_9:
	    gtk_adjustment_set_value(gadj, 9);
	    gtk_adjustment_value_changed(gadj);
	    break;
	case GDK_0:
	    gtk_adjustment_set_value(gadj, 10);
	    gtk_adjustment_value_changed(gadj);
	    break;
	default:
	    break;
	}

	switch (event->keyval) {
	case GDK_Left:
	    if (gadj->value > 0.001) {	/* do not use "> 0", or will cause 
					 * -0 */
		f = gadj->value - 0.001;
	    } else {
		f = 0;
	    }
	    break;
	case GDK_Right:
	    if (gadj->value < 10) {
		f = gadj->value + 0.001;
	    } else {
		f = 10;
	    }
	    break;
	default:
	    f = gadj->value;
	    break;
	}

	if (!net_lock()) {
	    sprintf(s_buf, "%s%06.3f", addr, f);
	    net_send_read(s_buf, r_buf, BUF_SIZE, &attempted);
	    net_unlock();
#ifdef CB_DEBUG
	} else {
	    g_print("net_lock failed\n");
#endif				/* CB_DEBUG */
	}
    }
    return TRUE;
}


void
on_draw_realize(GtkWidget * widget, gpointer user_data)
{
    draw_scale = widget->allocation.height / READ_MAX;

    gtk_obj[DRAW] = GTK_OBJECT(widget);

    gtk_object_class_add_signals(GTK_OBJECT(widget)->klass,
				 time_signals, LAST_SIGNAL);

    gtk_signal_connect(GTK_OBJECT(widget), "beat",
		       GTK_SIGNAL_FUNC(on_draw_beat), NULL);
}


void
on_hscale1_realize(GtkWidget * widget, gpointer user_data)
{
    gadj = GTK_ADJUSTMENT(gtk_adjustment_new(0, 0, 10, 0.001, 1, 0));
    gtk_range_set_adjustment(GTK_RANGE(widget), gadj);
}


void
on_NAP1_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[MAIN_WIN] = GTK_OBJECT(widget);
    style = gtk_widget_get_style(GTK_WIDGET(gtk_obj[MAIN_WIN]));

    gtk_object_class_add_signals(GTK_OBJECT(widget)->klass,
				 time_signals, LAST_SIGNAL);

    gtk_signal_connect(GTK_OBJECT(widget), "beat",
		       GTK_SIGNAL_FUNC(on_nap1_beat), NULL);


  /***********************
   * lemps for time beat *
   ***********************/
    beat_lemp_OFF =
	gdk_pixmap_create_from_xpm_d(GTK_WIDGET(gtk_obj[MAIN_WIN])->window,
				     &beat_lemp_OFF_mask,
				     &style->bg[GTK_STATE_NORMAL],
				     (gchar **) lemp0_12_xpm);
    beat_lemp_ON =
	gdk_pixmap_create_from_xpm_d(GTK_WIDGET(gtk_obj[MAIN_WIN])->window,
				     &beat_lemp_ON_mask,
				     &style->bg[GTK_STATE_NORMAL],
				     (gchar **) lemp1_yellow_12_xpm);
  /************************
   * lemps for connection *
   ************************/
    pixmap_cnt_lemp0 =
	gdk_pixmap_create_from_xpm_d(GTK_WIDGET(gtk_obj[MAIN_WIN])->window,
				     &pixmap_cnt_lemp0_mask,
				     &style->bg[GTK_STATE_NORMAL],
				     (gchar **) lemp0_12_xpm);
    pixmap_cnt_lemp1 =
	gdk_pixmap_create_from_xpm_d(GTK_WIDGET(gtk_obj[MAIN_WIN])->window,
				     &pixmap_cnt_lemp1_mask,
				     &style->bg[GTK_STATE_NORMAL],
				     (gchar **) lemp1_yellow_12_xpm);
    pixmap_cnt_lemp2 =
	gdk_pixmap_create_from_xpm_d(GTK_WIDGET(gtk_obj[MAIN_WIN])->window,
				     &pixmap_cnt_lemp2_mask,
				     &style->bg[GTK_STATE_NORMAL],
				     (gchar **) lemp2_yellow_12_xpm);
    pixmap_cnt_lemp3 =
	gdk_pixmap_create_from_xpm_d(GTK_WIDGET(gtk_obj[MAIN_WIN])->window,
				     &pixmap_cnt_lemp3_mask,
				     &style->bg[GTK_STATE_NORMAL],
				     (gchar **) lemp3_yellow_12_xpm);

  /***************************
   * lemps for digital input *
   ***************************/
    pixmap_di_lemp0 =
	gdk_pixmap_create_from_xpm_d(GTK_WIDGET(gtk_obj[MAIN_WIN])->window,
				     &pixmap_di_lemp0_mask,
				     &style->bg[GTK_STATE_NORMAL],
				     (gchar **) lemp0_12_xpm);
    pixmap_di_lemp1 =
	gdk_pixmap_create_from_xpm_d(GTK_WIDGET(gtk_obj[MAIN_WIN])->window,
				     &pixmap_di_lemp1_mask,
				     &style->bg[GTK_STATE_NORMAL],
				     (gchar **) lemp1_yellow_12_xpm);
    pixmap_di_lemp2 =
	gdk_pixmap_create_from_xpm_d(GTK_WIDGET(gtk_obj[MAIN_WIN])->window,
				     &pixmap_di_lemp2_mask,
				     &style->bg[GTK_STATE_NORMAL],
				     (gchar **) lemp2_yellow_12_xpm);
    pixmap_di_lemp3 =
	gdk_pixmap_create_from_xpm_d(GTK_WIDGET(gtk_obj[MAIN_WIN])->window,
				     &pixmap_di_lemp3_mask,
				     &style->bg[GTK_STATE_NORMAL],
				     (gchar **) lemp3_yellow_12_xpm);
}

void
on_nap1_beat(GtkWidget * widget, gpointer user_data)
{
    return;
}

double
fg_sine()
{
    static double   deg;
    if (deg > 360) {
	deg = 0;
    } else {
	deg++;
    }
    return (5 + 5 * sin(deg * 0.01745));
}

double
fg_arc_sine()
{
    static double   rate;

    if (rate > 0.990000) {
	rate = -1.000000;
    } else {
	rate += 0.010000;
    }
    return (5 + 5 * asin(rate) / asin(1.0));
}

double
fg_tangent()
{
    static double   deg;
    double          f;

    if (deg > 360) {
	deg = 0;
    } else {
	deg++;
    }
    f = 5 + tan(deg * 0.01745) / 5.000000;
    if (f >= 10.000000) {
	f = 10.000000;
    } else if (f <= 0.000000) {
	f = 0.000000;
    }
    return (f);
}

double
fg_random()
{
    return (READ_MAX * rand() / RAND_MAX);
}

unsigned int
fg_d_inc()
{
    static unsigned int i;

    i++;
    if (i > 15) {
	i = 0;
    }
    return (i);
}

unsigned int
fg_d_scan()
{
    static unsigned int i;
    i *= 2;
    if (i > 15 || i == 0) {
	i = 1;
    }
    return (i);
}

unsigned int
fg_d_random()
{
    return (15.0 * rand() / RAND_MAX);
}

void
on_fg_tbtn_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
    if (gtk_toggle_button_get_active(togglebutton)) {
	fg_a_on = 1;		/* enable function generator */
    } else {
	fg_a_on = 0;		/* disable function generator */
    }
}

void
on_fg_com_ent_changed(GtkEditable * editable, gpointer user_data)
{
    char           *fn;

    fn = gtk_editable_get_chars(editable, 0, -1);
    if (strcmp("sine", fn) == 0) {
	fg_a = fg_sine;
    } else if (strcmp("tangent", fn) == 0) {
	fg_a = fg_tangent;
    } else if (strcmp("arc sine", fn) == 0) {
	fg_a = fg_arc_sine;
    } else if (strcmp("random", fn) == 0) {
	fg_a = fg_random;
    }
}

void
on_fg_a_beat(GtkWidget * widget, gpointer user_data)
{
    char           *addr;
    addr = trim_space(gtk_entry_get_text(GTK_ENTRY(gtk_obj[FG_A_ENT])));

    if (fg_a_on && connected) {
	if (!net_lock()) {
	    sprintf(s_buf, "%s%06.3f", addr, fg_a());
	    net_send_read(s_buf, r_buf, BUF_SIZE, &attempted);
	    net_unlock();
#ifdef CB_DEBUG
	} else {
	    g_print("net_lock failed: on_fg_a_beat\n");
#endif				/* CB_DEBUG */
	}
    }
}

void
on_fg_com_realize(GtkWidget * widget, gpointer user_data)
{
    fg_a = fg_sine;
    gtk_obj[FG_A] = GTK_OBJECT(widget);

    gtk_object_class_add_signals(GTK_OBJECT(widget)->klass,
				 time_signals, LAST_SIGNAL);

    gtk_signal_connect(GTK_OBJECT(widget), "beat",
		       GTK_SIGNAL_FUNC(on_fg_a_beat), NULL);
}

void
on_man_btn_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[MAN_BTN] = GTK_OBJECT(widget);
}

void
on_man_ent2_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[MAN_ENT2] = GTK_OBJECT(widget);
}

void
on_draw_tbtn_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
    if (gtk_toggle_button_get_active(togglebutton)) {
	draw_on = 1;		/* enable drawing area */
    } else {
	draw_on = 0;		/* disable drawing area */
    }
}


void
on_ip_ent_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[IP_ENT] = GTK_OBJECT(widget);
    gtk_entry_set_text((GtkEntry *) widget, "192.168.255.12");
}


void
on_port_ent_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[PORT_ENT] = GTK_OBJECT(widget);
    gtk_entry_set_text((GtkEntry *) widget, "1500");
}


void
on_cnt_btn_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
#define A_MAX 64
    char           *s_ip,
                   *s_port,
                   *p,
                   *q;
    char            a[A_MAX + 1];
    unsigned int    i,
                    n;
    unsigned long int addr[4];
    unsigned short int port;

    gtk_obj[CNT_BTN] = GTK_OBJECT(togglebutton);
    if (gtk_toggle_button_get_active(togglebutton)) {	/* make connection 
							 */

    /********************
     * parse ip address *
     ********************/
	s_ip = gtk_entry_get_text(GTK_ENTRY(gtk_obj[IP_ENT]));
	p = s_ip + strlen(s_ip);
	while (isspace(*--p));
	*++p = '\0';
	while (isspace(*s_ip)) {
	    ++s_ip;
	}
	n = A_MAX;
	q = s_ip;
	for (p = a; 0 < n && *q != '\0'; --n) {
	    if (*q == '.') {
		*p++ = '\0';
		q++;
	    } else {
		*p++ = *q++;
	    }
	}
	for (; 0 < n; --n) {
	    *p++ = '\0';
	}
	p = a;
	for (i = 0; i < 4; i++) {
	    addr[i] = atoi(p);
	    while (*p++ != '\0');
	}

    /***************
     * port number *
     ***************/
	s_port = gtk_entry_get_text(GTK_ENTRY(gtk_obj[PORT_ENT]));
	p = s_port + strlen(s_port);
	while (isspace(*--p));
	*++p = '\0';
	while (isspace(*s_port)) {
	    ++s_port;
	}			/* port string */
	port = atoi(s_port);

	// g_print( "connect: %ld.%ld.%ld.%ld:%d\n", addr[0], addr[1],
	// addr[2], addr[3], port );

    /**************
     * connection *
     **************/
	if (net_open(addr, port)) {	/* failed */
	    connected = 0;
	    gtk_toggle_button_set_active(togglebutton, FALSE);
	} else {		/* successful */
	    gtk_pixmap_set(GTK_PIXMAP(gtk_obj[CNT_LEMP]),
			   pixmap_cnt_lemp1, pixmap_cnt_lemp1_mask);
	    connected = 1;
	}
	gtk_entry_set_text(GTK_ENTRY(gtk_obj[IP_ENT]), s_ip);
	gtk_entry_set_text(GTK_ENTRY(gtk_obj[PORT_ENT]), s_port);

    } else {			/* do disconnection */
	connected = 0;
	net_close();
	gtk_pixmap_set(GTK_PIXMAP(gtk_obj[CNT_LEMP]),
		       pixmap_cnt_lemp0, pixmap_cnt_lemp0_mask);
    }
}


void
on_cnt_lemp_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[CNT_LEMP] = GTK_OBJECT(widget);
}


void
on_fg_tbtn2_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
    if (gtk_toggle_button_get_active(togglebutton)) {
	fg_d_on = 1;		/* enable function generator digital out */
    } else {
	fg_d_on = 0;		/* disable function generator digital out */
    }
}


void
on_fg_d_beat(GtkWidget * widget, gpointer user_data)
{
    // static int beat;
    char           *addr;

    addr = trim_space(gtk_entry_get_text(GTK_ENTRY(gtk_obj[FG_D_ENT])));

    // beat++;
    // if (beat > 2) { beat = 0; }

    // if (fg_d_on && connected && !beat) {
    if (fg_d_on && connected) {
	if (!net_lock()) {
	    sprintf(s_buf, "%s0A%02X", addr, fg_d());
	    net_send_read(s_buf, r_buf, BUF_SIZE, &attempted);
	    net_unlock();
#ifdef CB_DEBUG
	} else {
	    g_print("net_lock failed: on_fg_a_beat\n");
#endif				/* CB_DEBUG */
	}
    }
}


void
on_fg_com2_realize(GtkWidget * widget, gpointer user_data)
{
    fg_d = fg_d_inc;
    gtk_obj[FG_D] = GTK_OBJECT(widget);

    gtk_object_class_add_signals(GTK_OBJECT(widget)->klass,
				 time_signals, LAST_SIGNAL);

    gtk_signal_connect(GTK_OBJECT(widget), "beat",
		       GTK_SIGNAL_FUNC(on_fg_d_beat), NULL);
}


void
on_fg_com2_ent_changed(GtkEditable * editable, gpointer user_data)
{
    char           *fn;

    fn = gtk_editable_get_chars(editable, 0, -1);
    if (strcmp("inc", fn) == 0) {
	fg_d = fg_d_inc;
    } else if (strcmp("scan", fn) == 0) {
	fg_d = fg_d_scan;
    } else if (strcmp("random", fn) == 0) {
	fg_d = fg_d_random;
    }
}


void
on_man_ent3_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[MAN_ENT3] = GTK_OBJECT(widget);
}


void
on_di_beat(GtkWidget * widget, gpointer user_data)
{
    char           *addr;

    addr = trim_space(gtk_entry_get_text(GTK_ENTRY(gtk_obj[DI_ENT])));

    if (di_on && connected) {

    /*****************
     * read from DIO *
     *****************/
	if (!net_lock()) {
	    if (!net_send_read(addr, r_buf, BUF_SIZE, &attempted)) {
		di_data = r_buf[4];
		if (di_data >= 'A') {
		    di_data -= 55;
		} else {
		    di_data -= 48;
		}
		do_data = r_buf[2];
		if (do_data >= 'A') {
		    do_data -= 55;
		} else {
		    do_data -= 48;
		}
	    }
	    net_unlock();
#ifdef CB_DEBUG
	} else {
	    g_print("net_lock failed: on_di_beat\n");
#endif				/* CB_DEBUG */
	}

    /*************
     * show time *
     *************/
	if (lemp_inverted) {	/* inverts lemps */
	    lemp_L = pixmap_di_lemp2;
	    lemp_L_mask = pixmap_di_lemp2_mask;
	    lemp_H = pixmap_di_lemp0;
	    lemp_H_mask = pixmap_di_lemp0_mask;
	} else {		/* do not invert lemps */
	    lemp_L = pixmap_di_lemp0;
	    lemp_L_mask = pixmap_di_lemp0_mask;
	    lemp_H = pixmap_di_lemp2;
	    lemp_H_mask = pixmap_di_lemp2_mask;
	}

	if (di_data & 0x08) {
	    gtk_pixmap_set(GTK_PIXMAP(gtk_obj[DI1]), lemp_H, lemp_H_mask);
	} else {
	    gtk_pixmap_set(GTK_PIXMAP(gtk_obj[DI1]), lemp_L, lemp_L_mask);
	}
	if (di_data & 0x04) {
	    gtk_pixmap_set(GTK_PIXMAP(gtk_obj[DI2]), lemp_H, lemp_H_mask);
	} else {
	    gtk_pixmap_set(GTK_PIXMAP(gtk_obj[DI2]), lemp_L, lemp_L_mask);
	}
	if (di_data & 0x02) {
	    gtk_pixmap_set(GTK_PIXMAP(gtk_obj[DI3]), lemp_H, lemp_H_mask);
	} else {
	    gtk_pixmap_set(GTK_PIXMAP(gtk_obj[DI3]), lemp_L, lemp_L_mask);
	}
	if (di_data & 0x01) {
	    gtk_pixmap_set(GTK_PIXMAP(gtk_obj[DI4]), lemp_H, lemp_H_mask);
	} else {
	    gtk_pixmap_set(GTK_PIXMAP(gtk_obj[DI4]), lemp_L, lemp_L_mask);
	}

    /********************************
     * align digital output buttons *
     ********************************/
	if (do_align) {
	    if (do_data & 0x08) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (gtk_obj[DO1]), TRUE);
	    } else {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (gtk_obj[DO1]), FALSE);
	    }
	    if (do_data & 0x04) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (gtk_obj[DO2]), TRUE);
	    } else {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (gtk_obj[DO2]), FALSE);
	    }
	    if (do_data & 0x02) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (gtk_obj[DO3]), TRUE);
	    } else {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (gtk_obj[DO3]), FALSE);
	    }
	    if (do_data & 0x01) {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (gtk_obj[DO4]), TRUE);
	    } else {
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
					     (gtk_obj[DO4]), FALSE);
	    }
	}
    }
}


void
on_di1_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[DI1] = GTK_OBJECT(widget);
}


void
on_di2_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[DI2] = GTK_OBJECT(widget);
}


void
on_di3_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[DI3] = GTK_OBJECT(widget);
}


void
on_di4_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[DI4] = GTK_OBJECT(widget);
}


void
on_di_tbtn_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
    if (gtk_toggle_button_get_active(togglebutton)) {
	di_on = 1;		/* enable digital input */
    } else {
	di_on = 0;		/* disable digital input */
    }
}

void
do_set(unsigned char d)
{
    char           *addr;

    addr = trim_space(gtk_entry_get_text(GTK_ENTRY(gtk_obj[DO_ENT])));

    if (!net_lock()) {
	sprintf(s_buf, "%s0A%02X", addr, d);
	net_send_read(s_buf, r_buf, BUF_SIZE, &attempted);
	net_unlock();
#ifdef CB_DEBUG
    } else {
	g_print("net_lock failed: do_set\n");
#endif				/* CB_DEBUG */
    }
}

void
on_do4_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
    if (gtk_toggle_button_get_active(togglebutton)) {
	do_data |= 0x01;	/* DO 4 on */
    } else {
	do_data &= 0x0E;	/* DO 4 off */
    }
    if (connected) {
	do_set(do_data);
    }
}


void
on_do4_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[DO4] = GTK_OBJECT(widget);
}


void
on_do3_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
    if (gtk_toggle_button_get_active(togglebutton)) {
	do_data |= 0x02;	/* DO 3 on */
    } else {
	do_data &= 0x0D;	/* DO 3 off */
    }
    if (connected) {
	do_set(do_data);
    }
}


void
on_do3_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[DO3] = GTK_OBJECT(widget);
}


void
on_do_tbtn_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
    if (gtk_toggle_button_get_active(togglebutton)) {
	do_align = 1;		/* align digital output */
    } else {
	do_align = 0;		/* do not align digital output */
    }
}


void
on_do2_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
    if (gtk_toggle_button_get_active(togglebutton)) {
	do_data |= 0x04;	/* DO 2 on */
    } else {
	do_data &= 0x0B;	/* DO 2 off */
    }
    if (connected) {
	do_set(do_data);
    }
}


void
on_do2_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[DO2] = GTK_OBJECT(widget);

}


void
on_do1_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
    if (gtk_toggle_button_get_active(togglebutton)) {
	do_data |= 0x08;	/* DO 1 on */
    } else {
	do_data &= 0x07;	/* DO 1 off */
    }
    if (connected) {
	do_set(do_data);
    }
}


void
on_do1_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[DO1] = GTK_OBJECT(widget);
}


void
on_di_tbtn_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[DI_BTN] = GTK_OBJECT(widget);
    gtk_object_class_add_signals(GTK_OBJECT(widget)->klass,
				 time_signals, LAST_SIGNAL);
    gtk_signal_connect(GTK_OBJECT(widget), "beat",
		       GTK_SIGNAL_FUNC(on_di_beat), NULL);
}


void
on_do_tbtn_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[DO_BTN] = GTK_OBJECT(widget);
}

void
on_fg_d_ent_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[FG_D_ENT] = GTK_OBJECT(widget);
    gtk_entry_set_text(GTK_ENTRY(widget), "#03");
}


void
on_fg_a_ent_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[FG_A_ENT] = GTK_OBJECT(widget);
    gtk_entry_set_text(GTK_ENTRY(widget), "#02");
}


void
on_draw_ent_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[DRAW_ENT] = GTK_OBJECT(widget);
    gtk_entry_set_text(GTK_ENTRY(widget), "#01");
}


void
on_di_ent_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[DI_ENT] = GTK_OBJECT(widget);
    gtk_entry_set_text(GTK_ENTRY(widget), "$036");
}


void
on_fg_a_ent_changed(GtkEditable * editable, gpointer user_data)
{
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_obj[FG_A_BTN]),
				 FALSE);
}


void
on_fg_d_ent_changed(GtkEditable * editable, gpointer user_data)
{
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_obj[FG_D_BTN]),
				 FALSE);
}


void
on_hs_ent_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[HS_ENT] = GTK_OBJECT(widget);
    gtk_entry_set_text(GTK_ENTRY(widget), "#02");
}


void
on_hs_ent_changed(GtkEditable * editable, gpointer user_data)
{
}


void
on_draw_ent_changed(GtkEditable * editable, gpointer user_data)
{
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_obj[DRAW_BTN]),
				 FALSE);
}


void
on_di_ent_changed(GtkEditable * editable, gpointer user_data)
{
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_obj[DI_BTN]),
				 FALSE);
}


void
on_fg_tbtn_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[FG_A_BTN] = GTK_OBJECT(widget);
}


void
on_fg_tbtn2_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[FG_D_BTN] = GTK_OBJECT(widget);
}


void
on_draw_tbtn_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[DRAW_BTN] = GTK_OBJECT(widget);
}





void
on_do_ent_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[DO_ENT] = GTK_OBJECT(widget);
    gtk_entry_set_text(GTK_ENTRY(widget), "#03");
}


void
on_do_ent_changed(GtkEditable * editable, gpointer user_data)
{
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_obj[DO_BTN]),
				 FALSE);
}


void
on_beat_sb_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[BEAT_SB] = GTK_OBJECT(widget);
}


void
on_beat_sb_changed(GtkEditable * editable, gpointer user_data)
{
    char            s_ms[8];
    char           *s_t;
    unsigned int    t;

    s_t = trim_space(gtk_entry_get_text(GTK_ENTRY(editable)));
    t = atoi(s_t) * 10;
    sprintf(s_ms, "%u", t);
    gtk_entry_set_text(GTK_ENTRY(gtk_obj[BEAT_ENT]), s_ms);

    if (beat_on) {
	t *= 1000;
	itv.it_value.tv_sec = t / 1000000;
	itv.it_value.tv_usec = t % 1000000;
	itv.it_interval.tv_sec = itv.it_value.tv_sec;
	itv.it_interval.tv_usec = itv.it_value.tv_usec;
	setitimer(ITIMER_REAL, &itv, 0);
    }
}


void
on_beat_ent_realize(GtkWidget * widget, gpointer user_data)
{
    char            s_ms[8];
    char           *s_t;
    unsigned int    t;

    gtk_obj[BEAT_ENT] = GTK_OBJECT(widget);

    s_t = trim_space(gtk_entry_get_text(GTK_ENTRY(gtk_obj[BEAT_SB])));
    t = atoi(s_t) * 10;
    sprintf(s_ms, "%u", t);
    gtk_entry_set_text(GTK_ENTRY(widget), s_ms);
}


void
on_lemp_inv_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[LEMP_INV] = GTK_OBJECT(widget);
}


void
on_lemp_inv_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
    if (gtk_toggle_button_get_active(togglebutton)) {
	lemp_inverted = 1;	/* inverts lemps */
    } else {
	lemp_inverted = 0;	/* don't invert lemps */
    }
}


void
on_beat_tbtn_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[BEAT_BTN] = GTK_OBJECT(widget);
}


void
on_beat_tbtn_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
    char            s_ms[8];
    char           *s_t;
    unsigned int    t;

    if (gtk_toggle_button_get_active(togglebutton)) {	/* enable time
							 * beat */

	beat_on = 1;
	gtk_pixmap_set(GTK_PIXMAP(gtk_obj[BEAT_LEMP]),
		       beat_lemp_ON, beat_lemp_ON_mask);
	s_t = trim_space(gtk_entry_get_text(GTK_ENTRY(gtk_obj[BEAT_SB])));
	t = atoi(s_t) * 10;
	sprintf(s_ms, "%u", t);
	gtk_entry_set_text(GTK_ENTRY(gtk_obj[BEAT_ENT]), s_ms);
	t *= 1000;
	itv.it_value.tv_sec = t / 1000000;
	itv.it_value.tv_usec = t % 1000000;

    } else {			/* disable time beat */

	beat_on = 0;
	gtk_pixmap_set(GTK_PIXMAP(gtk_obj[BEAT_LEMP]),
		       beat_lemp_OFF, beat_lemp_OFF_mask);
	itv.it_value.tv_sec = 0;
	itv.it_value.tv_usec = 0;

    }

    itv.it_interval.tv_sec = itv.it_value.tv_sec;
    itv.it_interval.tv_usec = itv.it_value.tv_usec;
    setitimer(ITIMER_REAL, &itv, 0);
}


void
on_beat_lemp_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[BEAT_LEMP] = GTK_OBJECT(widget);
}


void
on_draw_ent2_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[DRAW_ENT2] = GTK_OBJECT(widget);
}

void
on_draw_ent3_realize(GtkWidget * widget, gpointer user_data)
{
    gtk_obj[DRAW_ENT3] = GTK_OBJECT(widget);
}
