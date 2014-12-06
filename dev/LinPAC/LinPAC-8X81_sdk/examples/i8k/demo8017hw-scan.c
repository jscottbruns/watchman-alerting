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
	
long long curTime( )
{
    struct timeval cur_tm;
    gettimeofday(&cur_tm,NULL);
    return (long long)cur_tm.tv_sec*1000000 + cur_tm.tv_usec;
}

int main()
{
	int i,j,channel = 0, wRetVal, error = 0;
	float fAi;
	long long start;	
	unsigned int polling = 16;
	int vol = 5;

	//I-8024   
	wRetVal = Open_Slot(2);
	if (wRetVal > 0) {
		printf("open Slot failed!\n");
		return (-1);
	}
	
	//I8024 Initial
	I8024_Initial(2);
	
	//I8024_AO Output
	I8024_VoltageOut(2,3,vol);
	Close_Slot(2);
	
	//I-8017H  
	wRetVal = Open_Slot(3);
	if (wRetVal > 0) {
		printf("open Slot failed!\n");
		return (-1);
	}
	
	//I8017H Initial
	I8017_Init(3);
	start = curTime();

	for(channel = 0; channel < 8 ; channel++)
	{
		//I8017H_Channel Set
		I8017_SetChannelGainMode(3,channel,0,0);
	
		for(j=0; j<polling; j++)
		{
			fAi = HEX_TO_FLOAT_Cal(I8017HW_GetCurAdChannel_Hex(3),3,0);
			//printf("data %d 8017_AI_Cal_Float =%f\n\n\n",j+1,fAi);	
		}

		if(fAi > vol || fAi < (vol-1))
			error++;
	}

	printf("The sample rate of \"8 channel scan\" = datacount %d / total %f ms = %f K S/s DataError : %d\n\n", (polling), 1e-3*(curTime()-start), ((polling)/(1e-3*(curTime()-start))), error);

	Close_Slot(3);

	return 0;
}
