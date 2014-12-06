/* Test with I-87054 DIO Module at Address 06 of RS-485 network.

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

   i87kdio_87k.c
 
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
	/* COM2, Baudrate=9600, 8 data bit, non-parity, 1 stop bit */

	//Check Open_Com3
	wRetVal = Open_Com(COM3, 9600, Data8Bit, NonParity, OneStopBit);
	if (wRetVal > 0) {
		printf("open port failed!\n");
		return (-1);
	}
	
	//--- digital output ----  **(DigitalOut_87K()**)
	dwBuf[0] = 3;				   // COM Port
	dwBuf[1] = 06;				 // Address
	dwBuf[2] = 0x87054;		 // ID
	dwBuf[3] = 0;				   // CheckSum disable
	dwBuf[4] = 100;				 // TimeOut , 100 msecond
	dwBuf[5] = 0xff;			 // digital output
	dwBuf[6] = 0;				   // string debug
  wRetVal = DigitalOut_87K(dwBuf, fBuf, szSend, szReceive);  // DO Output
  printf("DO Value= %u", dwBuf[5]);
  
  //--- digital Input ----   **(DigitalIn_87K()**)
	dwBuf[0] = 3;				   // COM Port
	dwBuf[1] = 06;				 // Address
	dwBuf[2] = 0x87054;		 // ID
	dwBuf[3] = 0;				   // CheckSum disable
	dwBuf[4] = 100;			   // TimeOut , 100 msecond
	
	dwBuf[6] = 0;				   // string debug
	getch();
	DigitalIn_87K(dwBuf, fBuf, szSend, szReceive);   // DI Input
  printf("DI= %u",dwBuf[5]);
  
  //--- digital output ----   ** Close DO **
	dwBuf[0] = 3;				  // COM Port
	dwBuf[1] = 06;				// Address
	dwBuf[2] = 0x87054;		// ID
	dwBuf[3] = 0;				  // CheckSum disable
	dwBuf[4] = 100;				// TimeOut , 100 msecond
	dwBuf[5] = 0x00;			// digital output
	dwBuf[6] = 0;			  	// string debug
	getch();              // push any key to continue
  wRetVal = DigitalOut_87K(dwBuf, fBuf, szSend, szReceive);
  
	Close_Com(COM3);
	return 0;
}
