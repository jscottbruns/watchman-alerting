#include <gnome.h>

extern int      man_blocked;
extern int      fg_on;

enum {				/* change this, should recompile all
				 * modules */
    MAIN_WIN,
    IP_ENT,
    PORT_ENT,
    CNT_BTN,
    CNT_LEMP,
    FG_A,
    FG_A_BTN,
    FG_A_ENT,
    FG_D,
    FG_D_BTN,
    FG_D_ENT,
    MAN_BTN,
    MAN_ENT,
    MAN_ENT2,
    MAN_ENT3,
    HS_ENT,
    DRAW,
    DRAW_BTN,
    DRAW_ENT,
    DRAW_ENT2,
    DRAW_ENT3,
    LEMP_INV,
    DI_BTN,
    DI_ENT,
    DI1,
    DI2,
    DI3,
    DI4,
    DO_BTN,
    DO_ENT,
    DO1,
    DO2,
    DO3,
    DO4,
    BEAT_SB,
    BEAT_ENT,
    BEAT_BTN,
    BEAT_LEMP,
    XXXX,
    LAST_OBJECT
};

extern GtkObject *gtk_obj[LAST_OBJECT];	/* member objects here */

void
                on_man_send_released(GtkButton * button,
				     gpointer user_data);

void
                on_fg_on_released(GtkButton * button, gpointer user_data);

void
                on_fg_tbut_toggled(GtkToggleButton * togglebutton,
				   gpointer user_data);







gboolean
       quit(GtkWidget * widget, GdkEvent * event, gpointer user_data);
void
                on_man_but_released(GtkButton * button,
				    gpointer user_data);

void
                on_man_btn_released(GtkButton * button,
				    gpointer user_data);

void
                on_man_btn_released(GtkButton * button,
				    gpointer user_data);

void
                on_fg_tbtn_toggled(GtkToggleButton * togglebutton,
				   gpointer user_data);

void
                on_man_btn_released(GtkButton * button,
				    gpointer user_data);

void
                on_man_btn_released(GtkButton * button,
				    gpointer user_data);

void
                on_man_btn_released(GtkButton * button,
				    gpointer user_data);

void
                on_man_btn_released(GtkButton * button,
				    gpointer user_data);

void
                on_man_btn_released(GtkButton * button,
				    gpointer user_data);

void
                on_man_btn_released(GtkButton * button,
				    gpointer user_data);

void
                on_man_btn_enter(GtkButton * button, gpointer user_data);

void
                on_man_btn_released(GtkButton * button,
				    gpointer user_data);

void
                on_man_btn_released(GtkButton * button,
				    gpointer user_data);

void
                on_man_btn_released(GtkButton * button,
				    gpointer user_data);

void
                on_man_btn_released(GtkButton * button,
				    gpointer user_data);

void
                on_man_btn_released(GtkButton * button,
				    gpointer user_data);

void
                on_man_btn_released(GtkButton * button,
				    gpointer user_data);

gboolean
 
 
 
 on_man_ent_configure_event(GtkWidget * widget,
			    GdkEventConfigure * event, gpointer user_data);

void
                on_man_ent_activate(GtkEditable * editable,
				    gpointer user_data);

void
                on_man_ent_parent_set(GtkWidget * widget,
				      GtkObject * old_parent,
				      gpointer user_data);

gboolean
 
 
 
 on_man_ent_configure_event(GtkWidget * widget,
			    GdkEventConfigure * event, gpointer user_data);

gboolean
 
 
 
 on_man_ent_configure_event(GtkWidget * widget,
			    GdkEventConfigure * event, gpointer user_data);

gboolean
 
 
 
 on_man_ent_configure_event(GtkWidget * widget,
			    GdkEventConfigure * event, gpointer user_data);

gboolean
 
 
 
 on_man_ent_focus_out_event(GtkWidget * widget,
			    GdkEventFocus * event, gpointer user_data);

gboolean
 
 
 
 on_man_ent_configure_event(GtkWidget * widget,
			    GdkEventConfigure * event, gpointer user_data);

void
                on_man_ent_realize(GtkWidget * widget, gpointer user_data);

