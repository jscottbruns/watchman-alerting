/* Test with I-8055 DIO Module at Slot 03 of LinCon

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
   
   i8kdio.c
 
   v1.0 2004.9.1 by Edward  
*/


#include<stdio.h>
#include<stdlib.h>
#include "msw.h"

char szSend[80], szReceive[80];
DWORD dwBuf[12];
float fBuf[12];

int main()
{
	int i,j, wRetVal;
	WORD DOval,temp;	
  
	wRetVal = Open_Slot(3);	
	if (wRetVal > 0) {
		printf("open Slot failed!\n");
		return (-1);
	}
	
	//I-8055_DO
	DO_8(3,255);
	printf("DO of I-8055 = %x",255);	
	
	//I-8055_DI
	printf("DI of I-8055 = %x",DI_8(3));	
	
	Close_Slot(3);
	return 0;
}
