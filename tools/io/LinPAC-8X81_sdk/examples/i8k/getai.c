/* Get Analog Input for i8k modules.

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
	int RetValue,slot,channel,gain,mode;
	if (argc != 5) {
		printf("ICPDAS iTalk utility v%d\n",iTalkVersion);
		printf("function : getai\n");
		printf("Get analog input value to a module\n");
		printf("Usage: getai slot channel gain mode\n");
		printf("Example : ./getai.exe 1 2 3 0\n");
		printf("Get the analog value from channel 2 of slot 1 with gain 3 and mode 0\n");
		return FAILURE;
	}
	
	slot=atoi(argv[1]);
	channel=atoi(argv[2]);
	gain=atoi(argv[3]);
	mode=atoi(argv[4]);
	//printf("argc = %d argv0=%s argv1=%d argv2=%d\n",argc,dev_file,slot,value);
	/* open device file */
	RetValue = Open_Slot(slot);
	if (RetValue >0) {
		printf("open Slot %d failed!\n",slot);
		return FAILURE;
	}
	
	RetValue=I8017_Init(slot);
	I8017_SetChannelGainMode(slot,channel,gain,mode);
	printf("%f",I8017_GetCurAdChannel_Float_Cal(slot));
	Close_Slot(slot);
	return SUCCESS;
}
