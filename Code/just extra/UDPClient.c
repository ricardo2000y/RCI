#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>


#define PORT "58001"

int main(void)
{
    /**/
    int fd,errcode;
    ssize_t n;
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr;
    char buffer[128];
    fd= socket(AF_INET,SOCK_DGRAM,0);
    if(fd==-1)/*error*/exit(1);

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;//IPv4
    hints.ai_socktype=SOCK_DGRAM;//UDP socket

    errcode=getaddrinfo("localhost",PORT,&hints,&res);
    if(errcode!=0)/*error*/exit(1);

    n=sendto(fd,"mensagem\n",n,0,(struct sockaddr*)&addr,addrlen);
        if(n==-1)/*error*/exit(1);
    
    n=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
        if(n==-1)/*error*/exit(1);
    write(1,"recieved\n",10); write(1,buffer,n);// falta mudar aqui

    return 0;
}