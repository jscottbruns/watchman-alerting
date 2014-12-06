/* Get ASCII response from a i7k/i8k/i87k modules via serial port.

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
char	szSend[80], szReceive[80],module_name[10];
int main(int argc, char **argv)
{
	int RetValue,slot,comport,timeout;
	int wBits,wCheck,wStopBits;
	DWORD baudrate;
	WORD wT;
	if (argc == 6) {
		slot=atoi(argv[1]);
		comport=atoi(argv[2]);
		timeout=atoi(argv[3]);
		baudrate=atol(argv[4]);
		wBits = argv[5][0]-'0';
		wStopBits = argv[5][2]-'0'-1;
		switch(argv[5][1]){
			case 'n':
			case 'N':
				wCheck = NonParity;
				break;
			case 'o':
			case 'O':
				wCheck =OddParity;
				break;
			case 'e':
			case 'E':
				wCheck = EvenParity;
				break;
		}
	}
	else {
		usage();
		return FAILURE;
	}
	
	/* open device file */
	RetValue = Open_Com(comport, baudrate, wBits, wCheck, wStopBits);
	
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
	
	RetValue = Receive_Cmd(comport, szReceive, timeout, 0);
/*	if (RetValue) {
		printf("Module at COM%d error %d !!!\n",comport,RetValue);
		return FAILURE;
	}*/
	
	printf("%s",szReceive);
}

void usage(void)
{
	printf("ICPDAS iTalk utility v%d\n",iTalkVersion);
	printf("function : getreceive\n");
	printf("Get ASCII response from a serial module\n");
	printf("Usage: getreceive slot comport timeout baudrate format\n");
	printf("Example :getreceive 0 2 5 9600 8n1\n");
	printf("Get response from the module at COM2 with 8n1 format in 0.5s\n");
}
