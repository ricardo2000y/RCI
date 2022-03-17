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
    hints.ai_flags= AI_PASSIVE;
    errcode=getaddrinfo(NULL,PORT,&hints,&res);
    if(errcode!=0)/*error*/exit(1);

    if(bind(fd,res->ai_addr,res->ai_addrlen)==-1)/*error*/exit(1);

    while(1){
        addrlen=sizeof(addr);
        n=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
        if(n==-1)/*error*/exit(1);
        write(1,"recieved\n",10); write(1,buffer,n);
        n=sendto(fd,buffer,n,0,(struct sockaddr*)&addr, addrlen);
        if(n==-1)/*error*/exit(1);
        
    }
    
    
    /**/
    fd= socket(AF_INET,SOCK_DGRAM,0);
    if(fd==-1)/*error*/exit(1);

    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET;//IPv4
    hints.ai_socktype=SOCK_DGRAM;//UDP socket

    errcode=getaddrinfo("127.0.0.1",PORT,&hints,&res);
    if(errcode!=0)/*error*/exit(1);

    n=sendto(fd,"mensagem\n",n,0,(struct sockaddr*)&addr, addrlen);
        if(n==-1)/*error*/exit(1);
    addrlen= sizeof(addr);
    n=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
        if(n==-1)/*error*/exit(1);
    write(1,"recieved\n",10); write(1,buffer,n);// falta mudar aqui
    
    /**/
    char in_str[128];

    fd_set inputs, testfds;
    struct timeval timeout;

    int i,out_fds,n;

    FD_ZERO(&inputs); // Clear inputs
    FD_SET(0,&inputs); // Set standard input channel on

    printf("Size of fd_set: %d\n",sizeof(fd_set));
    printf("Value of FD_SETSIZE: %d\n",FD_SETSIZE);

    while(1)
    {
        testfds=inputs;
        timeout.tv_sec=10;
        timeout.tv_usec=0;

        printf("testfds byte: %d\n",((char *)&testfds)[0]);

        out_fds=select(FD_SETSIZE,&testfds,(fd_set *)NULL,(fd_set *)NULL,&timeout);

        printf("Time = %d and %d\n",timeout.tv_sec,timeout.tv_usec);

        printf("testfds byte: %d\n",((char *)&testfds)[0]);

        switch(out_fds)
        {
        case 0:
            printf("Timeout event\n");
            break;
        case -1:
            perror("select");
            exit(1);
        default:
            if(FD_ISSET(0,&testfds))
            {
                if((n=read(0,in_str,127))!=0)
                {
                    if(n==-1)
                        exit(1);
                    in_str[n]=0;
                    printf("From keyboard: %s\n",in_str);
                }
            }

        }

    }

    freeaddrinfo(res);
    close(fd);
    exit(0);
}
