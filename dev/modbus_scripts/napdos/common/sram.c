/* Creates a socket and connects it to the specified remote host. 

   Author: Moki Matsushima  (v 0.0.0  1 Sep 2004)

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
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */
/*
 *  System Status :
 *  bit 0~3	slot type
 *  bit 4~7	rotary switch value
 *  bit 8~11	SRAM type/status
 *  bit 12~15	reserve
 *  bit 16~31	slot status
 *
 */


#include<stdio.h>
#include<stdlib.h>
#include "msw.h"

int main()
{
	unsigned int i, wRetVal,address;
	
	wRetVal = Open_Slot(0);
	
	if (wRetVal > 0) {
		printf("open Slot failed!\n");
		return (-1);
	}
	
	wRetVal=getSystemStatus();
	wRetVal=(wRetVal>>8)&0x3;
	
	switch(wRetVal){
		case 0:
			printf("S256 with battery ok\n");
			address = 0x40000;
			break;
		case 1:
			printf("S256 with low battery\n");
			address = 0x40000;
			break;
		case 2:
			printf("S512 with battery ok\n");
			address = 0x80000;
			break;
		case 3:
			printf("S512 with low battery\n");
			address = 0x80000;
			break;
		default:
			break;
	}
	//address /=0x1000;
	printf("Press Enter to start SRAM access test\n");
	getchar();

	for(i=0x0;i<address;i++){
		Write_SRAM(i,(i+2)&0xff);
		if (i % 16 == 0)	printf("\nSave %02x ",i);
		else if (i % 8 ==0 ) printf(" ");
		printf("%02x ",(i+2)&0xff);
	}
	
	for(i=0x0;i<address;i++){
		if (i % 16 == 0)	printf("\nRead %02x ",i);
		else if (i % 8 ==0 ) printf(" ");
		printf("%02x ",	Read_SRAM(i));
	}

	printf("\n");
	Close_Slot(0);
	return 0;
}
