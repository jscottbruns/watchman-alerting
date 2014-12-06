/* Module DI/DO test of GUI(graphical user interface) environment.
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. 
   
   File level history (record changes for this file here.)
   
   v 0.0 1 Sep 2004 by Moki Matsushima
   
*/

#include <gtk/gtk.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "msw.h"
struct block
{
	gboolean   opened; 
	GtkWidget *button;
};

/**
 * 
 */
static struct block *map; 
static gint width=8;
static gint height=4;
static GtkWidget *bTemp[20];
static gint button_size=46;   /* button size */
static GtkWidget *window;

/**
 * g_timeout_add() timer callback function
 */
gboolean tick(gpointer data){
	gint j,DIVal1,DIVal2;
	DIVal1 = DI_8(1);
	DIVal2 = DI_8(2);
	for(j=width;j<width*2;j++) {
		if(DIVal1 % 2) {
			map[j].opened=TRUE;
			gtk_button_set_label(GTK_BUTTON(map[j].button), "ON");
		}
		else {
			gtk_button_set_label(GTK_BUTTON(map[j].button), "OFF");
			map[j].opened=FALSE;
		}
		DIVal1=DIVal1/2;
	}
	
	for(j=width*3;j<width*4;j++) {
		if(DIVal2 % 2) {
			map[j].opened=TRUE;
			gtk_button_set_label(GTK_BUTTON(map[j].button), "ON");
		}
		else {
			gtk_button_set_label(GTK_BUTTON(map[j].button), "OFF");
			map[j].opened=FALSE;
		}
		DIVal2=DIVal2/2;
	}
	return TRUE; /* reserve for user */
}


/**
 * mouse click event callback function
 */
gboolean on_mouse_click(GtkWidget *widget,GdkEventButton *event,gpointer data)
{
	gint index;
	gint row, col, j, DOVal1, DOVal2;
	gchar buf[4];

	index=(gint)data;
	row=index/width;
	col=index%width;
	
	if (((index>3)&&(index<16))||(index>23)) return TRUE;
	
	if(map[index].opened==TRUE) {
		map[index].opened=FALSE;
		gtk_button_set_label(GTK_BUTTON(widget), "OFF");
	}
	else{
		map[index].opened=TRUE;
		gtk_button_set_label(GTK_BUTTON(widget), "ON");
	}
	DOVal1 = 0 ;
	for(j=0;j<width;j++)
		if(map[j].opened==TRUE) DOVal1+=(gint)pow(2,j);
	DOVal2=0;	
	for(j=16;j<(width+16);j++)
		if(map[j].opened==TRUE) DOVal2+=(gint)pow(2,(j-16));

	DIO_DO_8(1,DOVal1);
	DIO_DO_8(2,DOVal2);

    return TRUE;
}

/**
 * main program
 */
