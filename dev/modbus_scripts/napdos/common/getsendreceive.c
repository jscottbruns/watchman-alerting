/* Send and Receive ASCII command from a i7k/i8k/i87k modules via serial port.

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
	DWORD baudrate;
	WORD wT;                     /* total time of send/receive interval,unit=1ms  */
	if (argc == 6) {
		slot=atoi(argv[1]);
		comport=atoi(argv[2]);
		timeout=atoi(argv[3]);
		baudrate=atol(argv[5]);
	}
	else if (argc == 5) {
		slot=atoi(argv[1]);
		timeout=atoi(argv[3]);
		comport = COM1;
		baudrate = 115200;
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
	
	RetValue = Send_Receive_Cmd(comport, argv[4], szReceive, timeout, 0, &wT);
	
	if (RetValue) {
		printf("Module at COM%d error %d !!!\n",comport,RetValue);
		return FAILURE;
	}
	printf("%s",szReceive);
}

void usage(void)
{
	printf("ICPDAS iTalk utility v%d\n",iTalkVersion);
	printf("function : getsendreceive\n");
	printf("Send ASCII command and wait resopnse from a serial module\n");
	printf("Usage: getsendreceive slot 1 timeout command\n");
	printf("       getsendreceive slot comport timeout command baudrate\n");
	printf("Example 1:getsendreceive 2 1 1 '$00M'\n");
	printf("Send command $00M to the module at slot 2 and wait response\n");
	printf("Example 2:getsendreceive 0 3 1 '$01M' 9600\n");
	printf("Send command $01M to the module at COM3 and wait response\n");
}
