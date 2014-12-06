
/* Demo for I-8084W (pluse-dir.c)

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
   
   
/*  Channel configurations:
    Xor=0 for Low Actived (signal from High to Low ==> change count)
    Low pass filters disabled
    all channel are set to Pulse/Dir counter
    
Wiring:
    active:   connected to 4.5~24V for isolated input
              connected to 2~5V for non-isolated input (TTL signal)
    inactive: open or connected to GND
    
    For Ch0, Ch1(A0,B0)
        if (B0 is active)    --> A0 = Up counting Ch0, Ch1
        if (B0 is inactive)  --> A0 = Down counting Ch0, Ch1
        
    For Ch2, Ch3(A1,B1)
        if (B1 is active)    --> A1 = Up counting Ch2, Ch3
        if (B1 is inactive)  --> A1 = Down counting Ch2, Ch3

    For Ch4, Ch5(A2,B2)
        if (B2 is active)    --> A2 = Up counting Ch4, Ch5
        if (B2 is inactive)  --> A2 = Down counting Ch4, Ch5
        
    For Ch6, Ch7(A3,B3)
        if (B3 is active)    --> A3 = Up counting Ch6, Ch7
        if (B3 is inactive)  --> A3 = Down counting Ch6, Ch7        
*/


#include "msw.h"
#include <string.h>
#include <stdio.h> // for prototype for string interpretation routine
#include <stdlib.h>

int main(int argc, char **argv)
{
    int slot,channel,i,Ret,iRet,RetValue; 
    unsigned long T1,T2,T3;
    unsigned int  Overflow, DIvalue;
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
    printf("=====================================================\n\r");
    printf(" Operation mode: Pulse/Dir counting mode.            \n\n\r");
    printf(" Press 'Ctrl c' or 'q' to quit the program.          \n\r");
    printf("=====================================================\n\r");
  
    printf("\n");

    printf("Slot number(1~8)[Enter]=");
    scanf("%d",&slot);
  
    if (Open_Slot(slot)) 
    {
		  printf("open Slot %d failed!\n",slot);
		  return FAILURE;
	  }
	  ChangeToSlot(slot);
	  
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
           printf("Some Pulse/Dir channels have one count offset.\n\r");
           printf("Return code: %02X\n\r",iRet);
        }
        
        for (channel=0; channel<8; channel++)
        {
            i8084W_SetXorRegister(slot,channel,0); // XOR=0 (Low Actived)
            iRet=i8084W_SetChannelMode(slot,channel,0);  // Pulse/Dir counter mode
                
            if (iRet>0)
              printf("The Pulse/Dir channel %d has one count offset.\n\r",channel);    
           
            i8084W_SetLowPassFilter_Status(slot,channel,0); //Disable LPF
            i8084W_SetLowPassFilter_Us(slot,channel,1); //Set LPF width= 0.001 ms    
          
            i8084W_ClrCnt(slot,channel);   //Clear all count at beginning.
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
               /* i8084W_AutoScan();  */
             
               // Dir/Pulse counting mode   
                printf("Pulse/Dir: %lus \n",(GetTimeTicks_ms()-luStartTime)/1000);
                 
                i8084W_ReadCntPulseDir(slot,0,&Count_Now[channel],&Overflow);
                printf(" A0/B0 Count=%08ld  \n",Count_Now[channel]);
                
                i8084W_ReadCntPulseDir(slot,2,&Count_Now[channel],&Overflow);
                printf(" A1/B1 Count=%08ld  \n",Count_Now[channel]);
                
                i8084W_ReadCntPulseDir(slot,4,&Count_Now[channel],&Overflow);
                printf(" A2/B2 Count=%08ld  \n",Count_Now[channel]);
                
                i8084W_ReadCntPulseDir(slot,6,&Count_Now[channel],&Overflow);
                printf(" A3/B3 Count=%08ld  \n",Count_Now[channel]);
                
                i8084W_ReadDI_Xor(slot,&DIvalue);
                printf("DI=%x \n\n\r",DIvalue);   
                                
               
            } 
            
      } 
    }
  
  Close_Slot(slot);
  Close_Com(COM1);
  return 0;
}

