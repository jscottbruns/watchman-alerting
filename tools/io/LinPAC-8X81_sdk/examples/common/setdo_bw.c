/* Set Digital Output for i8k modules.

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
	int RetValue,slot,value,bit;
	if (argc != 4) {
		printf("ICPDAS iTalk utility v%d\n",iTalkVersion);
		printf("function : setdo_bw\n");
		printf("Set digital output value to a module\n");
		printf("Usage: setdo_bw slot bit data\n");
		printf("Example :setdo 1 3 0\n");
		printf("Set the binary 0 to bit 3 of slot 1\n");
		return FAILURE;
	}

	slot=atoi(argv[1]);
	bit=atoi(argv[2]);
	value=atoi(argv[3]);
	
	/* open device file */
	RetValue = Open_Slot(slot);
	if (RetValue >0) {
		printf("open Slot %d failed!\n",slot);
		return FAILURE;
	}
	DO_32_BW(slot, bit, value);
	Close_Slot(slot);
	return SUCCESS;
}
