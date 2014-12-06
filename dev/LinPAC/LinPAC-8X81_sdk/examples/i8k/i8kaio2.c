/* Test I-8017H AI Module

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

   i8kaio2.c
   
   v 0.0 1 Sep 2004 by Moki Matsushima
   
   create, ... ok, i keep this as simple as possible
 */


#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>
#include "msw.h"
	
char szSend[80], szReceive[80];
DWORD dwBuf[12];
float fBuf[12];

int main()
{
	int i, wRetVal,wchannel,wslot;
	float fAi;
	int hAi;
	struct timeval start,end;
	/* COM2, Baudrate=9600, 8 data bit, non-parity, 1 stop bit */
	gettimeofday(&start,NULL);
	for(wslot = 1; wslot < 8;wslot++) {
		wRetVal = Open_Slot(wslot);
		if (wRetVal > 0) {
			printf("open Slot failed!\n");
			return (-1);
		}
		printf("Slot %d 8017 library version =%d\n",wslot,I8017_Init(wslot));
		for (wchannel=0;wchannel<8;wchannel++) {
			I8017_SetChannelGainMode(wslot,wchannel,0,0);
			fAi = I8017_GetCurAdChannel_Float_Cal(wslot);	
			printf("Slot %d Channel %d 8017 Float Analog In =%f\n",wslot,wchannel,fAi);
		}
		Close_Slot(wslot);
	}
	
	gettimeofday(&end,NULL);
	printf("\r\nTime = %ld us \n\r",end.tv_usec-start.tv_usec);
	printf("\r\nTime = %ld ms \n\r",(end.tv_usec-start.tv_usec)/1000);
	return 0;
}
