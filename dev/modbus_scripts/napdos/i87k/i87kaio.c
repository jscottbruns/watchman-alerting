/* Test with I-87022 AO Module and I-87017 AI Module at Slot 02 and 03 of LinCon seperately.

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

   i87kaio.c
 
   v1.0 2004.9.1 by Edward  
 
 */

#include<stdio.h>
#include<stdlib.h>
#include "msw.h"

char szSend[80], szReceive[80];
DWORD wBuf[12];
DWORD  wBuf7[12];
float fBuf[12];

int main()
{
	int i,j, wRetVal;
	DWORD temp;
  
  wRetVal = Open_Slot(0);
	if (wRetVal > 0) {
		printf("open Slot failed!\n");
		return (-1);
	}
		
	wRetVal = Open_Com(COM1, 115200, Data8Bit, NonParity, OneStopBit);
	if (wRetVal > 0) {
		printf("open port failed!\n");
		return (-1);
	}	
	
	ChangeToSlot(2);
	//--- Analog output ----  ****   87022 -- AO  ****
	i=0;
	wBuf[0] = 1;			  	// COM Port
	wBuf[1] = 00;		      // Address
	wBuf[2] = 0x87022;		// ID
	wBuf[3] = 0;				  // CheckSum disable
	wBuf[4] = 100;				// TimeOut , 100 msecond
	wBuf[5] = i;          // Channel Number of AO
	wBuf[6] = 0;				  // string debug
	fBuf[0] = 2.5;        // AO Value
  
			wRetVal = AnalogOut_87K(wBuf, fBuf, szSend, szReceive);
		if (wRetVal)
			printf("AO of 87022 Error !, Error Code=%d\n", wRetVal);
		else	
		  printf("AO of 87022 channel %d = %f \n",i,fBuf[0]);

	ChangeToSlot(3);	
	//--- Analog Input ----  ****   87017 -- AI  ****
	j=1;
	wBuf7[0] = 1;				  // COM Port
	wBuf7[1] = 00;	  		// Address
	wBuf7[2] = 0x87017;		// ID
	wBuf7[3] = 0;				  // CheckSum disable
	wBuf7[4] = 100;				// TimeOut , 100 msecond
	wBuf7[5] = j;         //Channel Number of AI
	wBuf7[6] = 0;				  // string debug
				
			wRetVal = AnalogIn_87K(wBuf7, fBuf, szSend, szReceive);
		if (wRetVal)
			printf("AI of 87017 Error !, Error Code=%d\n", wRetVal);
		else	
	    printf("AI of 87017 channel %d = %f \n",j,fBuf[0]);
	
	Close_Com(COM1);
	Close_SlotAll();
	
	return 0;
}
