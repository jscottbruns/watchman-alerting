/* Set Analog Output for i7k/i8k/i87k modules via serial port.

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

   v 0.0.0  1 April 2005 by Moki Matsushima
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
char	szSend[20], szReceive[20],module_name[10];
DWORD	dwBuf[12];
WORD	wBuf[12];
float	fBuf[12],fValue;
int main(int argc, char **argv)
{
	int RetValue,slot,comport,addr,channel;
	DWORD baudrate,dwValue,dwModuleName;
	WORD wT;
	if (argc == 7) {
		slot=atoi(argv[1]);
		comport=atoi(argv[2]);
		fValue=atof(argv[3]);
		channel=atoi(argv[4]);
		baudrate=atol(argv[5]);
		addr=atoi(argv[6]);
	}
	else if (argc == 5) {
		slot=atoi(argv[1]);
		fValue=atof(argv[3]);
		channel=atoi(argv[4]);
		comport = COM1;
		baudrate = 115200;
		addr=0;
	}
	else {
		usage();
		return FAILURE;
	}
	/* open device file */
	RetValue = Open_Com(comport, baudrate, Data8Bit, NonParity, OneStopBit);
	
	if (RetValue >0) {
		printf("open COM%d failed!\n",comport);
		return FAILURE;
	}
	if (comport == COM1) {
		RetValue = Open_Slot(0);	
		if (RetValue >0) {
			printf("open Slot %d failed!\n",slot);
			Close_Com(comport);
			return FAILURE;
		}
		ChangeToSlot(slot);
	}
	
	szSend[0] = '$';
	szSend[1] = (addr/16);
	
	if (szSend[1]<10) szSend[1] += '0';
	else szSend[1] +='A';
		
	szSend[2] = (addr%16);
	
	if (szSend[2]<10) szSend[2] += '0';
	else szSend[2] +='A';
	
	szSend[3] = 'M';
	szSend[4] = 0;
	RetValue = Send_Receive_Cmd(comport, szSend, szReceive, 1, 0, &wT);
	
	if (RetValue) {
		printf("Module at COM%d Address %d error !!!\n",comport,addr);
		return FAILURE;
	}
	
	module_name[0]=szReceive[3];
	module_name[1]=szReceive[4];
	module_name[2]=szReceive[5];
	module_name[3]=szReceive[6];
	module_name[4]=0;
	
	if ((module_name[0]=='8')&&(module_name[1]=='7')){
		module_name[4]=szReceive[7];
		module_name[5]= 0 ;
	}
	else if ((module_name[0]=='8')&&(module_name[1]!='7')){
		module_name[1]='0';
		szSend[3] = 'T';
		if(slot>3) {
			szSend[4] = '1';
			szSend[5] = 0;
		}
		else 
			szSend[4] = 0;

		RetValue = Send_Receive_Cmd(comport, szSend, szReceive, 1, 0, &wT);
		
		if (RetValue) {
			printf("Module at COM%d Address %d error !!!\n",comport,addr);
			return FAILURE;
		}
		if (szReceive[3+(slot-1)*2] == '7') {
			module_name[2]='1';
			module_name[3]='7';
		}
		else {
			module_name[2]=szReceive[3+(slot-1)*2];
			module_name[3]=szReceive[4+(slot-1)*2];
		}
	}
	
	dwModuleName = module_name[0]-'0';
	dwModuleName *= 16;
	dwModuleName += module_name[1]-'0';
	dwModuleName *= 16;
	dwModuleName += module_name[2]-'0';
	dwModuleName *= 16;
	dwModuleName += module_name[3]-'0';
	
	if (module_name[1] == '7') {
		dwModuleName *= 16;
		dwModuleName += module_name[4]-'0';
	}
	
	if (module_name[0]=='7') {
		wBuf[0] = comport;		              //COM Port number
		wBuf[1] = addr;			                //address
		wBuf[2] = (WORD)dwModuleName;	      //module name
		wBuf[3] = 0;			                  //check sum
		wBuf[4] = 100;			                //timeout
		wBuf[5] = channel;		              //analog channel
		wBuf[6] = 0;		                  	//debug
		fBuf[0] = fValue;		                //analog value
		RetValue = AnalogOut(wBuf,fBuf,szSend,szReceive);
		//dwValue = wBuf[5];
		//fValue = fBuf[0];
	}
	else {
		dwBuf[0] = comport;		              //COM Port number
		dwBuf[1] = addr;		                //address
		dwBuf[2] = dwModuleName;	          //module name
		dwBuf[3] = 0;			                  //check sum
		dwBuf[4] = 100;		                	//timeout
		dwBuf[5] = channel;	              	//analog channel
		dwBuf[6] = 0;			                  //debug
		dwBuf[7] = slot;	                	//slot
		fBuf[0] = fValue;	                	//analog value
		if (module_name[1]=='7')
			RetValue=AnalogOut_87K(dwBuf,fBuf,szSend,szReceive);
		else
			RetValue=AnalogOut_8K(dwBuf,fBuf,szSend,szReceive);
		//dwValue = wBuf[5];
		//fValue = fBuf[0];
	}
	if (RetValue) {
		printf("Module at COM%d Address %d DI error %d !!!\n",comport,addr,RetValue);
		return FAILURE;
	}
		
	if (comport == COM1) {
		Close_Slot(0);
	}
	Close_Com(comport);
	return SUCCESS;
}

void usage(void)
{
	printf("ICPDAS iTalk utility v%d\n",iTalkVersion);
	printf("function : setexao\n");
	printf("Set analog output value to a serial module\n");
	printf("Usage: setexao slot 1 value channel\n");
	printf("       setexao slot comport value channel baudrate address\n");
	printf("Example 1:setexao 2 1 6.7 5\n");
	printf("Set channel 5 analog value 6.7 to the module at slot 2\n");
	printf("Example 2:setexao 0 3 6.7 5 9600 2\n");
	printf("Set channel 5 analog value 6.7 to the module at COM3\n");
}