gboolean
 
 
 
 on_man_ent_key_press_event(GtkWidget * widget,
			    GdkEventKey * event, gpointer user_data);

gboolean
 
 
 
 on_hscale1_button_release_event(GtkWidget * widget,
				 GdkEventButton * event,
				 gpointer user_data);

gboolean
 
 
 
 on_hscale1_key_release_event(GtkWidget * widget,
			      GdkEventKey * event, gpointer user_data);

gboolean
 
 
 
 on_hscale1_key_press_event(GtkWidget * widget,
			    GdkEventKey * event, gpointer user_data);

gboolean
 
 
 
 on_hscale1_expose_event(GtkWidget * widget,
			 GdkEventExpose * event, gpointer user_data);

void
                on_hscale1_state_changed(GtkWidget * widget,
					 GtkStateType state,
					 gpointer user_data);

gboolean
 
 
 
 on_hscale1_event(GtkWidget * widget,
		  GdkEvent * event, gpointer user_data);

void
                on_hscale1_realize(GtkWidget * widget, gpointer user_data);

gboolean
 
 
 
 on_clock1_event(GtkWidget * widget, GdkEvent * event, gpointer user_data);

void
                on_nap1_beat(GtkWidget * widget, gpointer user_data);


void
                on_daq_draw_beat(GtkWidget * widget, gpointer user_data);


void
                on_daq_draw_realize(GtkWidget * widget,
				    gpointer user_data);

void
                on_NAP1_realize(GtkWidget * widget, gpointer user_data);

void
                on_daq_draw_realize(GtkWidget * widget,
				    gpointer user_data);

void
                on_NAP1_realize(GtkWidget * widget, gpointer user_data);

void
                on_draw_beat(GtkWidget * widget, gpointer user_data);

gboolean
 
 
 
 on_draw_configure_event(GtkWidget * widget,
			 GdkEventConfigure * event, gpointer user_data);

gboolean
 
 
 
 on_draw_expose_event(GtkWidget * widget,
		      GdkEventExpose * event, gpointer user_data);

gboolean
 
 
 
 on_draw_button_press_event(GtkWidget * widget,
			    GdkEventButton * event, gpointer user_data);

void
                on_draw_realize(GtkWidget * widget, gpointer user_data);

gboolean
 
 
 
 on_draw_motion_notify_event(GtkWidget * widget,
			     GdkEventMotion * event, gpointer user_data);

void
                on_NAP1_realize(GtkWidget * widget, gpointer user_data);

gboolean
 
 
 
 on_fg_com_event(GtkWidget * widget, GdkEvent * event, gpointer user_data);

void
                on_fg_com_ent_changed(GtkEditable * editable,
				      gpointer user_data);

void
                on_fg_com_realize(GtkWidget * widget, gpointer user_data);

double          fg_sine();
double          fg_tangent();
double          fg_arc_sine();
double          fg_random();
unsigned int    fg_d_inc();
unsigned int    fg_d_scan();
unsigned int    fg_d_random();

void
                on_fg_a_beat(GtkWidget * widget, gpointer user_data);

void
                on_man_btn_realize(GtkWidget * widget, gpointer user_data);

void
                on_man_btn_realize(GtkWidget * widget, gpointer user_data);

void
                on_man_btn_pressed(GtkButton * button, gpointer user_data);

void
                on_man_ent_realize(GtkWidget * widget, gpointer user_data);

gboolean
 
 
 
 on_man_ent_key_press_event(GtkWidget * widget,
			    GdkEventKey * event, gpointer user_data);

void
                on_man_ent2_realize(GtkWidget * widget,
				    gpointer user_data);

void
                on_daq_tbtn_toggled(GtkToggleButton * togglebutton,
				    gpointer user_data);

void
                on_draw_tbtn_toggled(GtkToggleButton * togglebutton,
				     gpointer user_data);

void
                on_ip_ent_realize(GtkWidget * widget, gpointer user_data);

void
                on_port_ent_realize(GtkWidget * widget,
				    gpointer user_data);

void
                on_cnt_btn_toggled(GtkToggleButton * togglebutton,
				   gpointer user_data);

void
                on_cnt_lemp_realize(GtkWidget * widget,
				    gpointer user_data);

