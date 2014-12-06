#include "errexit.h"
#include "bsdsocket.h"

/*************************************************************************
 * passivsock
 * allocate & bind a server socket using TCP or UDP
 *
 * Arguments:
 *   service   service associated with the desired port
 *   protocol  name of protocol to use, "tcp" or "udp"
 *   qlen      maximum length of the server request queue
 *************************************************************************/
// u_short htons(), ntohs();
u_short portbase = 0;

int passivesock(char *service, char *protocol, int qlen)
{
	struct servent *pse;		/* pointer to service information entry */
	struct protoent *ppe;		/* pointer to protocol information entry */

	struct sockaddr_in sin;		/* an Internet endpoint address */

	int s, type;				/* socket descriptor and socket type */

	bzero((char *) &sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;

  /***********************************
   * map service name to port number *
   ***********************************/
	if (pse = getservbyname(service, protocol)) {
		sin.sin_port = htons(ntohs((u_short) pse->s_port) + portbase);
	} else if ((sin.sin_port = htons((u_short) atoi(service))) == 0) {
		errexit("can't get \"%s\" service entry\n", service);
	}

  /****************************************
   * map protocol name to protocol number *
   ****************************************/
	if ((ppe = getprotobyname(protocol)) == 0) {
		errexit("can't get \"%s\" protocol entry\n", protocol);
	}

  /****************************************
   * use protocol to choose a socket type *
   ****************************************/
	if (strcmp(protocol, "udp") == 0) {
		type = SOCK_DGRAM;
	} else {
		type = SOCK_STREAM;
	}

  /*********************
   * allocate a socket *
   *********************/
	s = socket(PF_INET, type, ppe->p_proto);
	if (s < 0) {
		errexit("can't create socket: %s\n", sys_errlist[errno]);
	}

  /*******************
   * bind the socket *
   *******************/
	if (bind(s, (struct sockaddr *) &sin, sizeof(sin)) < 0) {
		errexit("can't bine to %s port: %s\n", service,
				sys_errlist[errno]);
	}
	if (type == SOCK_STREAM && listen(s, qlen) < 0) {
		errexit("can't listen on %s port: %s\n", service,
				sys_errlist[errno]);
	}
	return s;
}

/*************************************************************************
 * passiveTCP
 * create a passive socket for use in a TCP server
 *
 * Arguments:
 *   service  service associated with the desired port
 *   qlen     maximum server request queue length
 *************************************************************************/
int passiveTCP(char *service, int qlen)
{
	return passivesock(service, "tcp", qlen);
}
