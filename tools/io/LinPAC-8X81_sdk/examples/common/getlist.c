/* Open port, then get serial number from DS2502. 

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

#include<stdio.h>
#include<stdlib.h>
#include "msw.h"

char szSend[80], szReceive[80];

int main()
{
	int i, wRetVal,wsRetVal, count;
//unsigned char serial_num[8];
	WORD wT;
	wRetVal = Open_Slot(0);
	szSend[0] = '$';
	szSend[1] = '0';
	szSend[2] = '0';
	szSend[3] = 'M';
	szSend[4] = 0;
	
	if (wRetVal > 0) {
		printf("open Slot failed!\n");
		return (-1);
	}
//Read_SN(serial_num);
//printf("serial number = %x%x%x%x%x%x%x%x\n",serial_num[0],serial_num[1],serial_num[2],serial_num[3],serial_num[4],serial_num[5],serial_num[6],serial_num[7]);
	wsRetVal = Open_Com(COM1, 115200, Data8Bit, NonParity, OneStopBit);
	count=GetSlotCount();
	//for (i=1;i<9;i++) {
	 for (i=1;i<count+1;i++) {
		ChangeToSlot(i);
		wRetVal = Open_Slot(i);
		if (wRetVal > 0)
			printf("slot %d ... busy\n",i);
		else if (GetNameOfModule(i)==8000) {
			if (wsRetVal > 0 ) printf("slot %d ... busy\n",i);
			else {
				wRetVal = Send_Receive_Cmd(COM1, szSend, szReceive, 1, 0, &wT);
				if (wRetVal)
					printf("slot %d ... not installed\n",i);
				else
					printf("slot %d ... %s\n",i,&szReceive[3]);
			}
		}
		else
			printf("slot %d ... %d\n",i,GetNameOfModule(i));
		Close_Slot(i);
	}
	Close_Com(COM1);
	Close_Slot(0);
	return 0;
}
