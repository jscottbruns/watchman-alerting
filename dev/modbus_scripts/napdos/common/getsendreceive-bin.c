/* Send and Receive Binary command from a i7k/i8k/i87k modules via serial port.

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

   v 0.0.0  1 Jan 2009 by Moki Matsushima
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
	WORD wT;                     /* total time of send/receive interval,unit=1ms */
  int s_length;
  WORD r_length;

	if (argc == 8) {
		slot=atoi(argv[1]);
		comport=atoi(argv[2]);
		timeout=atoi(argv[3]);
		baudrate=atol(argv[5]);
		s_length=atol(argv[6]);
		r_length=atol(argv[7]);
	}
	else if (argc == 7) {
		slot=atoi(argv[1]);
		timeout=atoi(argv[3]);
		comport = COM1;
		baudrate = 115200;
		s_length=atol(argv[5]);
		r_length=atol(argv[6]);
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
	
	if (RetValue) {
		printf("Module at COM%d error %d !!!\n",comport,RetValue);
		return FAILURE;
	}
	

  Send_Binary(comport,argv[4],s_length);
  Receive_Binary(comport,szReceive,timeout,r_length,&wT);
  printf("%s",szReceive);
}

void usage(void)
{
	printf("ICPDAS iTalk utility v%d\n",iTalkVersion);
	printf("function : getsendreceive-bin\n");
	printf("Send Binary command and wait resopnse from a serial module\n");
	printf("Usage:getsendreceive-bin slot 1 timeout binary-cmd s-length r-length\n");
	printf("      getsendreceive-bin slot comport timeout binary-cmd baudrate s-length r-length\n");
	printf("Example 1:getsendreceive-bin 2 1 10 '0123' 4 4\n");
	printf("Send bin-command 0123 to the module at slot 2 and wait response\n");
	printf("Example 2:getsendreceive-bin 0 3 10 '01234' 9600 5 5\n");
	printf("Send bin-command 01234 to the module at COM3 and wait response\n");
}
