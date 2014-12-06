/* Test with I-8024 AO Module and I-8017H AI Module at Slot 01 and 02 of LinCon seperately
   
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

   i8kaio.c
   
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
	int i, wRetVal,j;
	float fAi;
	int hAi, chAi, Succ;
	int Arr_hAi[5];
	float Arr_fAi[5];
	
	
	//I-8024   
	wRetVal = Open_Slot(1);
	if (wRetVal > 0) {
		printf("open Slot failed!\n");
		return (-1);
	}
	
	//I8024 Initial
	I8024_Initial(1);
	
	//I8024_AO Output
	I8024_VoltageOut(1,0,5.5);
	Close_Slot(1);
	
	//I-8017H  
	wRetVal = Open_Slot(2);
	if (wRetVal > 0) {
		printf("open Slot failed!\n");
		return (-1);
	}
	
	//I8017H Initial
	I8017_Init(2);
	//I8017H_Channel Set
	I8017_SetChannelGainMode(2,2,0,0);
	
	//First Method: Get AI Value
	hAi = I8017_GetCurAdChannel_Hex(2);         //Get Not-calibrated AI Hex Value
	printf("8017_AI_not_Cal_Hex =%x\n",hAi); 
	fAi = HEX_TO_FLOAT_Cal(hAi,2,0);            //Not-calibrated AI Hex Value modify to calibrated AI Float Value
	printf("8017_AI_Cal_Float =%f\n\n",fAi);
	
	//Second Method: Get AI Value
	hAi = I8017_GetCurAdChannel_Hex_Cal(2);     //Get Calibrated AI Hex Value
	printf("8017_AI_Cal_Hex =%x\n",hAi);
	fAi = CalHex_TO_FLOAT(hAi,0);               //Calibrated AI Hex Value modify to Calibrated AI Float Value
	printf("8017_AI_Cal_Float =%f\n\n",fAi);
	
	//Third Method: Get AI Value
	fAi = I8017_GetCurAdChannel_Float_Cal(2);	  //Get Calibrated AI Float Value Directly
	printf("8017_AI_Cal_Float =%f\n\n\n",fAi);	

	Close_Slot(2);
	
	return 0;
}
