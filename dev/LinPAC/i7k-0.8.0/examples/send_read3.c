/*
 *  send_read3.c
 *
 *  v 0.0 2011.05.18 by Golden Wang
 *
 *  open VCOM port, send and receive test message from ttyVCOM0
 *
 *  History
 *
 *  v 0.0 2011.05.18 by Golden Wang
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
	int comport = 0;	/* minor 0 = /dev/ttyVCOM0 */
	int timeout = 1;
	WORD wT;

	rs = Open_Com(comport, 9600, Data8Bit, NonParity, OneStopBit);

	if (rs > 0) 
	{
		printf("open port failed!\n");
		return -1;
	}

	cmd = "test\r";

	Send_String(comport, cmd, 0);
	printf(" Send String = %s \n", cmd);


	Receive_String(comport, rbuf, timeout, 0);

	printf(" Receive String = %s \n", rbuf);

        if (rs)
                printf("Receive String, Error Code=%d\n", rs);

	Close_Com(comport);

	return 0;
}
