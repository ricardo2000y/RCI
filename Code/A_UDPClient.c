// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
   
#define PORT     8080
#define MAXLINE 1024
#define SA_in struct sockaddr_in

void func(int sockfd, int mode,struct sockaddr_in *servaddr){
	char buff[MAXLINE];
	int n;
    socklen_t len = sizeof(*servaddr);
	bzero(buff, sizeof(buff));
	if(mode == 0) {
		printf("Enter the string : ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n')
			;
		sendto(sockfd, (const char *)buff, strlen(buff),
        MSG_CONFIRM, (const struct sockaddr *) servaddr, 
            len);
    printf("Hello message sent.\n");
	}
	else{
         n = recvfrom(sockfd, (char *)buff, MAXLINE, 
                MSG_WAITALL, (struct sockaddr *) servaddr,
                &len);
        buff[n] = '\0';
        printf("Server : %s\n", buff);

    }
    
}
void send_recieve_udp_message(bool mode,int sock_fd, SA_in* servaddr, char* buff){
	int n;
    socklen_t len = sizeof(*servaddr);
    if(mode){
        sendto(sockfd, buff, strlen(buff),
            MSG_CONFIRM, (const struct sockaddr *) servaddr, len);
        n = recvfrom(sockfd, buff, MAXLINE, 
            MSG_WAITALL, (struct sockaddr *) servaddr,&len);
        //* here we are recieving an ACK size is 3 or 4 so not going to use buff (?)
    
    }else{
        n = recvfrom(sockfd, buff, MAXLINE, 
        MSG_WAITALL, (struct sockaddr *) servaddr,&len);
        sendto(sockfd,"ACK\0", strlen("ACK\0"),
            MSG_CONFIRM, (const struct sockaddr *) servaddr, len);
        //* here we are SENDING an ACK size is 3 or 4 so not going to use buff (?)
    }
}
//todo mudar o nome para algo que diga explicitamente UDP_chord
void init_udp_client(int *sockfd,SA_in*servaddr, char*port_ ,char* addr/*aqui addiciona-se a info do server*/){
     // Creating socket file descriptor
    in_addr_t  ip;
    if ( (*sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    // Filling server information
    servaddr->sin_family = AF_INET;
    int port = strtol(port_, NULL, 10);
    servaddr->sin_port = htons(port);

    inet_pton( AF_INET, addr, (in_addr_t*) &servaddr->sin_addr.s_addr );
    
}

// Driver code
int main(int argc, char* argv[]) {
    if (argc != 4) {
        printf("to run client supply ring key, IP and port, separated by a single space \n");
        exit(0);
    } 
    int sockfd;
    char buffer[MAXLINE];
    char *hello = "Hello from client";
    //struct sockaddr_in     servaddr;
    SA_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    init_udp_client(&sockfd/*&chord_fd*/,&servaddr/*&chord_addr*/,/*chord.port*/ argv[3],/*chord.IP*/argv[2]);
    
    while(1){
        func(sockfd,0,&servaddr);
        func(sockfd,1,&servaddr);
    }
    close(sockfd);
    return 0;
}
    