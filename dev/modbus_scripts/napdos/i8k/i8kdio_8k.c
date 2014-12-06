/* Test with I-8055 DIO Module at Slot 00 of I-8000 Controller

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

   i8kdio_8k.c
 
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
	int i, wRetVal;
	DWORD temp;

	//Check Open_Com2
	wRetVal = Open_Com(COM2, 115200, Data8Bit, NonParity, OneStopBit);
	if (wRetVal > 0) {
		printf("open port failed!\n");
		return (-1);
	}
		
	//--- digital output ----  **(DigitalOut_8K()**)
	dwBuf[0] = 2;				   // COM Port
	dwBuf[1] = 01;				 // Address
	dwBuf[2] = 0x8055;		 // ID
	dwBuf[3] = 0;				   // CheckSum disable
	dwBuf[4] = 100;				 // TimeOut , 100 msecond
	dwBuf[5] = 0xff;			 // digital output
	dwBuf[6] = 0;				   // string debug
	dwBuf[7] = 0;          // slot number

  wRetVal = DigitalOut_8K(dwBuf, fBuf, szSend, szReceive);  // DO Output
  if (wRetVal)
			printf("DO of 8055 Error !, Error Code=%d\n", wRetVal);
	else	
		  printf("DO of 8055 = 0x%x" ,dwBuf[5]);
  

  //--- digital Input ----   **(DigitalIn_8K()**)
	dwBuf[0] = 2;				   // COM Port
	dwBuf[1] = 01;				 // Address
	dwBuf[2] = 0x8055;		 // ID
	dwBuf[3] = 0;				   // CheckSum disable
	dwBuf[4] = 100;			   // TimeOut , 100 msecond
	dwBuf[6] = 0;				   // string debug
	dwBuf[7] = 0;          // slot number
	
	getch();
	DigitalIn_8K(dwBuf, fBuf, szSend, szReceive);   // DI Input
  printf("DI= %u",dwBuf[5]);
  
 
  //--- digital output ----   ** Close DO **
	dwBuf[0] = 2;				   // COM Port
	dwBuf[1] = 01;				 // Address
	dwBuf[2] = 0x8055;		 // ID
	dwBuf[3] = 0;				   // CheckSum disable
	dwBuf[4] = 100;				 // TimeOut , 100 msecond
	dwBuf[5] = 0x00;			 // digital output
	dwBuf[6] = 0;			  	 // string debug
	dwBuf[7] = 0;          // slot number
	
	getch();               // push any key to continue
  wRetVal = DigitalOut_8K(dwBuf, fBuf, szSend, szReceive);
  
	Close_Com(COM2);
	return 0;
}
