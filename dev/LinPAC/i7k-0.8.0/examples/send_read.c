/*
 *  send_read.c
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
#include "i7k.h"

#define RCVBUF_SIZE 16

int main()
{
	int fd;						/* file descriptor */
	char *cmd;					/* point to command string */
	char rbuf[RCVBUF_SIZE];		/* receive buffer */
	__tod_t texp;				/* to receive the expended time */
	int r;

	fd = sio_open("/dev/ttyS0", B9600, DATA_BITS_8, NO_PARITY,
				  ONE_STOP_BIT);
	if (fd == ERR_PORT_OPEN) {
		printf("open port failed!\n");
		return (-1);
	}

	cmd = "$01M\r";

	sio_set_noncan(fd);
	sio_set_timer(fd, 0, 10);	/* set timeout for 1 sec */

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
