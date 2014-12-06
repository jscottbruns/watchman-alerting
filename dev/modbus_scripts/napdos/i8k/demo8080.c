
/* Demo for 8080 

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

   v 0.0.0  1 May 2007 by Moki Matsushima create, blah blah... */

#include "msw.h"
#include <string.h>
#include <stdio.h> // for prototype for string interpretation routine
#include <stdlib.h>

int main(int argc, char **argv)
{
    int slot,channel,iRet,mode,i,Ret,RetValue; 
    int AutoMode_UpdateTime=330, LowMode_UpdateTime=1000, HighMode_UpdateTime=100;
    //mode 0: Auto mode: update time = 330 ms
    //     1: Low mode : update time = 1000 ms
    //     2: High mode: update time = 100 ms
    unsigned long T1,T2,T3;
    unsigned long Frequency;
    
    unsigned long Count32;
    unsigned int  Overflow;
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
    printf("               I-8080 Demo Testing  \n");
    printf("=====================================================\n\r");
    printf(" Please choose one of the following operation mode:\n\r");
    printf("        0.) Dir/Pulse counting mode.               \n\r");
    printf("        1.) Up/Down counting mode.                 \n\r");
    printf("        2.) Frequency mode.                        \n\r");
    printf("        3.) Up counting mode.                      \n\r");
    printf("=====================================================\n\r");
    //printf("Press 'Ctrl c' to quit.\n\r");
    printf("\n");
    printf("Choose your answer [Enter]...");
    
    //mode=getch();
    scanf("%d",&mode);
    printf("Slot number(0~7)[Enter]=");
    scanf("%d",&slot);
    
    //slot=getch();
    //Close_Slot(slot);
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
    iRet=i8080_InitDriver(slot);
    if (iRet==(-1))
    {
		  printf("Initiate I-8080 on slot %d error!\n\r",slot);
		  printf("Cannot find I-8080.");
	  }
	  else
	  {
		    printf("Initiate I-8080 on slot %d ok.\n\r",slot);
        if(iRet>0)
        {
            printf("Some Pulse/Dir channels have one count offset.\n\r");
            printf("Return code: %02X\n\r",iRet);
        }
        
        switch (mode)
        {
              case 0:
                printf("Dir/Pulse counting mode: \n\r");
                break;
              case 1:
                printf("Up/Down counting mode: \n\r");
                break;
              case 2:
                printf("Frequency mode: \n\r");
                break;
              case 3:
                printf("Up counting mode: \n\r");
                break;
        } 
        
        for (channel=0; channel<8; channel++)
        {
            i8080_SetXorRegister(slot,channel,0); // XOR=0 (Low Actived)
            i8080_SetChannelMode(slot,channel,mode); 
                //mode 0: Pulse/Dir counter mode
                //     1: Up/Down counter mode
                //     2: frequency mode
                //     3: Up counter mode
            
            i8080_SetLowPassFilter_Status(slot,channel,0); //Disable LPF
            i8080_SetLowPassFilter_Us(slot,channel,1); //Set LPF width= 0.001 ms    
            //i8080_SetFreqMode(slot,channel,0);
            i8080_ClrCnt(slot,channel);
        }
        
        //T1=*TimeTicks;
        luStartTime=T1=GetTimeTicks_ms();
        //T1=GetTimeTicks_ms();
        T2=T1;

       for (;;)
       {  
            T3=GetTimeTicks_ms();
            
            if ((T3-T1) >50) // Auto scan 8080 every 50 ms (one time one slot)
            {
                T1=T3;
                i8080_AutoScan();
            }
        
            if ((T3-T2) >1000) // 500 -->Print frequency every 500 ms
            {
                T2=T3;
                i8080_AutoScan();  /**/
             
              switch (mode)
              {
                case 0:   // Dir/Pulse counting mode   
                    printf("%lus: ",(GetTimeTicks_ms()-luStartTime)/1000);
                    for (channel=0; channel<8; channel++)   
                    { 
                        i8080_ReadCntPulseDir(slot,channel,&Count_Now[channel],&Overflow);
                        luDelta=Count_Now[channel]-Count_Old[channel];
                        printf("%d[%08lX] ",luDelta,Count_Now[channel]);
                        Count_Old[channel]=Count_Now[channel]; 
                    }
                    
					          lMinCount=lMaxCount=Count_Now[0];
                    for(i=1;i<8;i++)
                    {
                        if(lMaxCount<Count_Now[i])
                            lMaxCount=Count_Now[i];
                        
                        if(lMinCount>Count_Now[i])
                            lMinCount=Count_Now[i];
                    }   
                    printf("D=%d\n\r",lMaxCount-lMinCount);
                    
                    break;
                    
               case 1:   // Up/Down counting mode
                  
                    printf("%lus: ",(GetTimeTicks_ms()-luStartTime)/1000);
                    for (channel=0; channel<8; channel++)   
                    { 
                        i8080_ReadCntUpDown(slot,channel,&Count_Now[channel],&Overflow);                 
                        luDelta=Count_Now[channel]-Count_Old[channel];
                        printf("%d[%08lX] ",luDelta,Count_Now[channel]);
                        Count_Old[channel]=Count_Now[channel]; 
                    }
                        
					          lMinCount=lMaxCount=Count_Now[0];
                    for(i=1;i<8;i++)
                    {
                        if(lMaxCount<Count_Now[i])
                           lMaxCount=Count_Now[i];
                        
                        if(lMinCount>Count_Now[i])
                           lMinCount=Count_Now[i];
                    }   
                    printf("D=%d\n\r",lMaxCount-lMinCount);
                    
                    
                    break;  
               case 2:  // Frequency mode
                    
                    printf("%lus: ",(GetTimeTicks_ms()-luStartTime)/1000);
                    i8080_SetFreqUpdateTime(slot, AutoMode_UpdateTime, LowMode_UpdateTime, HighMode_UpdateTime);
                  
                    for (channel=0; channel<8; channel++)   
                    {   
                      i8080_SetFreqMode(slot,channel,0);   
                      i8080_ReadFreq(slot,channel,&Frequency);                
					            printf("%lu[%08lX] ",Frequency,Frequency);                    
                    }
                    
                    printf("\n");/**/
                     
                    break;       
                case 3:  // Up counter mode
                    //i8080_ReadCntUp(slot,channel,&Count32,&Overflow);
                    printf("%lus: ",(GetTimeTicks_ms()-luStartTime)/1000);
                    
                    for (channel=0; channel<8; channel++)   
                    {   
                      i8080_ReadCntUp(slot,channel,&Count_Now[channel],&Overflow);                                  
                      luDelta=Count_Now[channel]-Count_Old[channel];
					            printf("%lu[%08lX] ",luDelta,Count_Now[channel]);        
					            Count_Old[channel]=Count_Now[channel];  
                    }
                    // total count
                    lMinCount=lMaxCount=Count_Now[0];
                    for(i=1;i<8;i++)
                    {
                        if(lMaxCount<Count_Now[i])
                            lMaxCount=Count_Now[i];
                        
                        if(lMinCount>Count_Now[i])
                            lMinCount=Count_Now[i];
                    }              
                    printf("D=%d\n\r",lMaxCount-lMinCount);
                    
                    break;
                  
                }
                
            } //if ((T3-T2) >500)
        
            //Press "q" to quit program.
           /* scanf("%d",&iRet);
            //iRet=getchar();
            if(iRet==113) //Press 'q' to quit the program.
            {
                return;
            }*/
            
           
      }  //for
    }
  
  Close_Slot(slot);
  Close_Com(COM1);
  return 0;
}

