/* Test with I-7050 DIO Module at Address 05 of RS-485 network. 
    
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
   
   i7kdio.c
 
   v1.0 2004.9.1 by Edward  
    
 */

#include<stdio.h>
#include<stdlib.h>
#include "msw.h"

char szSend[80], szReceive[80], ans;
WORD wBuf[12];
float fBuf[12];

int main()
{
	int  wRetVal;

  // Check Open_Com3
	wRetVal = Open_Com(COM3, 115200, Data8Bit, NonParity, OneStopBit);
	if (wRetVal > 0) {
		printf("open port failed!\n");
		return (-1);
	}
	
	//  *****  7050 DO && DI Parameter *******
	wBuf[0] = 3;				  // COM Port
	wBuf[1] = 05;				// Address
	wBuf[2] = 0x7050;			// ID
	wBuf[3] = 0;				  // CheckSum disable
	wBuf[4] = 100;				// TimeOut , 100 msecond
	wBuf[5] = 0xff;       // 8 DO Channels On
	wBuf[6] = 0;				  // string debug
  	
  	// 7050 DO 
		wRetVal = DigitalOut(wBuf, fBuf, szSend, szReceive);
		if (wRetVal)
		  printf("DigitalOut_7050 Error !, Error Code=%d\n", wRetVal);
		printf("The DO of 7050 : %u \n", wBuf[5]);	
	
		// 7050 DI	
		DigitalIn(wBuf, fBuf, szSend, szReceive);
		printf("The DI of 7050 : %u \n", wBuf[5]);
		
	Close_Com(COM3);

  return 0;
}
