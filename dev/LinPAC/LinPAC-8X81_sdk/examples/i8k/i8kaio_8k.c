/* Test with I-8024 AO Module and I-8017H AI Module at Slot 00 and 01 of I-8000 Controller separately

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

   i8kaio_8k.c
 
   v1.0 2004.9.1 by Edward  
 
*/

#include<stdio.h>
#include<stdlib.h>
#include "msw.h"

char szSend[80], szReceive[80];
DWORD wBuf[12];
float fBuf[12];

int main()
{
	int i,j, wRetVal;
	DWORD temp;

	wRetVal = Open_Com(COM2, 115200, Data8Bit, NonParity, OneStopBit);
	if (wRetVal > 0) {
		printf("open port failed!\n");
		return (-1);
	}

	//--- Analog output ----  ****   8024 -- AO  ****
	i = 0;
	wBuf[0] = 2;				  // COM Port
	wBuf[1] = 01;			  	// Address
	wBuf[2] = 0x8024;			// ID
	wBuf[3] = 0;				  // CheckSum disable
	wBuf[4] = 100;				// TimeOut , 100 msecond
	wBuf[5] = i;          // Channel No. of AO
	wBuf[6] = 0;				  // string debug
	wBuf[7] = 0;				  // Slot Number
	fBuf[0] = 3.5;


			wRetVal = AnalogOut_8K(wBuf, fBuf, szSend, szReceive);
		if (wRetVal)
			printf("AO of 8024 Error !, Error Code=%d\n", wRetVal);
		else
		  printf("AO of 8024 channel %d = %f \n",i,fBuf[0]);

 
	//--- Analog Input ----  ****   8017H -- AI  ****
	j = 2;
	wBuf[0] = 2;				  // COM Port
	wBuf[1] = 01;			  	// Address
	wBuf[2] = 0x8017;			// ID
	wBuf[3] = 0;				  // CheckSum disable
	wBuf[4] = 100;				// TimeOut , 100 msecond
	wBuf[5] = j;          // Channel of AI
	wBuf[6] = 0;				  // string debug
	wBuf[7] = 1;				  // Slot Number
	
			wRetVal = AnalogIn_8K(wBuf, fBuf, szSend, szReceive);
		if (wRetVal)
			printf("AI of 8017H Error !, Error Code=%d\n", wRetVal);
	  else
		  printf("AI of 8017H channel %d = %f \n",j,fBuf[0]);

	Close_Com(COM2);
	
	return 0;
}
