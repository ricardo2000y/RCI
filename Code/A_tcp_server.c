#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr
#define SA_in struct sockaddr_in
#define max(A,B) ((A)>=(B)?(A):(B))
// Function designed for chat between client and server.
void func(int connfd)
{
	char buff[MAX];
	int n;
	// infinite loop for chat
    bzero(buff, MAX);	
    // read the message from client and copy it in buffer
    read(connfd, buff, sizeof(buff));
    // print buffer which contains the client contents
    printf("From client: %s\t To client : ", buff);
    bzero(buff, MAX);
    n = 0;
    // copy server message in the buffer
    while ((buff[n++] = getchar()) != '\n')
        ;

    // and send that buffer to client
    write(connfd, buff, sizeof(buff));

    // if msg contains "Exit" then server exit and chat ended.
    /*if (strncmp("exit", buff, 4) == 0) {
        printf("Server Exit...\n");
		close(connfd);
		exit(0);
    }*/
	
}

void  init_tcp_server(int *listen_fd){
	SA_in servaddr;
	*listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (*listen_fd == -1) {
		printf("Socket creation failed.\n");
		exit(0);
	}
	else
		printf("Socket successfully created.\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// Binding newly created socket to given IP and verification
	if ((bind(*listen_fd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
		printf("Socket bind failed.\n");
		exit(0);
	}
	else
		printf("Socket successfully binded.\n");

	// Now server is ready to listen and verification
	if ((listen(*listen_fd, 5)) != 0) {
		printf("Listen failed.\n");
		exit(0);
	}
	else
		printf("Server listening.\n");
}

// Driver function

int main()
{
	int listen_fd, connfd, newsockfd, tcp_fd=0, maxfd, counter, n;
	socklen_t len;
	SA_in cli;
	fd_set rfds;
	enum {idle,busy} state;
	char buff[MAX];
	//int listen_fd;
	init_tcp_server(&listen_fd);
	// socket create and verification
	len = sizeof(SA_in);

//! Codigo select
/*
	state = idle;
	while (1) {

		FD_ZERO(&rfds);
		switch(state){
			case idle: 
				FD_SET(listen_fd, &rfds);
				maxfd = listen_fd; 
				break;
			case busy: 
				FD_SET(listen_fd, &rfds); 
				FD_SET(tcp_fd, &rfds); 
				maxfd = max(listen_fd, tcp_fd); 
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
					if(FD_ISSET(listen_fd,&rfds)) {
						FD_CLR(listen_fd,&rfds);
						len = sizeof(SA_in);
						
						if((newsockfd=accept(listen_fd,(SA*)&cli,&len))==-1){
							printf("Server accept failed.\n");
							exit(1);
						}
						
						tcp_fd = newsockfd;
						state = busy;
					}
					break;
				case busy:
					if(FD_ISSET(listen_fd, &rfds)){
						FD_CLR(listen_fd, &rfds);
						if((newsockfd = accept(listen_fd, (SA*)&cli, &len)) == -1){
							printf("Server accept failed.\n");
							exit(1);
						}
						write(newsockfd,"Busy\n",sizeof("Busy\n"));
						close(newsockfd);
					
					} else if(FD_ISSET(tcp_fd, &rfds)){
						FD_CLR(tcp_fd, &rfds);
						if((n = read(tcp_fd, buff, MAX)) != 0){
							if(n == -1){
								printf("Reading error.\n");
								exit(1);
							}
							write(tcp_fd,buff,sizeof(buff));
						}else{
							close(tcp_fd);
							state = idle;
						}

					}
					break;		
			}
		}
	}
	
*/
	// Accept the data packet from client and verification
	connfd = accept(listen_fd, (SA*)&cli, &len);
	if (connfd < 0) {
		printf("Server accept failed.\n");
		exit(0);
	}
	else
		printf("Server accept the client.\n");

	// Function for chatting between client and server
	while(1){
		func(connfd);
	}
	

	// After chatting close the socket
	close(listen_fd);
}
