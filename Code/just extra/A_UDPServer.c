// Server side implementation of UDP client-server model
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
#define SA struct sockaddr
#define SA_in struct sockaddr_in
#define max(A,B) ((A)>=(B)?(A):(B))


void  init_tcp_server(int *udp_fd){
	SA_in servaddr;
// Creating socket file descriptor
    if ( (*udp_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
       
    // Filling server information
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);
       
    // Bind the socket with the server address
    if ( bind(*udp_fd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}

// Driver code
int main() {
    int udp_fd, connfd, newsockfd, afd=0, maxfd, counter;
    char buffer[MAXLINE];
    char *hello = "Hello from server";
    struct sockaddr_in servaddr, cliaddr;

  
	socklen_t len;
	memset(&cliaddr, 0, sizeof(cliaddr));
	fd_set rfds;
	enum {idle,busy} state;
	init_tcp_server(&udp_fd);
       
    
       
	/*//!Codigo Select
	state = idle;
	while (1) {

		FD_ZERO(&rfds);
		switch(state){
			case idle: 
				FD_SET(udp_fd, &rfds); 
				maxfd = udp_fd; 
				break;
			case busy: 
				FD_SET(udp_fd, &rfds); 
				FD_SET(afd, &rfds); 
				maxfd = max(udp_fd, afd); 
				break;
		}

		//number of file descriptors ready
		//select blocks until one of the file descriptors in rfds is ready to read
		counter = select(maxfd+1, &rfds, (fd_set*)NULL, (fd_set*)NULL, (struct timeval *)NULL);
		
		if(counter<=0){
			printf("There are no files ready to read.\n");
			exit(1);
		}

		for(;counter;--counter){
			switch(state){
				case idle: 
					if(FD_ISSET(udp_fd,&rfds)) {
						FD_CLR(udp_fd,&rfds);
						len = sizeof(servaddr);
						
						if((newsockfd=accept(udp_fd,(const struct sockaddr *)&servaddr,&len))==-1){
							printf("Server accept failed.\n");
							exit(1);
						}
						
						afd = newsockfd;
						state = busy;
					}
					break;
				case busy:
					if(FD_ISSET(udp_fd, &rfds)){
						FD_CLR(udp_fd, &rfds);
						if((newsockfd = accept(udp_fd, (const struct sockaddr *)&servaddr, &len)) == -1){
							printf("Server accept failed.\n");
							exit(1);
						}
						write(newsockfd,"Busy\n",sizeof("Busy\n"));
						close(newsockfd);
					
					} else if(FD_ISSET(afd, &rfds)){
						FD_CLR(afd, &rfds);
						if((n = read(afd, buffer, MAXLINE)) != 0){
							if(n == -1){
								printf("Reading error.\n");
								exit(1);
							}
							write(afd,buffer,sizeof(buffer));
						}else{
							close(afd);
							state = idle;
						}

					}
					break;		
			}
		}
	}
*/


    
    int  n;
   
    len = sizeof(SA_in);  //len is value/resuslt
   while(1){
		n = recvfrom(udp_fd, (char *)buffer, MAXLINE, 
					MSG_WAITALL, ( struct sockaddr *) &cliaddr,
					&len);
		buffer[n] = '\0';
		printf("Client : %s\n", buffer);
		sendto(udp_fd, (const char *)hello, strlen(hello), 
			MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
				len);
		printf("Hello message sent.\n"); 
   }   
    return 0;
}