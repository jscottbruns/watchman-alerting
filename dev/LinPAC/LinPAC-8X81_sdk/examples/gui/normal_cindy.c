#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "msw.h"
//#include <sys/utsname.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <strings.h>
//#include <sys/ioctl.h>
//#include <linux/if.h>

static GtkWidget *window;
static gchar szSend[80], szReceive[80];
static	gint wsRetVal;
static	gint wRetVal;
static gint i=1;

gboolean model_name(gpointer label)
//gboolean model_name(void)
{
	gchar buf[10];
	WORD wT;

	szSend[0] = '$';
	szSend[1] = '0';
	szSend[2] = '0';
	szSend[3] = 'M';
	szSend[4] = 0;

	wsRetVal = Open_Com(COM1, 115200, Data8Bit, NonParity, OneStopBit);
	ChangeToSlot(i);
	wRetVal = Open_Slot(i);
  //g_print("slot c+ %d ... %d\n",i,GetNameOfModule(i));
  
	if (wRetVal > 0){
	    g_print("wRetVal %d \n",wRetVal);
			g_print("slot %d ... busy\n",i);
  }		
	else if (GetNameOfModule(i)==8000) {
			 if (wsRetVal > 0 )
				  g_snprintf(buf,10, "%s","Busy");
			 else {
				     wRetVal = Send_Receive_Cmd(COM1, szSend, szReceive, 1, 0, &wT);
				     if (wRetVal > 0){
				         //g_print("szReceive[1] %d ...%s\n",i,&szReceive[1]);
                 g_print("slot a %d ... %d\n",i,GetNameOfModule(i));
				         g_snprintf(buf,20,"slot %d ... %d\n",i,GetNameOfModule(i));
				         //g_snprintf(buf,10, "I-%d",&szReceive[3]);			   
        	      //g_snprintf(buf,10, "%s","N/A");
				     }
				     else{
			   	        g_print("slot b  %d ...%s\n",i,&szReceive[1]);
                 // g_print("slot b %d ... %d\n",i,GetNameOfModule(i));		   	 
				          g_snprintf(buf,10, "I-%d",&szReceive[3]);
			       }
		  }
	}
	else{
	    // g_print("szReceive[1] %d ...%s\n",i,&szReceive[1]);
		   g_print("slot c %d ... %d\n",i,GetNameOfModule(i));		   
		   g_snprintf(buf,10, "I-%d",GetNameOfModule(i));
	}
	
  //g_print("slot d %d ... %d\n",i,GetNameOfModule(i));   
  Close_Slot(i);
		
	
	//g_snprintf(buf,10, "%d",i);
  gtk_label_set_text(label,buf);
  //    label=gtk_label_new("");
	//gtk_label_set_text(GTK_LABEL(label),buf);
	i++;
	memset( buf, 0, strlen(buf) );                // fill with zero.
return FALSE;
}

int main( int   argc, char *argv[])
{
	GtkWidget *main_hbox;
	GtkWidget *tab_hbox;
	GtkWidget *notebook;
	GtkWidget *hbox;
	GtkWidget *vbox;
	GtkWidget *vbox1;
	GtkWidget *tabel;
	GtkWidget *label;
	GtkWidget *frame;
	gint j;
	//gchar buf[10];

	wRetVal = Open_Slot(0);
  if (wRetVal > 0) {
       g_print("open Slot failed!\n");
  }

	gtk_init (&argc, &argv);
	window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_signal_connect(GTK_OBJECT(window),"destroy",G_CALLBACK(gtk_main_quit),NULL);
	gtk_widget_set_size_request (window, 350, 250);
	gtk_container_set_border_width (GTK_CONTAINER (window), 10);

	main_hbox = gtk_hbox_new (FALSE, 10);
	gtk_container_set_border_width (GTK_CONTAINER (main_hbox), 10);

	gtk_container_add (GTK_CONTAINER (window), main_hbox);
	vbox1 = gtk_vbox_new (TRUE, 0);
//	label=gtk_label_new("");
	   	
/**/for(j=1; j<8; j++){
//		frame = gtk_frame_new (NULL);
//    gtk_frame_set_shadow_type(GTK_FRAME(frame),GTK_SHADOW_IN);
          
	   	label=gtk_label_new("");
	   	//gtk_label_set_text(GTK_LABEL(label),buf);
	   	
      g_timeout_add(200,model_name,label);    
// 		gtk_container_add (GTK_CONTAINER (frame), label);
	  	gtk_box_pack_start(GTK_BOX(vbox1), label, TRUE, TRUE, 10);
	    
      gtk_widget_show (label);
//  	gtk_widget_show (frame);
	    gtk_widget_show (vbox1);      
   }
   
	gtk_box_pack_start(GTK_BOX(main_hbox), vbox1, TRUE, TRUE, 10);

	gtk_widget_show (main_hbox);
	gtk_widget_show (window);
	
//	g_timeout_add(1000, model_name, NULL);   
	
  gtk_main ();
	Close_Com(COM1);
	Close_Slot(0);

	return 0;

}
