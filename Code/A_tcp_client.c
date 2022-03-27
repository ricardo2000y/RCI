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

void tcp_message(int *sockfd, int mode, char** message)
{
	
	if(mode == 0) {
			write(*sockfd, *message, 100000);
	}
	else{
		read(*sockfd, *message, 100000);
		printf("From Server : %s", *message);
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

	int sockfd ;
	SA_in servaddr;
	size_t len;
	char* buff= (char*)malloc(sizeof(char)*100);
	
	if (argc != 4) {
        printf("To run client supply ring key, IP and port, separated by a single space \n");
        exit(0);
    } 
	memset (&servaddr, 0,sizeof(servaddr));
	//int true = 1;
	
	// function for chat
	while(1){
		init_tcp_client(&sockfd,&servaddr,argv[3],argv[2] );
		getline(&buff,&len,stdin);
		buff[strlen(buff)-1]= '\0';
		func(&sockfd,0, &buff);
		func(&sockfd,1,&buff);
		printf("\n%d\n",sockfd);
		close(sockfd);
	}

	// close the socket
	close(sockfd);
}
