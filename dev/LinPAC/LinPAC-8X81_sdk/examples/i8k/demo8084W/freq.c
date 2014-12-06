
/* Demo for I-8084W (Frequency)

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
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

/* File level history (record changes for this file here.)

   v 0.0.0  1 June 2009 by Cindy Huang create, blah blah... */

/*
   Channel configurations:
    Xor=0 for Low Actived (signal from High to Low ==> change count)
    Low pass filters disabled
    all channel are set to Freq mode
    
Wiring:
    Ch0=A0=Freq0
    Ch1=B0=Freq1
    Ch2=A1=Freq2
    Ch3=B1=Freq3
    Ch4=A2=Freq4
    Ch5=B2=Freq5
    Ch6=A3=Freq6
    Ch7=B3=Freq7
*/

#include "msw.h"
#include <string.h>
#include <stdio.h> // for prototype for string interpretation routine
#include <stdlib.h>

int main(int argc, char **argv)
{
    int slot,channel,i,Ret,iRet,RetValue; 
    int AutoMode_UpdateTime=330, LowMode_UpdateTime=1000, HighMode_UpdateTime=100;
    //mode 0: Auto mode: update time = 330 ms
    //     1: Low mode : update time = 1000 ms
    //     2: High mode: update time = 100 ms
    unsigned long T1,T2,T3;
    unsigned long Frequency;   
    unsigned int  Overflow,DIvalue;
    unsigned long luStartTime;
    unsigned long luDelta;
    unsigned long lMaxCount,lMinCount;
    unsigned long Count_Now[8];
    unsigned long Count_Old[8];
    
/* open device file  */
// comport=1, baudrate=115200, databit=8, non-parity, stopbit=1
   RetValue = Open_Com(COM1, 115200, Data8Bit, NonParity, OneStopBit);
	 if (RetValue >0) 
   {
		 printf("open COM1 failed!\n");
		 return FAILURE;
	 }

    printf("\n");
    printf("               I-8084W Demo Testing  \n");
    printf("===================================================== \n\r");
    printf(" Operation mode:Frequency mode.                       \n\n\r");
    printf(" Press 'Ctrl c' to quit the program.           \n\r");
    printf("=====================================================\n\r");
    
    printf("\n");
   
    printf("Slot number(1~8)[Enter]=");
    scanf("%d",&slot);
  
    if (Open_Slot(slot)) 
    {
		  printf("open Slot %d failed!\n",slot);
		  return FAILURE;
	  }
	  //ChangeToSlot(slot);
	  
	  for (channel=0; channel<8; channel++)
	  {
	     Count_Old[channel]=0;//set every channel be zero
	  }
    iRet=i8084W_InitDriver(slot);
    if (iRet==(-1))
    {
		  printf("Initiate I-8084W on slot %d error!\n\r",slot);
		  printf("Cannot find I-8084W.");
	  }
	  else
	  {
		    printf("Initiate I-8084W on slot %d ok.\n\n\r",slot);
        if(iRet>0)
        {  
            printf("Some Frequency channels have one count offset.\n\r");   
            printf("Return code: %02X\n\r",iRet);
        }
        
        for (channel=0; channel<8; channel++)
        {
            i8084W_SetXorRegister(slot,channel,0); // XOR=0 (Low Actived)
            i8084W_SetChannelMode(slot,channel,2); // 2: frequency mode
             
            i8084W_SetLowPassFilter_Status(slot,channel,0); //Disable LPF
            i8084W_SetLowPassFilter_Us(slot,channel,1); //Set LPF width= 0.001 ms
            i8084W_SetFreqMode(slot,channel,1);  //mode 0: Auto mode: update time = 330 ms
                      
           // i8084W_ClrCnt(slot,channel);   //Clear all count at beginning.
        }
        
        //T1=*TimeTicks;
        luStartTime=T1=GetTimeTicks_ms();
        //T1=GetTimeTicks_ms();
        T2=T1;

       for (;;)
       {  
            T3=GetTimeTicks_ms();
            
            if ((T3-T1) >50) // Auto scan 8084W every 50 ms (one time one slot)
            {
                T1=T3;
                i8084W_AutoScan();
            }
        
            if ((T3-T2) >1000) // 500 -->Print frequency every 500 ms
            {
                T2=T3;
                /*i8084W_AutoScan();  */
             
                printf("Frequency: %lus \n ",(GetTimeTicks_ms()-luStartTime)/1000);
                i8084W_SetFreqUpdateTime(slot, AutoMode_UpdateTime, LowMode_UpdateTime, HighMode_UpdateTime);
            
					      i8084W_ReadFreq(slot,0,&Frequency);
                printf("A0=%06lu  \n ",Frequency);
                
                i8084W_ReadFreq(slot,1,&Frequency);
                printf(" B0=%06lu  \n ",Frequency);
                
                i8084W_ReadFreq(slot,2,&Frequency);
                printf(" A1=%06lu  \n ",Frequency);
                
                i8084W_ReadFreq(slot,3,&Frequency);
                printf(" B1=%06lu  \n ",Frequency);
                
                i8084W_ReadFreq(slot,4,&Frequency);
                printf(" A2=%06lu  \n ",Frequency);
                
                i8084W_ReadFreq(slot,5,&Frequency);
                printf(" B2=%06lu  \n ",Frequency);
                
                i8084W_ReadFreq(slot,6,&Frequency);
                printf(" A3=%06lu  \n ",Frequency);
                
                i8084W_ReadFreq(slot,7,&Frequency);
                printf(" B3=%06lu  \n ",Frequency);
               
                i8084W_ReadDI_Xor(slot,&DIvalue);
                printf("DI=%x \n\n\r",DIvalue);   
                     
            }
        } 
     }
  
  Close_Slot(slot);
  Close_Com(COM1);
  return 0;
}

