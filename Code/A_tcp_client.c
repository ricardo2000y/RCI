#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>


#define MAX 80
#define PORT 12
#define SA struct sockaddr
#define SA_in struct sockaddr_in

void func(int sockfd, int mode)
{
	char buff[MAX];
	int n;
	bzero(buff, sizeof(buff));
	if(mode == 0) {
		printf("Enter the string : ");
		n = 0;
		while ((buff[n++] = getchar()) != '\n')
			;
		write(sockfd, buff, sizeof(buff));
	}
	else{
		read(sockfd, buff, sizeof(buff));
		printf("From Server : %s", buff);
		//if ((strncmp(buff, "exit", 4)) == 0) {
		//	printf("Client Exit...\n");
			
	}
}

void init_tcp_client(int *sockfd,SA_in*servaddr, char ** argv){
	in_addr_t  ip;
    if ( (*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
        perror("socket creation failed");
        exit(0);
    }
    // Filling server information
    servaddr->sin_family = AF_INET;
    int port = strtol(argv[3], NULL, 10);
    servaddr->sin_port = htons(port);
		if (inet_pton( AF_INET, argv[2],  (in_addr_t*) &servaddr->sin_addr.s_addr) < 1) {
		//inet_pton - convert IPv4 and IPv6 addresses from text to binary form 
		// returns 0 if given adress isn't valid 
		printf("Adress not valid.\n");
		exit(-1);
  	}
    // connect the client socket to server socket
	if (connect(*sockfd, (SA*)servaddr, sizeof(*servaddr)) != 0) {
		printf("Connection with the server failed.\n");
		exit(0);
	}
	else
		printf("Connected to the server.\n");
    
}


int main(int argc, char* argv[])
{
	int sockfd, connfd;
	SA_in servaddr, cli;
	if (argc != 4) {
        printf("To run client supply ring key, IP and port, separated by a single space \n");
        exit(0);
    } 
	memset (&servaddr, 0,sizeof(servaddr));
	init_tcp_client(&sockfd,&servaddr,argv );
	

	// function for chat
	while(1){
		func(sockfd,0);
		func(sockfd,1);
	}
	

	// close the socket
	close(sockfd);
}
