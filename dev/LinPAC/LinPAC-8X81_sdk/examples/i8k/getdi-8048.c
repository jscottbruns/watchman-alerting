/* Get Digital Input for i-8048 modules.

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

   v 0.0.0  1 Feb 2008 by Moki Matsushima
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
	int fd;
	char *dev_file;
	slot_reg_t reg;
	
	if (argc != 2) {
		printf("ICPDAS iTalk utility v%d\n",iTalkVersion);
		printf("function : getdi-i8048\n");
		printf("Get digital input value from i8048\n");
		printf("Usage : getdi-i8048 slot\n");
		printf("Example : getdi-i8048 1 \n");
		printf("Get the 8bit dec digital input value from slot 1\n");
		return FAILURE;
	}
	
       sprintf(dev_file,"/dev/slot%d",atoi(argv[1]));
	//printf("argc = %d argv0=%s argv1=%d argv2=%s\n",argc,dev_file,atoi(argv[1]),argv[2]);
	/* open device file */
	fd = open(dev_file, O_RDWR);
	
	if (fd < 0) {
		printf("Failure of open device file \"%s.\"\n", dev_file);
		return FAILURE;
	}
	
	reg.offset=13;  /* by default, i8048 offset=13 */
	
	if (ioctl(fd, SLOT_READ_REG, &reg) ) {
		printf("Failure of ioctl command SLOT_READ_REG SLOT_PC.\n");
		close(fd);
		return FAILURE;
	}
	
	printf("%d\n",reg.value);
	close(fd);
	return SUCCESS;
}

