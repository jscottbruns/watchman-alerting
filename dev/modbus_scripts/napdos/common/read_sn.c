/* Open port, then get serial number from DS2502. 

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
    
    v 0.0.0  1 Sep 2004  by Moki Matsushima
   
*/
   
#include<stdio.h>
#include<stdlib.h>
#include "msw.h"

int main()
{
	int i, wRetVal;
	DWORD temp;
	unsigned char serial_num[8];
	wRetVal = Open_Slot(0);
	
	if (wRetVal > 0) {
		printf("open Slot failed!\n");
		return (-1);
	}
	
	Read_SN(serial_num);

	printf("serial number = %x%x%x%x%x%x%x%x\n",serial_num[0],serial_num[1],serial_num[2],serial_num[3],serial_num[4],serial_num[5],serial_num[6],serial_num[7]);
	Close_Slot(0);
	return 0;
}
