#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX 80
#define PORT 8080
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


int main(int argc, char* argv[])
{
	int sockfd, connfd;
	SA_in servaddr, cli;
	if (argc != 4) {
        printf("To run client supply ring key, IP and port, separated by a single space \n");
        exit(0);
    } 
	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		printf("Socket creation failed.\n");
		exit(0);
	}
	else
		printf("Socket successfully created.\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign socket
	servaddr.sin_family = AF_INET;
	if (inet_pton( AF_INET, argv[2], & servaddr.sin_addr) < 1) {
		//inet_pton - convert IPv4 and IPv6 addresses from text to binary form 
		// returns 0 if given adress isn't valid 
		printf("Adress not valid.\n");
		exit(-1);
  	}
	int port=strtol(argv[3], NULL, 10);
	//servaddr.sin_addr.s_addr = inet_addr(argv[2]);
	servaddr.sin_port = htons(port);

	// connect the client socket to server socket
	if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		printf("Connection with the server failed.\n");
		exit(0);
	}
	else
		printf("Connected to the server.\n");

	// function for chat
	while(1){
		func(sockfd,0);
		func(sockfd,1);
	}
	

	// close the socket
	close(sockfd);
}
