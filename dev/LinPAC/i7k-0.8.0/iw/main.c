#define IW_DEBUG 2				/* unmark to show debug message */

#include <sys/types.h>
#include <sys/signal.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <netinet/in.h>

#include <netdb.h>

#include "i7k.h"

#include "errexit.h"
#include "bsdsocket.h"

#define QLEN    5				/* maximum connection queue length */
#define BUFSIZE 4096

#define SIO_PORT "/dev/ttyS0"

/*************************************************************************
 * reaper
 * clean up zombie children
 *************************************************************************/
void reaper(int sig)
{
	union wait status;
	while (wait3(&status, WNOHANG, (struct rusage *) 0) >= 0);
}

/*************************************************************************
 * main
 *************************************************************************/
int main(int argc, char *argv[])
{
	char *service = "iwarp";	/* server name or port number */
	struct sockaddr_in fsin;	/* the address of a client */
	int alen;					/* length of client's address */
	int msock;					/* master server socket */
	int ssock;					/* slave server socket */

	switch (argc) {
	case 1:
		break;
	case 2:
		service = argv[1];
		break;
	default:
		errexit("usage: TCPechod [port]\n");
	}

  /**********************
   * start iWarp server *
   **********************/
	msock = passiveTCP(service, QLEN);

	signal(SIGCHLD, reaper);

	while (1) {
		alen = sizeof(fsin);
		ssock = accept(msock, (struct sockaddr *) &fsin, &alen);
		if (ssock < 0) {
			if (errno == EINTR) {
				continue;
			}
			errexit("accept: %s\n", sys_errlist[errno]);
		}

		switch (fork()) {
		case 0:				/* child */
			(void) close(msock);
			exit(TCPechod(ssock, &fsin));
		default:				/* parent */
			(void) close(ssock);
			break;
		case -1:
			errexit("fork: %s\n", sys_errlist[errno]);
		}
	}
}

/*************************************************************************
 * TCPechod
 * echo data until end of file
 *************************************************************************/
int TCPechod(int fd, struct sockaddr_in *sin)
{
	char buf[BUFSIZE + 1];
	char cmd[16];
	int cc;
	int sio;
	__tod_t exp;

#ifdef IW_DEBUG
	printf("connected    -> %lu.%lu.%lu.%lu:%u\n",
		   sin->sin_addr.s_addr & 0x0FF,
		   sin->sin_addr.s_addr >> 8 & 0x0FF,
		   sin->sin_addr.s_addr >> 16 & 0x0FF,
		   sin->sin_addr.s_addr >> 24 & 0x0FF, ntohs(sin->sin_port));
#endif							/* IW_DEBUG */

  /********************
   * open serial port *
   ********************/
	sio = sio_open(SIO_PORT, B9600, DATA_BITS_8, NO_PARITY, ONE_STOP_BIT);
	if (sio == ERR_PORT_OPEN) {
#ifdef IW_DEBUG
		printf("cannot open port!\n");
#endif							/* IW_DEBUG */
	}
	sio_set_timer(sio, 0, 1);	/* set time-out */

  /********************
   * read from socket *
   ********************/
	while (cc = read(fd, buf, sizeof(buf))) {
		if (cc < 0) {
			errexit("echo read: %s\n", sys_errlist[errno]);
			sio_close(sio);
		}
		buf[cc] = '\0';
#if IW_DEBUG > 1
		printf("read %s\n", buf);
#endif							/* IW_DEBUG */

	/***********************
     * warp to serial port *
     ***********************/
		sprintf(cmd, "%s\r", buf);
		buf[0] = '\0';
		i7k_send_readt(sio, cmd, buf, BUFSIZE, &exp);
		cc = strlen(buf);
		if (!cc) {
			buf[0] = '-';
			buf[1] = '-';
			buf[2] = '\0';
		}

	/*******************
     * write to socket *
     *******************/
#if IW_DEBUG > 1
		printf("send %s\n", buf);
#endif							/* IW_DEBUG */
		if (write(fd, buf, strlen(buf)) < 0) {
			errexit("echo write: %s\n", sys_errlist[errno]);
			sio_close(sio);
		}
	}

  /*********************
   * end of connection *
   *********************/
#ifdef IW_DEBUG
	printf("disconnected -> %lu.%lu.%lu.%lu:%u\n",
		   sin->sin_addr.s_addr & 0x0FF,
		   sin->sin_addr.s_addr >> 8 & 0x0FF,
		   sin->sin_addr.s_addr >> 16 & 0x0FF,
		   sin->sin_addr.s_addr >> 24 & 0x0FF, ntohs(sin->sin_port));
#endif							/* IW_DEBUG */

	sio_close(sio);
	return 0;
}
