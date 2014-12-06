#include <stdio.h>
#include <stdlib.h>
#include "gtk/gtk.h"
#include "msw.h"

//static char szSend[80], szReceive[80];
static GtkWidget *window;

static void model_name(void)
{	  
   gint wsRetVal,i;
   gchar szSend[80], szReceive[80];
   gint wRetVal;
   gchar buf[20];
   WORD wT;
                                                                     
   szSend[0] = '$';
   szSend[1] = '0';
   szSend[2] = '0';
   szSend[3] = 'M';
   szSend[4] = 0;
                                                                                                                                                               
   wRetVal = Open_Slot(0);
   if (wRetVal > 0) {
       printf("open Slot failed!\n");
   }
                                                                                                       
   wsRetVal = Open_Com(COM1, 115200, Data8Bit, NonParity, OneStopBit);
   for (i=1;i<8;i++) {                                                                                                               
       ChangeToSlot(i);
       wRetVal = Open_Slot(i);
       if (wRetVal > 0)
            g_print("slot %d ... busy\n",i);
       else if (GetNameOfModule(i)==8000) {
            if (wsRetVal > 0 ) g_print("slot %d ... busy\n",i);
            else {
                    wRetVal = Send_Receive_Cmd(COM1, szSend, szReceive, 1, 0, &wT);
                       
                    g_print("wRetVal %d \n",wRetVal);
                    if (wRetVal > 0) {
                          // g_print("slot %d ... not installed\n",i);
                          //printf("slot %d a... %s\n",i,&szReceive[3]); 
                          g_print("slot %d a... %d\n",i,GetNameOfModule(i));
                    }                             
                    else {
                          //g_print("szReceive %d b... %s\n", i, &szReceive[3]);
                          //g_snprintf(buf,10, "I-%d",&szReceive[3]);
                         	printf("slot %d b ... %s\n",i,&szReceive[3]); 
                          //g_print("slot %d b... %d\n",i,GetNameOfModule(i));
                    }                                                                                                 
           }
       }
       else
           g_print("slot %d c... %d\n",i,GetNameOfModule(i));
       Close_Slot(i);
         
  }
   		
	//Close_Com(COM1);
	//Close_Slot(0);
}

int main( int   argc, char *argv[])
{
	GtkWidget *main_vbox,*label;
	GtkWidget *frame;
	

	gtk_init (&argc, &argv);
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_signal_connect(GTK_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
	gtk_widget_set_size_request (window, 300, 200);
	gtk_widget_show(window);

	main_vbox = gtk_hbox_new (FALSE, 10);
	gtk_container_set_border_width (GTK_CONTAINER (main_vbox), 10);
	gtk_container_add (GTK_CONTAINER (window), main_vbox);
	gtk_widget_show(main_vbox);
	
	label=gtk_label_new("123");
	
	 gtk_box_pack_start(GTK_BOX(main_vbox), label, TRUE,TRUE, 10);

	gtk_widget_show(label);
    
	model_name();                                                           

    	gtk_main ();
	Close_Com(COM1);
	Close_Slot(0);
	return 0;


}
