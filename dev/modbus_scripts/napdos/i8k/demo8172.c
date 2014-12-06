/* Example of Read I/O port from i-8172/FR-Net modules.

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

   v 0.0.0  1 Sep 2006 by Moki Matsushima
     create, blah blah... */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "msw.h"
#define iTalkVersion	15
void	usage(void);
void setFRNET(int slot,int port, int address, WORD DOVal);
WORD getFRNET(int slot,int port, int address);

int main(int argc, char **argv)
{
	int wRetVal, slot, port, address;
	WORD DOVal,DIVal;	 
 if(argc !=4){
    slot = SLOT1;
    port = 0;
    address = 0;
  } else {
    slot = atoi(argv[1]);
    port = atoi(argv[2]);
    address = atoi(argv[3]);
  }
  
 /* if(argc == 4){
    slot = atoi(argv[1]);
    port = atoi(argv[2]);
    address = atoi(argv[3]);
  } else {
    usage();
		return FAILURE;
  }
*/  
  
  wRetVal = Open_Slot(slot);	
	if (wRetVal > 0) {
		printf("open Slot failed!\n");
		return (-1);
	}
	while(1){
 	for(DOVal=1;DOVal>0;(DOVal = DOVal<<1)){
  	 setFRNET(slot, port, address, DOVal); 
  	 usleep(500000);                       //500sec
  	 DIVal = getFRNET(slot, port, address);
  	 printf("FRNet Slot %d Port %d Address %d Output value = %04X Input value = %04X\n",slot,port,address,DOVal,DIVal);
     }
  for(DOVal=0x8000;DOVal>0;(DOVal = DOVal>>1)){
  	 setFRNET(slot, port, address, DOVal);
  	 usleep(500000);
  	 DIVal = getFRNET(slot, port, address);
  	 printf("FRNet Slot %d Port %d Address %d Output value = %04X Input value = %04X\n",slot,port,address,DOVal,DIVal);
    }
  }
	Close_Slot(slot);
	return SUCCESS;
}

void setFRNET(int slot, int port,int address, WORD DOVal){
  if(port == 0){  // FRNET Port 0 of I-8172 
    outb(slot,0x20+((address&0xf)<<1),DOVal&0xff);      //low
    outb(slot,0x21+((address&0xf)<<1),(DOVal>>8)&0xff); //high
  } else {        // FRNET Port 1 of I-8172
    outb(slot,0x30+((address&0xf)<<1),DOVal&0xff);
    outb(slot,0x31+((address&0xf)<<1),(DOVal>>8)&0xff);
  }
}

WORD getFRNET(int slot,int port,int address){    
  int lowByte,highByte;
  if(port == 0){ // FRNET Port 0 of I-8172
    lowByte = 0xff & inb(slot,0x20+((address&0xf)<<1)); 
    highByte = 0xff & inb(slot,0x21+((address&0xf)<<1));
  } else {       // FRNET Port 1 of I-8172
    lowByte = 0xff & inb(slot,0x30+((address&0xf)<<1));
    highByte = 0xff & inb(slot,0x31+((address&0xf)<<1));
  }
  return (WORD)((highByte<<8)|lowByte);
}

void usage(void)
{
	printf("ICPDAS iTalk utility v%d\n",iTalkVersion);
	printf("function:i-8172/FR-Net modules\n");
	printf("Set analog output value to a serial module\n");
	printf("Usage: 8172.exe slot port address\n");
	printf("Example :8172.exe 1 1 5\n");
	printf("Set i-8172 address 5 at slot1 from port1\n");
}
