/**********************************************************************
 **********************************************************************/

/**********************************************************************
 *
 *  network.h
 *
 *  v 0.0.0 2000.04.25 by Reed Lai
 *
 *  head of network.c
 *
 *  History:
 *
 *  v 0.0.0 2000.04.25 by Reed Lai
 *    create, blah blah...
 *
 **********************************************************************/

int             send_read(char *, char *, unsigned int);

int             net_open(unsigned long int addr[],
			 unsigned short int port);
int             net_close();
int             net_send_read(char *snd, char *rcv, unsigned int rcv_max,
			      unsigned int *attempted);
int             net_lock();
void            net_unlock();
