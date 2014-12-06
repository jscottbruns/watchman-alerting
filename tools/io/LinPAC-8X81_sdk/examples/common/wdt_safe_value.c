/* Watchdog test for DI/O modules ,for example: i87063

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

   v 0.0.0  1 April 2007 by Moki Matsushima create, blah blah... */
    
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "msw.h"

#define iTalkVersion	15

char	a, szSend[30], szReceive[30]; 
WORD wT; 
int RetValue,i;
void reset_status()
{
  // Reset module status , send command '~AA1'          
  szSend[0] = '~';     //delimiter character      
	szSend[1] = '0';     //AA: address of module (00 to FF)
	szSend[2] = '0';
	szSend[3] = '1';     //1 : command for all modules
	szSend[4] = 0;
	RetValue = Send_Receive_Cmd(COM1, szSend, szReceive, 20 , 0, &wT); 
  if (RetValue) 
  {
		printf("Reset Module status failed, Error Code=%d !\n",RetValue);
		//return FAILURE;
	}
}


int main(int argc, char **argv)
{
/* open device file */ 
//slot=1, comport=1, baudrate=115200, databit=8, non-parity, stopbit=1
	if (Open_Slot(0)) 
  {
		printf("open Slot0 failed!\n");
		return FAILURE;
	}
	ChangeToSlot(1);
  
  RetValue = Open_Com(COM1, 115200, Data8Bit, NonParity, OneStopBit);
	if (RetValue >0) 
  {
		printf("open COM1 failed!\n");
		return FAILURE;
	}

	 reset_status();
	 
	// set safe value , send command '~AA5S'
  strcpy(szSend,"");
  strcpy(szReceive,"");
	szSend[0] = '~';     //delimiter character      
	szSend[1] = '0';     //AA: address of module (00 to FF)
	szSend[2] = '0';
	szSend[3] = '5';     //3: command for set host watchdog timeout value
	szSend[4] = 'S';     //S: command of the safe value
	szSend[5] = 0;
	RetValue = Send_Receive_Cmd(COM1, szSend, szReceive, 10 , 0, &wT); 
  if (RetValue) 
  {
		printf("Module at COM1 failed, Error Code=%d !\n",RetValue);
		return FAILURE;
	}
	 
  
  printf("\n");
  printf("             Watchdog test for i87063\n");
  printf("====================================================\n");
  printf("Please choose one of the following:\n");
  printf("       1). Start test i87063 wdt at slot 1.\n");
  printf("       2). Exit program.\n");
  printf("====================================================\n");
  printf("\n");
  printf("Choose your answer [Enter]...");
	
  a=getche();
  switch (a)
  {
    case '1':
        // unsigned long StartTime=GetTimeTicks_ms();
        // set module DO=0 , send command '@aa(data)'
        strcpy(szSend,"");
        strcpy(szReceive,"");
        szSend[0] = '@';          //delimiter character      
	      szSend[1] = '0';          //AA: address of module (00 to FF)
	      szSend[2] = '0';
	      szSend[3] = '0';          //set DO = 0
        szSend[4] = 0;
        Send_Receive_Cmd(COM1, szSend, szReceive, 10 , 0, &wT); 
     
	      // Enable Watchdog timeout value , send command '~AA3EVV'
	      szSend[0] = '~';     //delimiter character
	      szSend[1] = '0';     //AA: address of module (00 to FF)
	      szSend[2] = '0';     
	      szSend[3] = '3';     //3: command for set host watchdog timeout value
	      szSend[4] = '1';     //E: 1=enable , 0= disable
	      szSend[5] = '6';     //VV: timeout value, from 01 to FF, each for 0.1 second
	      szSend[6] = '4';
	      szSend[7] = 0;
	      RetValue = Send_Receive_Cmd(COM1, szSend, szReceive, 15 , 0, &wT); 
	  
        // Host OK , send command '~**'
        strcpy(szSend,"");
        strcpy(szReceive,"");
        szSend[0] = '~';          //delimiter character      
	      szSend[1] = '*';          //**: command for all modules
	      szSend[2] = '*';
	      szSend[3] = 0;
        Send_Receive_Cmd(COM1, szSend, szReceive, 5 , 0, &wT); 
	   
  	    printf("\n");   
        for (i=1;i<11;i++)
        { //printf("%lus: ",(GetTimeTicks_ms()-StartTime)/1000); 
          printf("Waitting for 10 seconds set safe value by wdt... %d sec\n",i);
          sleep(1);        
        }  
      
   	    	  
        // Disable Watchdog timeout value , send command '~AA3EVV' 
	      strcpy(szSend,"");
        strcpy(szReceive,"");
	      szSend[0] = '~';     //delimiter character
	      szSend[1] = '0';     //AA: address of module (00 to FF)
	      szSend[2] = '0';     
	      szSend[3] = '3';     //3: command for set host watchdog timeout value
	      szSend[4] = '0';     //E: 1=enable , 0= disable
	      szSend[5] = '6';     //VV: timeout value, from 01 to FF, each for 0.1 second
	      szSend[6] = '4';
	      szSend[7] = 0;
	      RetValue = Send_Receive_Cmd(COM1, szSend, szReceive, 10 , 0, &wT); 
        if (RetValue) 
        {
		       printf("Module Disable failed, Error Code=%d !\n",RetValue);
		       return FAILURE;
	      }
            
        printf("Set safe value successful!\n");
        reset_status();
        break;
    case '2':
        reset_status();
        exit(0);
        break;     
    default: 
        reset_status();
        printf("Please choose one again!");       
  }
    
  
  Close_Com(COM1);
	Close_Slot(0);
	return 0;
}
 
