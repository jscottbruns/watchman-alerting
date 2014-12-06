/* ========================================================================== 
 Demo for 8050 

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

   v 0.0.0  1 May 2007 by Moki Matsushima create, blah blah... *
                                                                        
   8050.c                                                               
   (c) 2001 Author                                                                                                                             
 ========================================================================== */

#include<stdio.h>
#include<stdlib.h>
#include "msw.h"

int main()
{
   unsigned int slot, type, ret, data;
   unsigned short config;
   int name;
   printf("\n\r");
   printf("                   I-8050 Demo Testing            \n\r");
   printf("=========================================================\n\r");
   printf(" Please choose one of the following :                    \n\r");
   printf("        1.) All for DO mode (Ch0~Ch15).                  \n\r");   
   printf("        2.) All for DI mode (Ch0~Ch15).                  \n\r"); 
   printf("        3.) Ch0~Ch7 is DO mode and Ch8~Ch15 is DI mode.  \n\r");
   printf("=========================================================\n\r");
   printf("\n");
   printf("Choose your answer :");
              
   scanf("%d",&type);
   printf("Slot number(0~7) :");
   scanf("%d",&slot);
  
/* open device file  */
   if (Open_Slot(slot)) 
   {
	   printf("open Slot %d failed!\n",slot);
	   return FAILURE;
	 }
	     
  switch (type)
  {
    case 1:                                    // All for DO type (Ch0~Ch15)  
        UDIO_WriteConfig_16(slot, 0xFFFF);      // 1:DO, 0:DI
        
        ret=UDIO_ReadConfig_16(slot);
        printf("Read the I/O Type is : 0x%04lx  \n\r",ret);
        
        printf("Input DO value: ");
        scanf("%d:",&data);
        ret=UDIO_DO16( slot, data);
        if (ret==0)
          printf("DO of I-8050 in Slot %d = 0x%x\n\r",slot, data);
        else 
          printf("Digital Output error!\n\r");   
        break; 
          
    case 2:                                   // All for DI type (Ch0~Ch15)
        UDIO_WriteConfig_16(slot, 0x0000);     // 1:DO, 0:DI
        
        ret=UDIO_ReadConfig_16(slot);
        printf("Read the I/O Type is : 0x%04lx  \n\r",ret);
           
        data=UDIO_DI16(slot);    
        printf("DI of I-8055 in Slot %d = 0x%x\n",slot, data);/**/
        break;
        
    case 3:                                   // Ch0~Ch7 is DO type and Ch8~Ch15 is DI type.         
        UDIO_WriteConfig_16(slot, 0x00FF);     // 1:DO, 0:DI
                
        ret=UDIO_ReadConfig_16(slot);
        printf("Read the I/O Type is : 0x%04lx  \n\r",ret);
        
        data=UDIO_DI16(slot);    
        printf("DI(Ch8~Ch15) of I-8055 in Slot %d = 0x%x\n\r",slot, data);
        printf("Input DO value: ");
        scanf("%d:",&data);
        ret=UDIO_DO16( slot, data);
        if (ret==0)
          printf("DO(Ch0~Ch7) of I-8050 in Slot %d = 0x%x\n\r",slot, data);
        else 
          printf ("Digital Output error!\n\r");         
        break;  
  }
   
/* wait for exit 
	 puts("press <enter> to exit...\n");
	 while (getchar() != 10);
	 Close_Slot(slot);
	 puts("\nEnd of program !");
	 return SUCCESS;  */
	
           
	Close_Slot(slot);
	return 0;
}
