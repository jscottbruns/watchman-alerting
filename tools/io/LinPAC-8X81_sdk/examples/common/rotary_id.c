/* Open slot, then get rotary ID position. 

   Author: Moki Matsushima  

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
    rotary_id.c   
    v 0.0.0  1 January 2009 by Cindy Huang
   
*/
   
#include<stdio.h>
#include<stdlib.h>
#include "msw.h"


void	usage(void);
int main(int argc, char **argv)
{
	int wRetVal, slot;
  
  if (argc == 2) {
	    slot=atoi(argv[1]);
	}
	else{ 
	    usage();
	    return FAILURE;
	}
	
	    	      
  switch (slot){
	   case 0:
        wRetVal = Open_Slot(8);
	      if (wRetVal > 0) {
		        printf("open Slot failed!\n");
		        return (-1);
	      }
	      printf("GetRotaryID =%d \n",GetRotaryID(8)); 
	      Close_Slot(8);
	      break;
     case 8:
        wRetVal = Open_Slot(0);
	      if (wRetVal > 0) {
		        printf("open Slot failed!\n");
		        return (-1);
	      }
	     	printf("GetRotaryID =%d \n",GetRotaryID(0));
        Close_Slot(0); 
	      break;
	   default:  
	      wRetVal = Open_Slot(0);
	      if (wRetVal > 0) {
		        printf("open Slot failed!\n");
		        return (-1);
	      }
	      printf("GetRotaryID =%d \n",GetRotaryID(0));
        Close_Slot(0); 
			  break;  
  }
  
	return 0;
}

void usage(void)
{
    printf("function : rotary_id\n");
		printf("Get rotary ID position \n");
		printf("Usage: rotary_id slot\n");
		printf("Example: rotary_id 8\n");
		printf("Open slot to get rotary ID position\n");
}
