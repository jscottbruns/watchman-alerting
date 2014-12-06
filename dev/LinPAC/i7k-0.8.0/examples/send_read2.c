/*
 *  send_read2.c
 *
 *  v 0.0 1999.12.20 by Reed Lai
 *
 *  open port, then quest the name of module 01
 *
 *  History
 *
 *  v 0.0 1999.12.20 by Reed Lai
 *    create, ... ok, i keep this as simple as possible
 */

// #define I7K_DEBUG
#include "msw.h"
#include "i7k.h"
#include "i7000.h"



#define RCVBUF_SIZE 16

int main()
{
	int fd,rs;						/* file descriptor */
	char *cmd;					/* point to command string */
	char rbuf[RCVBUF_SIZE];		/* receive buffer */
	__tod_t texp;				/* to receive the expended time */
	int comport = COM1;
	int timeout = 1;
	WORD wT;

	rs = Open_Com(comport, 9600, Data8Bit, NonParity, OneStopBit);

	if (rs > 0) {
		printf("open port failed!\n");
		return (-1);
	}

	cmd = "$01M\r";

        rs = Send_Receive_Cmd(comport, cmd, rbuf, timeout, 0, &wT); 

	printf("Send Command = %s Receive Command = %s \n",cmd, rbuf);

        if (rs)
                printf("Send Receive Command, Error Code=%d\n", rs);


	Close_Com(comport);
	return (0);
}
