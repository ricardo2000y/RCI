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

void func(int *sockfd, int mode, SA_in *servaddr, char** message)
{
	//char buff[MAX];
	//int n;
	
	if(mode == 0) {
		if(*sockfd==-1){
			// connect the client socket to server socket
			if (connect(*sockfd, (SA*)servaddr, sizeof(*servaddr)) != 0) {
				printf("Connection with the server failed.\n");
				exit(0);
			}
			else
				printf("Connected to the server.\n");
			printf("Enter the string : ");
		}
			write(*sockfd, *message, sizeof(*message));
	}
	else{
		read(*sockfd, *message, sizeof(*message));
		printf("From Server : %s", *message);
		//if ((strncmp(buff, "exit", 4)) == 0) {
		//	printf("Client Exit...\n");
			
	}
}

void init_tcp_client(int *sockfd,SA_in*servaddr, char * port_, char*addr){
    if ( (*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
        perror("socket creation failed");
        exit(0);
    }
    // Filling server information
    servaddr->sin_family = AF_INET;
    int port = strtol(port_, NULL, 10);
    servaddr->sin_port = htons(port);
		if (inet_pton( AF_INET, addr,  (in_addr_t*) &servaddr->sin_addr.s_addr) < 1) {
		//inet_pton - convert IPv4 and IPv6 addresses from text to binary form 
		// returns 0 if given adress isn't valid 
		printf("Adress not valid.\n");
		exit(-1);
  	} 
	  if (connect(*sockfd, (SA*)servaddr, sizeof(*servaddr)) != 0) {
				printf("Connection with the server failed.\n");
				exit(0);
			}
			else
				printf("Connected to the server.\n");
}


int main(int argc, char* argv[])
{

	int *sockfd , n;
	SA_in servaddr;
	char* buff= (char*)malloc(sizeof(char)*MAX);
	
	if (argc != 4) {
        printf("To run client supply ring key, IP and port, separated by a single space \n");
        exit(0);
    } 
	memset (&servaddr, 0,sizeof(servaddr));
	sockfd= malloc(sizeof(int));
	//int true = 1;
	
	// function for chat
	while(1){
		init_tcp_client(sockfd,&servaddr,argv[3],argv[2] );
		n=0;
		bzero(buff, sizeof(*buff));
		while ((buff[n++] = getchar()) != '\n')
			;
		func(sockfd,0,&servaddr, &buff);
		func(sockfd,1,&servaddr,&buff);
		close(*sockfd);
		
		
		
	}
	

	// close the socket
	close(*sockfd);
}
