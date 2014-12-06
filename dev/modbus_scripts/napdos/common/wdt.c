/* Watchdog time test.

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

   v 0.0.0  1 Sep 2004 by Moki Matsushima
     create, blah blah... */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "msw.h"
#define iTalkVersion	15

int main(int argc, char **argv)
{
	int rcsr;
	/* open device file */
	if (Open_Slot(SLOT0)){
		printf("open Slot %d failed!\n",SLOT0);
		return FAILURE;
	}
	
	rcsr = WatchDogSWEven();
	printf("RCSR = %x ",rcsr);
	
	if(rcsr&0x1) printf("Hardware reset ");
	if(rcsr&0x2) printf("Software reset ");
	if(rcsr&0x4) printf("Watchdog reset ");
	if(rcsr&0x8) printf("Sleep mode reset ");
		
	printf("\n");
	printf("Press Enter to refresh Watchdog timer or other key to stop demo\n");
	printf("LinCon will reboot if not refresh watchdog timer\n");
	EnableWDT(10000);	//enable watchdog timer interval 10000ms, 10s
	
	while (getchar()==10){
		printf("refresh watchdog timer\n");
		EnableWDT(10000);      	/* refresh watchdog timer 10s */
	}
	
	printf("Disable watchdog timer\n");
	DisableWDT();
	ClearWDTSWEven(0xf);
	Close_Slot(SLOT0);
	return SUCCESS;
}
