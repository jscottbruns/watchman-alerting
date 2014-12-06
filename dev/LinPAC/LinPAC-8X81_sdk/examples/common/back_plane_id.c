/* Open slot, then get Back Plane ID. 

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
   
#include<stdio.h>
#include<stdlib.h>
#include "msw.h"

int main()
{
	int wRetVal;
	wRetVal = Open_Slot(9);
	
	if (wRetVal > 0) {
		printf("open Slot failed!\n");
		return (-1);
	}
	
  printf("GetBackPlaneID =%d \n",GetBackPlaneID()); 

  Close_Slot(9);
	return 0;
}
