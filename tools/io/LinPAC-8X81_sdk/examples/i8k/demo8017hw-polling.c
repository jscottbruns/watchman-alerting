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
	
unsigned long getjiffies(void);

unsigned long getjiffies()
{
        int fd;
        unsigned long sec,usec;
        char buf[64];

        fd = open("/proc/uptime",O_RDONLY);

        if (fd == -1)
        {
                perror("open");
                return 0;
        }
        memset(buf,0,64);
        read(fd,buf,64);
        sscanf(buf,"%u.%u",&sec,&usec);
        close(fd);

        //the number of milliseconds since system was started
        return ((sec * 100 + usec)*10);
}

int main()
{
	int i, wRetVal,error = 0;
	float fAi;
	unsigned long start, totalms;	
	unsigned int datacount = 8000;
	int *hAi = malloc(sizeof(int)*datacount);
	int vol = 10;

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
	//I8017H_Channel Set
	start = getjiffies();

	I8017HW_AD_POLLING(3,2,0,datacount,hAi);

	for(i=0;i<datacount;i++)
	{
		fAi = HEX_TO_FLOAT_Cal(hAi[i],3,0);
		if(fAi > vol || fAi < (vol-1))
                	error++;
		//printf("data %d 8017_AI_Cal_Float =%f\n\n\n",i+1,fAi);
	}

	totalms = getjiffies()-start;
	printf("The sample rate of \"single channel polling\" = datacount %d / total %d ms = %d K S/s DataError : %d\n\n", datacount, totalms, ((datacount)/totalms),error);
	Close_Slot(3);


	return 0;
}