void
                on_fg_tbtn2_toggled(GtkToggleButton * togglebutton,
				    gpointer user_data);

void
                on_fg_com2_realize(GtkWidget * widget, gpointer user_data);

void
                on_fg_com2_ent_changed(GtkEditable * editable,
				       gpointer user_data);

void
                on_man_ent3_realize(GtkWidget * widget,
				    gpointer user_data);

void
                on_di1_realize(GtkWidget * widget, gpointer user_data);

void
                on_di2_realize(GtkWidget * widget, gpointer user_data);

void
                on_di3_realize(GtkWidget * widget, gpointer user_data);

void
                on_di4_realize(GtkWidget * widget, gpointer user_data);

void
                on_di_tbtn_toggled(GtkToggleButton * togglebutton,
				   gpointer user_data);

void
                on_do4_toggled(GtkToggleButton * togglebutton,
			       gpointer user_data);

void
                on_do4_realize(GtkWidget * widget, gpointer user_data);

void
                on_do3_toggled(GtkToggleButton * togglebutton,
			       gpointer user_data);

void
                on_do3_realize(GtkWidget * widget, gpointer user_data);

void
                on_do_tbtn_toggled(GtkToggleButton * togglebutton,
				   gpointer user_data);

void
                on_do2_toggled(GtkToggleButton * togglebutton,
			       gpointer user_data);

void
                on_do2_realize(GtkWidget * widget, gpointer user_data);

void
                on_do1_toggled(GtkToggleButton * togglebutton,
			       gpointer user_data);

void
                on_do1_realize(GtkWidget * widget, gpointer user_data);

void
                on_di_tbtn_realize(GtkWidget * widget, gpointer user_data);

void
                on_do_tbtn_realize(GtkWidget * widget, gpointer user_data);


void
                do_set(unsigned char d);

void
                on_fg_d_ent_realize(GtkWidget * widget,
				    gpointer user_data);

void
                on_fg_a_ent_realize(GtkWidget * widget,
				    gpointer user_data);

void
                on_draw_ent_realize(GtkWidget * widget,
				    gpointer user_data);

void
                on_di_ent_realize(GtkWidget * widget, gpointer user_data);

char           *trim_space(char *str);

void
                on_fg_a_ent_changed(GtkEditable * editable,
				    gpointer user_data);

void
                on_fg_d_ent_changed(GtkEditable * editable,
				    gpointer user_data);

void
                on_hs_ent_realize(GtkWidget * widget, gpointer user_data);

void
                on_hs_ent_changed(GtkEditable * editable,
				  gpointer user_data);

void
                on_draw_ent_changed(GtkEditable * editable,
				    gpointer user_data);

void
                on_di_ent_changed(GtkEditable * editable,
				  gpointer user_data);

void
                on_fg_tbtn_realize(GtkWidget * widget, gpointer user_data);

void
                on_fg_tbtn2_realize(GtkWidget * widget,
				    gpointer user_data);

void
                on_draw_tbtn_realize(GtkWidget * widget,
				     gpointer user_data);

void
                on_do_ent_realize(GtkWidget * widget, gpointer user_data);

void
                on_do_ent_changed(GtkEditable * editable,
				  gpointer user_data);

void
                on_beat_sb_realize(GtkWidget * widget, gpointer user_data);

void
                on_beat_sb_changed(GtkEditable * editable,
				   gpointer user_data);

void
                on_beat_ent_realize(GtkWidget * widget,
				    gpointer user_data);











void
                on_lemp_inv_realize(GtkWidget * widget,
				    gpointer user_data);

void
                on_lemp_inv_toggled(GtkToggleButton * togglebutton,
				    gpointer user_data);

void
                on_beat_tbtn_realize(GtkWidget * widget,
				     gpointer user_data);

void
                on_beat_tbtn_toggled(GtkToggleButton * togglebutton,
				     gpointer user_data);

void
                on_beat_lemp_realize(GtkWidget * widget,
				     gpointer user_data);

void
                on_draw_ent2_realize(GtkWidget * widget,
				     gpointer user_data);

void
                on_draw_ent3_realize(GtkWidget * widget,
				     gpointer user_data);