int main(int argc, char **argv)
{
	GtkWidget *vbox;
	GtkWidget *hbox;
	GtkWidget *label;
	gint i, j, index, RetVal;
	gchar buf[4];
	
	RetVal = Open_Slot(1);
	if (RetVal > 0) {
		printf("open Slot failed!\n");
		return (-1);
	}

	RetVal = Open_Slot(2);
	if (RetVal > 0) {
		printf("open Slot failed!\n");
		return (-1);
	}
	
	/* initialize GTK library */
	gtk_init(&argc, &argv);

	/* alloc map memory to save I/O status */
	map=(struct block *)g_malloc0(sizeof(struct block)* width*height);
			     
	/* build the user interface */
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(window), "delete_event",gtk_main_quit, NULL);
		
	vbox=gtk_vbox_new(FALSE, 0);
	hbox=gtk_hbox_new(FALSE, 0);
	label=gtk_label_new("ICPDAD LinCon Demo");
	gtk_box_pack_start(GTK_BOX(hbox), label,	FALSE, FALSE, 4);
	gtk_widget_show_all(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox,	FALSE, FALSE, 0);
		
	index = 0;
	
	hbox=gtk_hbox_new(TRUE, 0);
	label=gtk_label_new("Slot 1 i-8063");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 4);
	gtk_widget_show_all(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox,	FALSE, FALSE, 0);

	hbox=gtk_hbox_new(TRUE, 0);
	label=gtk_label_new("Bit");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 4);
	gtk_widget_show_all(hbox);
	
	for(j=0; j<width; j++){
		g_snprintf(buf,4,"%d",j);
		label=gtk_label_new("");
		gtk_label_set_text(GTK_LABEL(label),buf);
		gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 4);
	}
	
	gtk_widget_show_all(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);
	hbox=gtk_hbox_new(TRUE, 0);
	label=gtk_label_new("DO ");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 4);
	gtk_widget_show_all(hbox);
	
	for(j=0; j<width; j++){
		GtkWidget *button;
		bTemp[j]=button;
		button=gtk_toggle_button_new();
		gtk_widget_set_usize(button, button_size, button_size);
		g_object_set(G_OBJECT(button), "can-focus", FALSE, NULL);
		gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
		gtk_widget_show(button);
		g_signal_connect(G_OBJECT(button), "button-press-event",G_CALLBACK(on_mouse_click), (gpointer)index);
		gtk_button_set_label(GTK_BUTTON(button), "OFF");
		map[index].button=button;
		index++;
	}
	
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	hbox=gtk_hbox_new(TRUE, 0);
	label=gtk_label_new("DI ");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 4);
	gtk_widget_show_all(hbox);
	
	for(j=0; j<width; j++){
		GtkWidget *button;
		bTemp[j+width]=button;
		button=gtk_toggle_button_new();
		gtk_widget_set_usize(button, button_size, button_size);
		g_object_set(G_OBJECT(button), "can-focus", FALSE, NULL);
		gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
		gtk_widget_show(button);
		g_signal_connect(G_OBJECT(button), "button-press-event",G_CALLBACK(on_mouse_click), (gpointer)index);
		gtk_button_set_label(GTK_BUTTON(button), "OFF");
		map[index].button=button;
		index++;
	}
	
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);
	hbox=gtk_hbox_new(TRUE, 0);
	label=gtk_label_new("");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 4);
	gtk_widget_show_all(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	hbox=gtk_hbox_new(TRUE, 0);
	label=gtk_label_new("Slot 1 i-8054");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 4);
	gtk_widget_show_all(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox,	FALSE, FALSE, 0);

	hbox=gtk_hbox_new(TRUE, 0);
	label=gtk_label_new("Bit");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 4);
	gtk_widget_show_all(hbox);
	
	for(j=0; j<width; j++){
		g_snprintf(buf,4,"%d",j);
		label=gtk_label_new("");
		gtk_label_set_text(GTK_LABEL(label),buf);
		gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 4);
	}
	
	gtk_widget_show_all(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);
	hbox=gtk_hbox_new(TRUE, 0);
	label=gtk_label_new("DO ");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 4);
	gtk_widget_show_all(hbox);
	
	for(j=0; j<width; j++){
		GtkWidget *button;
		button=gtk_toggle_button_new();
		gtk_widget_set_usize(button, button_size, button_size);
		g_object_set(G_OBJECT(button), "can-focus", FALSE, NULL);
		gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
		gtk_widget_show(button);
		g_signal_connect(G_OBJECT(button), "button-press-event",G_CALLBACK(on_mouse_click), (gpointer)index);
		gtk_button_set_label(GTK_BUTTON(button), "OFF");
		map[index].button=button;
		index++;
	}
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);


	hbox=gtk_hbox_new(TRUE, 0);
	label=gtk_label_new("DI ");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 4);
	gtk_widget_show_all(hbox);
	
	for(j=0; j<width; j++){
		GtkWidget *button;
		button=gtk_toggle_button_new();
		gtk_widget_set_usize(button, button_size, button_size);
		g_object_set(G_OBJECT(button), "can-focus", FALSE, NULL);
		gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
		gtk_widget_show(button);
		g_signal_connect(G_OBJECT(button), "button-press-event",G_CALLBACK(on_mouse_click), (gpointer)index);
		gtk_button_set_label(GTK_BUTTON(button), "OFF");
		map[index].button=button;
		index++;
	}
	
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);
	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_widget_show(vbox);
	gtk_widget_show(window);

	g_timeout_add(1000, (GSourceFunc)tick, NULL);
	
	gtk_main();

	g_free(map); /* free map memory before exit program */
	Close_Slot(1);
	Close_Slot(2);
	return 0;
}
