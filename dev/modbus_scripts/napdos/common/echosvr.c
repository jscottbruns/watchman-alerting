/* Creates a socket and connects it to the specified remote host. 

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
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. 
   
   File level history (record changes for this file here.)

   echosvr.c
   
   v 0.0 1 Sep 2004 by Moki Matsushima
   
   */

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <netdb.h>

/*
int my_write(int fd,void *buffer,int length)
{
    int bytes_left;
    int written_bytes;
    char *ptr;

     ptr=buffer;
    bytes_left=length;
    while(bytes_left>0)
    {

        written_bytes=write(fd,ptr,bytes_left);
        if(written_bytes<=0)
        {
            if(errno==EINTR)
                written_bytes=0;
            else
                return(-1);
        }
        bytes_left-=written_bytes;
        ptr+=written_bytes;
    }
    return(0);
}
*/

#define BUFSIZE  4096

int echo(int fd);

int main(int argc, char *argv[])
{
    int sockfd,new_fd;
    int sin_size,portnumber;
    int re = 1;
    int nfds,fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    char hello[] = "Hello,Hello\n\a";
    char buffer[BUFSIZE];
    
    fd_set rfds;
    fd_set afds;    
    
    memset(buffer,0,BUFSIZE);

    if (argc!= 2)
    {
        fprintf(stderr,"Usage:%s portnumber\a\n", argv[0]);
        exit(1);
    }

    portnumber = atoi(argv[1]);
    
    if (portnumber<0)
    {
        fprintf(stderr,"Usage:%s portnumber\a\n",argv[0]);
        exit(1);
    }

    if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
        fprintf(stderr,"Socket error:%s\n\a",strerror(errno));
        exit(1);
    }
    
    bzero(&server_addr,sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(portnumber);

    if (bind(sockfd,(struct sockaddr*)(&server_addr),sizeof(struct sockaddr)) == -1)
    {
        fprintf(stderr,"Bind error:%s\n\a",strerror(errno));
        exit(1);
    }

    if (listen(sockfd,5) == -1)
    {
        fprintf(stderr,"Listen error:%s\n\a",strerror(errno));
        exit(1);
    }

    sin_size=sizeof(struct sockaddr_in);
    nfds = getdtablesize();
    FD_ZERO(&afds);
    FD_SET(sockfd,&afds);

    while(1)
    {
        memcpy(&rfds,&afds,sizeof(rfds));
    if (select(nfds,&rfds,(fd_set*)0,(fd_set*)0,(struct timeval*)0) < 0)
    {
            fprintf(stderr,"select:%s\n",strerror(errno));
            exit(1);
    }
        if (FD_ISSET(sockfd,&rfds))
        {
            if ((new_fd=accept(sockfd,(struct sockaddr *)(&client_addr),&sin_size))==-1)
            {
                fprintf(stderr,"Accept error:%s\n\a",strerror(errno));
                exit(1);
            }
                    
            fprintf(stderr,"Server get connection from %s\n",
            inet_ntoa(client_addr.sin_addr));
            
            FD_SET(new_fd,&afds);
        }
        
        for (fd = 0;fd<nfds;++fd )
        {
            if (fd != sockfd&&FD_ISSET(fd,&rfds))
            {
                if(echo(fd)==0)
                {
                    (void)close(fd);
                    FD_CLR(fd,&afds);
                }    
            }    
    }
    }
/*        switch(fork())
        {
            case 0:
            {
                (void)close(sockfd);
                exit(echo(new_fd));
            }
            default:
            {
                (void)close(new_fd);
                break;
            }
            case -1:
            {
                fprintf(stderr,"fork:%s\n",strerror(errno));
                exit(1);    
            }    
        }        
    }
  close(new_fd);
    close(sockfd);
    exit(0); */
}

int echo(int fd)
{
    char buf[BUFSIZE];
    int cc;

    cc = read(fd,buf,sizeof(buf));
    if (cc<0)
    {
            fprintf(stderr,"echo read:%s\n",strerror(errno));
            exit(1);
    }
    fprintf(stderr,"cc = %d\n",cc);
    fprintf(stderr,"buf = %s\n",buf);
        
    if (cc&&write(fd,buf,cc)<0)
    {
            fprintf(stderr,"echo read:%s\n",strerror(errno));
            exit(1);
    }        
    return cc;    
}
