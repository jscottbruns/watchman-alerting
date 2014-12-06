/* non-blocking demo for i-8144 module.

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
   v 0.0.0  1 Jan 2007 by Moki Matsushima
     create, blah blah... */
     
#include "msw.h"

#define COM_M "/dev/ttyS2"
#define COM_S "/dev/ttyS3"
#define RCVBUF_SIZE 16

int main()
{
	int fd_m,fd_s,i,r;			        /* file descriptor */
	char *cmd;				             	/* point to command string */
	char rbuf[RCVBUF_SIZE];			   	/* receive buffer */
	__tod_t texp;				           	/* to receive the expended time */
	

	fd_m = sio_open(COM_M, B9600, DATA_BITS_8, NO_PARITY,ONE_STOP_BIT);
	if (fd_m == ERR_PORT_OPEN) {
		printf("open port_m failed!\n");
		return (-1);
	}
	
	fd_s = sio_open(COM_S, B9600, DATA_BITS_8, NO_PARITY,ONE_STOP_BIT);
	if (fd_s == ERR_PORT_OPEN) {
		printf("open port_s failed!\n");
		return (-1);
	}

  cmd = "$01M";
	sio_set_noncan(fd_m);
	sio_set_noncan(fd_s);

/**********************************************************************
 *  sio_set_timer
 *
 *  set input condictions for given port, this function for
 *  non-canonical mode only
 *
 *  Arguments: 
 *    fd       file descriptor for the port
 *    vmin     minmum number of bytes to read
 *    vtime    interbyte timer
 *             the number of tenths-of-a-second to wait for data to arrive
 *             
 *  Returned:  void
 */
	sio_set_timer(fd_m, 0, 0);         	/* set timeout for 0 sec */
	sio_set_timer(fd_s, 0, 0);	        /* set timeout for 0 sec */
	
  if (write(fd_m, cmd, strlen(cmd)) == -1) {
		printf("Write error!\n");
		return (-1);
	}
  
  for(i=1;i<5;i++)                     //read one every time
  { 
    //Sleep(100);
    while(!(r=read(fd_s, &rbuf[i], sizeof(rbuf[i])))){}
    printf("\n");
    printf("read %d byte --> ",r);
    printf("%c",rbuf[i]);
  }
  
  printf("\n");
  
  sio_close(fd_m);
  sio_close(fd_s);
  return (0);
}
