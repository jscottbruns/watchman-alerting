/* Get port value for i8k module.

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

   v 0.0.0  1 Sep 2004 by Mok Matsushima
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
	
	if (argc != 3) {
		printf("ICPDAS iTalk utility v%d\n",iTalkVersion);
		printf("function : getport\n");
		printf("Get port value from a module\n");
		printf("Usage: getport slot offset\n");
		printf("Example :getport 1 0\n");
		printf("Get the dec value from offset 0 of slot 1\n");
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
	
	reg.offset=atoi(argv[2]);
	
	if (ioctl(fd, SLOT_READ_REG, &reg) ) {
		printf("Failure of ioctl command SLOT_READ_REG SLOT_PC.\n");
		close(fd);
		return FAILURE;
	}
	
	printf("%d",reg.value);
	close(fd);
	return SUCCESS;
}
