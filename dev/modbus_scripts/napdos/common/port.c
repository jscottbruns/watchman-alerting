/* Example of Read/Write Port for LinCon.

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

int main()
{
	int doVal,wRetVal;

	/* open device file */
	wRetVal = Open_Slot(SLOT1);
	
	if (wRetVal > 0) {
		printf("open Slot failed!\n");
		return (-1);
	}
	
	printf("Module ID = %d \n",IoRead(SLOT1,0x0));
	/* read, get out if error or ESC pressed */
	doVal = 1;
	
	while (getchar() != 27) {
		if(doVal < 0xFF) doVal=doVal*2;
		else doVal = 1;
		IoWrite(SLOT1,0x0,doVal);
	}
	puts("End of program");

	Close_Slot(SLOT1);
	return SUCCESS;
}
