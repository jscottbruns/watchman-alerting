/* Encoder demo for I-8090 i8k modules.

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

   v 0.0.0  1 Sep 2005 by Moki Matsushima
     create, blah blah... */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "msw.h"

int main(int argc, char **argv)
{
	int RetValue,slot,type;

	slot=1;
	RetValue = Open_Slot(slot);
	
	if (RetValue >0) {
		printf("open Slot %d failed!\n",slot);
		return FAILURE;
	}
	
	if (I8090_REGISTRATION(slot,0x0)==YES){
		I8090_INIT_CARD(slot,ENC_QUADRANT,ENC_QUADRANT,ENC_QUADRANT);
		I8090_RESET_ENCODER(slot,X_axis);
		I8090_RESET_ENCODER(slot,Y_axis);
		I8090_RESET_ENCODER(slot,Z_axis);
		while (1){
			printf("\rX= %d Y=%d Z=%d		",(short int)I8090_GET_ENCODER(slot,X_axis),(short int)I8090_GET_ENCODER(slot,Y_axis),(short int)I8090_GET_ENCODER(slot,Z_axis));
		}
		printf("\n");
	}
	else
		printf("8090 not found on Slot %d !\n",slot);


	Close_Slot(slot);
	return SUCCESS;
}
