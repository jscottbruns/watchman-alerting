/* Open port, then quest the name of module 01 

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
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. 

   File level history (record changes for this file here.)

   uart.c
   
   v 0.0 1 Sep 2004 by Moki Matsushima         */


// #define I7K_DEBUG
#include "i7k.h"

#define RCVBUF_SIZE 16

int main()
{
	int fd;					        	   /* file descriptor */
	char *cmd;				           /* point to command string */
	char rbuf[RCVBUF_SIZE];	     /* receive buffer */
	__tod_t texp;				         /* to receive the expended time */
	int r;

	fd = sio_open("/dev/ttyS0", B9600, DATA_BITS_8, NO_PARITY,ONE_STOP_BIT);
				  
	if (fd == ERR_PORT_OPEN) {
		printf("open port failed!\n");
		return (-1);
	}

	cmd = "$01M\r";
	sio_set_noncan(fd);          /* set a opened serial port to non-canonical mode */
	sio_set_timer(fd, 0, 10);	   /* set timeout for 1 sec */
	r = i7k_send_readt(fd, cmd, rbuf, RCVBUF_SIZE, &texp);

	switch (r) {
	case 0:
		printf("time-out!\ntime expended: %llu us\n", texp);
		break;
	case -1:
		printf("error!\ntime expended: %llu us\n", texp);
		break;
	default:
		printf("%s\ntime expended: %llu us\n", rbuf, texp);
		break;
	}

	sio_close(fd);
	return (0);
}
