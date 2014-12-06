#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#define BufferLength 255
#define RemotePort 49950

int main(int argc, char *argv[])
{

	int sd, rc, length = sizeof(int);
	struct sockaddr_in serveraddr;
	char buffer[BufferLength];
	char server[255];
	char temp;
	int totalcnt = 0;
	struct hostent *hostp;
	char data[3];

	if ( ( sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Client-socket() error");
		exit(-1);
	}

	if(argc == 3)
	{
		strcpy(server, argv[1]);	
		strcpy(data, argv[2]);	
	}
	else
	{
		perror("Not enough params \n");
		exit(-1);
	}

	memset(&serveraddr, 0x00, sizeof(struct sockaddr_in));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(RemotePort);

	if((serveraddr.sin_addr.s_addr = inet_addr(server)) == (unsigned long)INADDR_NONE)
	{
		hostp = gethostbyname(server);
		memcpy(&serveraddr.sin_addr, hostp->h_addr, sizeof(serveraddr.sin_addr));
	} 

	if((rc = connect(sd, (struct sockaddr *)&serveraddr, sizeof(serveraddr))) < 0)
	{
		perror("Client-connect() error");
		close(sd);
		exit(-1);
	}

	printf("Writing [%s] to host [%s] \n", data, server);
	rc = write(sd, data, sizeof(data));

	if(rc < 0)
	{
		perror("Client-write() error");
		rc = getsockopt(sd, SOL_SOCKET, SO_ERROR, &temp, &length);

		if(rc == 0)	
		{
			errno = temp;
			perror("SO_ERROR was");
		}
		close(sd);
		exit(-1);
	}

	totalcnt = 0;

	rc = recv(sd, buffer, sizeof(buffer) - 1, 0);
	if ( rc < 0 )
	{
		perror("Client-read() error");
		close(sd);
		exit(-1);
	}
	else if (rc == 0)
	{
		printf("Server program has issued a close()\n");
		close(sd);
		exit(-1);
	}
	
	printf("Client-read() is OK\n");
	printf("Response (%d) %s \n", rc, buffer);

	close(sd);
	exit(0);

	return 0;
}

