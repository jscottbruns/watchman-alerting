/*************************************************************************
 * bsdsocket.h
 *
 * v 0.0.0 2000.05.16 by Reed Lai
 *
 * head of bsdsocket.c
 *
 * History
 *
 * v 0.0.0 2000.05.16 by Reed Lai
 * create
 *************************************************************************/
#ifndef BSDSOCKET_H
#define BSDSOCKET_H

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netdb.h>

int passivesock(char *service, char *protocol, int qlen);
int passiveTCP(char *service, int qlen);

#endif							/* BSDSOCKET_H */
