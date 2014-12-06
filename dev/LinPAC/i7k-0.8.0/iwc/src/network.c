/**********************************************************************
 **********************************************************************/

/**********************************************************************
 *
 *  network.c
 *
 *  v 0.0.0 2000.04.18 by Reed Lai
 *
 *  network utilities
 *
 *  History:
 *
 *  v 0.0.0 2000.04.18 by Reed Lai
 *    create, blah blah...
 *
 **********************************************************************/
// #define NET_DEBUG /* unmark to show network debug information */

#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netdb.h>

#include <gnome.h>

#include "network.h"

/*********************
 * Local definitions *
 *********************/
#define TCP "tcp"
#define UDP "udp"

#define REMOTE_ADDR     { 192, 168, 255, 12 }
#define REMOTE_ADDR_LEN 4
#define REMOTE_PORT     1500


int             skt = 0;	/* socket's file descriptor */
int             block = 0;	/* flag when busy */

/**********************************************************************
 *  net_unlock
 *  release exclusive socket
 **********************************************************************/
void
net_unlock()
{
    if (block) {
	block--;
    }
}

/**********************************************************************
 *  net_lock
 *  exclusive socket
 *  Return:
 *    zero for success
 **********************************************************************/
int
net_lock()
{
    int             error = 0;
    block++;
    if (block > 1) {		/* lock failed */
	error = block;
	block--;
    }
    return (error);
}

/**********************************************************************
 *  close socket
 *
 *  Arguments:  socket's file descriptor
 **********************************************************************/
int
close_socket(int s)
{
    int             error;

    error = shutdown(s, 0);
#ifdef NET_DEBUG
    g_print("socket shutdown error %d\n", error);
#endif				/* NET_DEBUG */
    error = close(s);
#ifdef NET_DEBUG
    g_print("socket close error %d\n", error);
#endif				/* NET_DEBUG */
    return error;
}

int
net_close()
{
#ifdef NET_DEBUG
    g_print("close socket.\n");
#endif				/* NET_DEBUG */
    return (close_socket(skt));
}

/**********************************************************************
 *  net_open
 *  open socket and make connection
 *
 *  Arguments:
 *    addr     unsigned long int array, holds the ip address
 *    port     unsigned short int value, the connection port
 *
 *  Return:
 *    0 successsul, or failed
 **********************************************************************/
int
net_open(unsigned long int addr[], unsigned short int port)
{
    struct protoent *pe;	/* protocol info entry */
    struct sockaddr_in sin;	/* damn! */

    char           *protocol;
    // unsigned long addr[] = REMOTE_ADDR;

    int             error = 0;

  /****************
   * address type *
   ****************/
    sin.sin_family = AF_INET;

  /*******************************
   * get protocol number by name *
   *******************************/
    protocol = TCP;
    pe = getprotobyname(protocol);
    if (pe == 0) {
	return -1;		/* can't get protocol entry */
    }

  /***********************************
   * set port number by service name *
   ***********************************/
    // sin.sin_port = htons( REMOTE_PORT );
    sin.sin_port = htons(port);

  /********************
   * set host address *
   ********************/
    sin.sin_addr.s_addr =
	addr[0] + (addr[1] << 8) + (addr[2] << 16) + (addr[3] << 24);

  /*****************
   * create socket *
   *****************/
    skt = socket(PF_INET, SOCK_STREAM, pe->p_proto);
    if (skt < 0) {
	return skt;		/* socket creating failed! */
    }

  /***********
   * connect *
   ***********/
#ifdef NET_DEBUG
    g_print("connect ...");
#endif				/* NET_DEBUG */
    error = connect(skt, (struct sockaddr *) &sin, sizeof(sin));
    if (error) {
	close_socket(skt);
#ifdef NET_DEBUG
	g_print(" error!\n");
#endif				/* NET_DEBUG */
    }
#ifdef NET_DEBUG
    g_print(" ok\n");
#endif				/* NET_DEBUG */
    return error;
}

/**********************************************************************
 *  net_send_read
 *  send and read response
 *
 *  Arguments:
 *    snd        string to send
 *    rcv        read buffer
 *    rcv_max    buffer size in bytes
 *    attempted  attempted counter for receiving
 **********************************************************************/
int
net_send_read(char *snd, char *rcv, unsigned int rcv_max,
	      unsigned int *attempted)
{
    unsigned int    i;
    int             oc,
                    rn,
                    wn;
    int             error = 0;

  /********
   * send *
   ********/
#ifdef NET_DEBUG
    g_print("sending command %s ...", snd);
#endif				/* NET_DEBUG */
    oc = strlen(snd);
    wn = write(skt, snd, oc);
#ifdef NET_DEBUG
    g_print(" done.\n");
#endif				/* NET_DEBUG */

  /****************
   * get response *
   ****************/
    i = rn = 0;
#ifndef NET_DEBUG
    while ((rn == 0 || rn < 0) && i++ < 111) {
	rn = read(skt, rcv, rcv_max);
	if (rn == 0) {		/* End of File */
	    rcv[0] = '\0';
	    error = 0;
	} else if (rn < 0) {	/* Error */
	    error = rn;
	} else {		/* Got Response */
	    rcv[rn] = '\0';
	    error = 0;
	}
    }
#else
    while ((rn == 0 || rn < 0) && i++ < 111) {
	g_print("read response ...");
	rn = read(skt, rcv, rcv_max);
	if (rn == 0) {		/* End of File */
	    rcv[0] = '\0';
	    error = 0;
	    g_print(" EOF  try %d\n", i);
	} else if (rn < 0) {	/* Error */
	    error = rn;
	    g_print(" error: %d  try %d\n", errno, i);
	} else {		/* Got Response */
	    rcv[rn] = '\0';
	    error = 0;
	    g_print(" %d: \"%s\"  try %d\n", rn, rcv, i);
	}
    }
    g_print("end of read response with return code: %d\n", error);
#endif				/* NET_DEBUG */

    *attempted = i;
    return error;
}
