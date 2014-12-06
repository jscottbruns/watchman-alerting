/* demo for i-8144 module.

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

#define COM_M1 "/dev/ttyS2"  /* i-8144  port 1  in slot 1 */ 
#define COM_S1 "/dev/ttyS3"  /* i-8144  port 2  in slot 1 */ 
#define COM_M2 "/dev/ttyS4"  /* i-8144  port 3  in slot 1 */ 
#define COM_S2 "/dev/ttyS5"  /* i-8144  port 4  in slot 1 */ 
#define RCVBUF_SIZE 64

int main()
{
  char fd[5];
	int i,r;			        /* file descriptor */
	char *cmd;				             	/* point to command string */
	char rbuf[RCVBUF_SIZE];			   	/* receive buffer */
	__tod_t texp;				           	/* to receive the expended time */
	

  fd[0] = sio_open(COM_M1, B9600, DATA_BITS_8, NO_PARITY,ONE_STOP_BIT);
	fd[2] = sio_open(COM_M2, B9600, DATA_BITS_8, NO_PARITY,ONE_STOP_BIT);
	if (fd[0] == ERR_PORT_OPEN) {
		printf("open port_m failed!\n");
		return (-1);
	}
	if (fd[2] == ERR_PORT_OPEN) {
		printf("open port_m failed!\n");
		return (-1);
	}
	fd[1] = sio_open(COM_S1, B9600, DATA_BITS_8, NO_PARITY,ONE_STOP_BIT);
	fd[3] = sio_open(COM_S2, B9600, DATA_BITS_8, NO_PARITY,ONE_STOP_BIT);
	if (fd[1] == ERR_PORT_OPEN) {
		printf("open port_s failed!\n");
		return (-1);
	}
  if (fd[3] == ERR_PORT_OPEN) {
		printf("open port_s failed!\n");
		return (-1);
	}
  printf("fd[0]=%d\n", fd[0]);
  printf("fd[1]=%d\n", fd[1]);
  printf("fd[2]=%d\n", fd[2]);
  printf("fd[3]=%d\n", fd[3]);
 
  cmd = "$01M";
	sio_set_noncan(fd[0]);
	sio_set_noncan(fd[1]);
	sio_set_noncan(fd[2]);
	sio_set_noncan(fd[3]);

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
	sio_set_timer(fd[0], 0, 0);         	/* set timeout for 0 sec */
	sio_set_timer(fd[1], 0, 0);	        /* set timeout for 0 sec */
	sio_set_timer(fd[2], 0, 0);         	/* set timeout for 0 sec */
	sio_set_timer(fd[3], 0, 0);	        /* set timeout for 0 sec */
  if (write(fd[0], cmd, strlen(cmd)) == -1) {
		printf("Write error!\n");
		return (-1);
	}
  if (write(fd[2], cmd, strlen(cmd)) == -1) {
		printf("Write error!\n");
		return (-1);
	}
	
  for(i=1;i<5;i++)                     //read one every time
  { 
    //Sleep(100);
    while(!(r=read(fd[1], &rbuf[i], sizeof(rbuf[i])))){}
    printf("read %d byte --> ",r);
    printf("%c\n",rbuf[i]);
    
  }
  for(i=1;i<5;i++)                     //read one every time
  { 
    //Sleep(100);
    while(!(r=read(fd[3], &rbuf[i], sizeof(rbuf[i])))){}
    printf("read %d byte --> ",r);
    printf("%c\n",rbuf[i]); 
  }
  
  printf("\n");
  
  sio_close(fd[0]);
  sio_close(fd[1]);
  sio_close(fd[2]);
  sio_close(fd[3]);
  return (0);
}
