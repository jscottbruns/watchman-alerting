/* Open port, then send/receive from i87k module. 

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

   send_receive.c
   
   v 0.0.0  1 Sep 2004  by Moki Matsushima
   
*/

#include<stdio.h>
#include<stdlib.h>
#include "msw.h"


char szSend[80], szReceive[80];
DWORD dwBuf[12];
float fBuf[12];

int main()
{
	int wRetVal;
	DWORD temp;
	WORD wT;
	/* COM2, Baudrate=9600, 8 data bit, non-parity, 1 stop bit */

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
	//--- digital output ----
	szSend[0] = '$';
	szSend[1] = '0';
	szSend[2] = '0';
	szSend[3] = 'M';
	szSend[4] = 0;
	
	wRetVal = Send_Receive_Cmd(COM1, szSend, szReceive, 20, 0, &wT);
	printf("Send Command = %s Receive Command = %s \n",szSend,szReceive);
	
	if (wRetVal)
		printf("Send Receive Command, Error Code=%d\n", wRetVal);
	
	Close_Com(COM1);
	Close_Slot(0);
	return 0;
}
