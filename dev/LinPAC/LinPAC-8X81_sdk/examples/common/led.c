/* Open port, then Set LED display on/off.  

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

    read_sn.c   
    
    v 0.0.0  1 January 2009 by Cindy Huang
   
*/
   
#include <stdio.h>
#include <stdlib.h>
#include "msw.h"

int main(int argc, char **argv)
{
	int i, wRetVal;
	
	wRetVal = Open_Slot(0);
	
	if (wRetVal > 0) {
		printf("open Slot failed!\n");
		return (-1);
	}
	
	if (argc != 2) {
		printf("function : led\n");
		printf("Set LED display ON/OFF \n");
		printf("Usage: led value\n");
		printf("Example :led 0 \n");
		printf("Set LED display OFF\n");
		return FAILURE;
	}
		
	SetLED(atoi(argv[1]));  //1-->on ; 0-->off

  Close_Slot(0);
	return 0;
}
