/* Test with I-7021 AO Module and I-7017 AI Module at Address 05 and 03 of 485 network seperately. 
    
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
   
   i7kaio.c
 
   v1.0 2004.9.1 by Edward  
  
 */

#include<stdio.h>
#include<stdlib.h>
#include "msw.h"

char szSend[80], szReceive[80];
WORD wBuf[12];
float fBuf[12];

int main()
{
	int i,j, wRetVal;
	DWORD temp;

	wRetVal = Open_Com(COM3, 9600, Data8Bit, NonParity, OneStopBit);
	if (wRetVal > 0) {
		printf("open port failed!\n");
		return (-1);
	}

	//--- Analog output ----  ****   7021 -- AO  ****
	i = 0;
	wBuf[0] = 3;				// COM Port
	wBuf[1] = 05;		  	// Address
	wBuf[2] = 0x7021;		// ID
	wBuf[3] = 0;				// CheckSum disable
	wBuf[4] = 100;			// TimeOut , 100 msecond
	//wBuf[5] = i;      // Not used if module ID is 7016/7021
                      // Channel No.(0 to 1) if module ID is 7022
                      // Channel No.(0 to 3) if module ID is 7024
	wBuf[6] = 0;				// string debug
	fBuf[0] = 3.5;      // Analog Value

			wRetVal = AnalogOut(wBuf, fBuf, szSend, szReceive);
		if (wRetVal)
			printf("AO of 7021 Error !, Error Code=%d\n", wRetVal);
		else
		  printf("AO of 7021 channel %d = %f \n",i,fBuf[0]);
  
	//--- Analog Input ----  ****   7017 -- AI  ****
	j = 1;
	wBuf[0] = 3;				// COM Port
	wBuf[1] = 03;			  // Address
	wBuf[2] = 0x7017;		// ID
	wBuf[3] = 0;				// CheckSum disable
	wBuf[4] = 100;			// TimeOut , 100 msecond
	wBuf[5] = j;        // Channel of AI
	wBuf[6] = 0;				// string debug	

			wRetVal = AnalogIn(wBuf, fBuf, szSend, szReceive);
		if (wRetVal)
			printf("AI of 7017 Error !, Error Code=%d\n", wRetVal);
	  else
		  printf("AI of 7017 channel %d = %f \n",j,fBuf[0]);

	Close_Com(COM3);
	
	return 0;
}
