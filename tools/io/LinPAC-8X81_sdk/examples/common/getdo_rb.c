/* Get Digital Output Read back for i8k modules.

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
   v 0.0.0  1 Dec 2006 by Moki Matsushima
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
	int RetValue,slot,type;
	if (argc != 3) {
		printf("ICPDAS iTalk utility v%d\n",iTalkVersion);
		printf("function : getdo\n");
		printf("Get digital output read back value from a module\n");
		printf("Usage: getdo slot type\n");
		printf("Example :getdo 1 16\n");
		printf("Get the 16bit dec digital output value from slot 1\n");
		return FAILURE;
	}
	slot=atoi(argv[1]);
	type=atoi(argv[2]);
//	printf("argc = %d argv0=%s argv1=%d argv2=%d\n",argc,dev_file,slot,value);
	/* open device file */
	RetValue = Open_Slot(slot);
	if (RetValue >0) {
		printf("open Slot %d failed!\n",slot);
		return FAILURE;
	}
	switch(type)
	{
		case 16:
			printf("%d",DO_16_RB(slot));
			break;
		case 32:
			printf("%d",DO_32_RB(slot));
			break;
		default:
			printf("%d",DO_8_RB(slot));
			break;

	}
	Close_Slot(slot);
	return SUCCESS;
}
