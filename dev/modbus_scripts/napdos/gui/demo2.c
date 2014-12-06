/* Module DI/DO test of GUI(graphical user interface) environment by auto/manual run.
     
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
static GtkWidget *image[20];
static gint button_size=46;   /* button size */
static GtkWidget *window;
static gint demo_mode;
static gint dir;
/**
 * g_timeout_add() timer callback function
 */
gboolean tick(gpointer data){
	gchar buf[8];
	gint j, DOVal1, DOVal2, DIVal;
		
	if (demo_mode == 1) {
		DOVal1=0;
		for(j=0;j<width;j++)
			if(map[j].opened==TRUE) DOVal1+=(gint)pow(2,j);

		if (dir!=0) {
			DOVal1 = DOVal1 / 2;
			if (DOVal1 <= 0) { 
				DOVal1=1;
				dir=0;
				DOVal1 = DOVal1 * 2;
			}
		}else {
			DOVal1 = DOVal1 * 2;
			if (DOVal1==0) DOVal1+=1;
			if (DOVal1 > 255) { 
				dir=1;
				DOVal1 = DOVal1 /2;
			}
		}

		DOVal2 =DOVal1;
		for(j=0;j<width;j++) {
			if(DOVal2 % 2) {
				map[j].opened=TRUE;
				gtk_image_set_from_file(GTK_IMAGE(image[j]),"/opt/share/lincon/on.xpm");
			}
			else {
				gtk_image_set_from_file(GTK_IMAGE(image[j]),"/opt/share/lincon/off.xpm");
				map[j].opened=FALSE;
			}
			DOVal2=DOVal2/2;
		}
		DIO_DO_8(1,DOVal1);
	}

	usleep(100000);
	/*get digital input from slot 2*/
	DIVal = DI_8(2);
	for(j=width;j<width*2;j++) {
		if(DIVal % 2) {
			map[j].opened=TRUE;
			gtk_image_set_from_file(GTK_IMAGE(image[j]),"/opt/share/lincon/on.xpm");
		}
		else {
			gtk_image_set_from_file(GTK_IMAGE(image[j]),"/opt/share/lincon/off.xpm");
			map[j].opened=FALSE;
		}
		DIVal=DIVal/2;
	}
	return TRUE;
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
	if (((index>7)&&(index<16))||(index>23)) return TRUE;
	
	if(map[index].opened==TRUE) {
		map[index].opened=FALSE;
		gtk_image_set_from_file(GTK_IMAGE(image[index]),"/opt/share/lincon/off.xpm");
	}
	else{
		map[index].opened=TRUE;
		gtk_image_set_from_file(GTK_IMAGE(image[index]),"/opt/share/lincon/on.xpm");
	}
	
	DOVal1 = 0 ;
	for(j=0;j<width;j++)
		if(map[j].opened==TRUE) DOVal1+=(gint)pow(2,j);
	DIO_DO_8(1,DOVal1);
    return TRUE;
}

gboolean on_mode_click(GtkWidget *widget,GdkEventButton *event,gpointer data)			
{
	gint index;
	gint row, col, j, DOVal1, DOVal2;
	gchar buf[4];

	index=(gint)data;
	if (((index>7)&&(index<16))||(index>23)) return TRUE;
	
	if(demo_mode==1) {
		demo_mode=0;
		gtk_button_set_label(GTK_BUTTON(widget), "Manual");
	}
	else{
		demo_mode=1;
		gtk_button_set_label(GTK_BUTTON(widget), "Auto");
	}
	
	DOVal1 = 0 ;
	for(j=0;j<width;j++)
		if(map[j].opened==TRUE) DOVal1+=(gint)pow(2,j);
	DIO_DO_8(1,DOVal1);
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
	GtkWidget *button;
	gint i, j, index, RetVal;
	gchar buf[4];
	
	RetVal = Open_Slot(1);
	if (RetVal > 0) {
		printf("open Slot failed!\n");
		return (-1);
	}
	
	demo_mode=1;

	RetVal = Open_Slot(2);
	if (RetVal > 0) {
		printf("open Slot failed!\n");
		return (-1);
	}
	
	/* initialize GTK library */
	gtk_init(&argc, &argv);

	/* alloc map memory to save I/O status */
	map=(struct block *)g_malloc0(sizeof(struct block)*width*height);
			      
	/* build the user interface */
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	g_signal_connect(G_OBJECT(window), "delete_event",gtk_main_quit, NULL);

	vbox=gtk_vbox_new(FALSE, 0);
	hbox=gtk_hbox_new(FALSE, 0);
	label=gtk_label_new("ICPDAD LinCon Demo");
	//label=gtk_label_new(" ");
	gtk_box_pack_start(GTK_BOX(hbox), label,FALSE, FALSE, 4);			
	gtk_widget_show_all(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox,	FALSE, FALSE, 0);
		
	index = 0;

	hbox=gtk_hbox_new(FALSE, 0);
	label=gtk_label_new("Demo Mode");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 4);
	gtk_widget_show_all(hbox);

  //add auto/manual button
	button=gtk_toggle_button_new();
	gtk_widget_set_usize(button, button_size, button_size);
	g_object_set(G_OBJECT(button), "can-focus", FALSE, NULL);
	gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
	gtk_widget_show(button);
	g_signal_connect(G_OBJECT(button), "button-press-event",G_CALLBACK(on_mode_click), (gpointer)index);
	gtk_button_set_label(GTK_BUTTON(button), "Auto");
	gtk_widget_show_all(hbox);
	gtk_box_pack_start(GTK_BOX(vbox), hbox,	FALSE, FALSE, 0);
		
	hbox=gtk_hbox_new(TRUE, 0);
	label=gtk_label_new("Digital I/O Demo");
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
	
	//add 0~7 DO button
	for(j=0; j<width; j++){
		button=gtk_toggle_button_new();
		bTemp[j]=button;
		gtk_widget_set_usize(button, button_size, button_size);
		g_object_set(G_OBJECT(button), "can-focus", FALSE, NULL);
		gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
		gtk_widget_show(button);
		g_signal_connect(G_OBJECT(button), "button-press-event",G_CALLBACK(on_mouse_click), (gpointer)index);	
		image[j] = gtk_image_new_from_file("/opt/share/lincon/off.xpm");
		gtk_widget_show(image[j]);
		gtk_container_add(GTK_CONTAINER(button),image[j]);
		map[index].button=button;
		index++;
	}
	
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
	gtk_widget_show(hbox);

	hbox=gtk_hbox_new(TRUE, 0);
	label=gtk_label_new("DI ");
	gtk_box_pack_start(GTK_BOX(hbox), label, FALSE, FALSE, 4);
	gtk_widget_show_all(hbox);

  //add 0~7 DI button
	for(j=0; j<width; j++){
		button=gtk_toggle_button_new();
		bTemp[j+width]=button;
		gtk_widget_set_usize(button, button_size, button_size);
		g_object_set(G_OBJECT(button), "can-focus", FALSE, NULL);
		gtk_box_pack_start(GTK_BOX(hbox), button, FALSE, FALSE, 0);
		gtk_widget_show(button);
		g_signal_connect(G_OBJECT(button), "button-press-event",G_CALLBACK(on_mouse_click), (gpointer)index);
		image[j+width] = gtk_image_new_from_file("/opt/share/lincon/off.xpm");
		gtk_widget_show(image[j+width]);
		gtk_container_add(GTK_CONTAINER(button),image[j+width]);
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
