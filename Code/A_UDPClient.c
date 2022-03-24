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
// Driver code
int main() {
    int sockfd;
    char buffer[MAXLINE];
    char *hello = "Hello from client";
    struct sockaddr_in     servaddr;
   
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
   
    memset(&servaddr, 0, sizeof(servaddr));
       
    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    inet_pton( AF_INET, "127.0.0.1", & servaddr.sin_addr);
    func(sockfd,0,&servaddr);
    func(sockfd,1,&servaddr);
    close(sockfd);
    return 0;
}