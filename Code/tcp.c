#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 500
#define PORT 6969

void initialize_hints_and_getaddrinfo(struct addrinfo* hints){
    memset(hints, 0, sizeof(struct addrinfo));
    hints->ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints->ai_socktype = SOCK_STREAM; /* STREAM socket */
    hints->ai_flags = AI_PASSIVE;    /* For wildcard IP address */
    hints->ai_protocol = 0;          /* Any protocol */
    hints->ai_canonname = NULL;
    hints->ai_addr = NULL;
    hints->ai_next = NULL;
/*
    *s = getaddrinfo(NULL, argv[1], hints, &result);// not sure if *s or &s idk to check

    if (*s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(*s));
        //exit(EXIT_FAILURE);
        }*/
}

void start_tcp_client(struct addrinfo *result,struct addrinfo *rp, int* cfd){
    /* getaddrinfo() returns a list of address structures.
    Try each address until we successfully connect(2).
    If socket(2) (or connect(2)) fails, we (close the socket
    and) try the next address. */
    
    for (rp = result; rp != NULL; rp = rp->ai_next) {// ppbly wrong atm pointers shit

        *cfd = socket(rp->ai_family, rp->ai_socktype,
                rp->ai_protocol);
    if (*cfd == -1)
        continue;

    if (connect(*cfd, rp->ai_addr, rp->ai_addrlen) != -1)
        break;                  /* Success */

    
    }
   

}
void start_tcp_server(struct addrinfo *result,struct addrinfo *rp, int* sfd){
        /* getaddrinfo() returns a list of address structures.
        Try each address until we successfully bind(2).
        If socket(2) (or bind(2)) fails, we (close the socket
        and) try the next address. */
    
    for (rp = result; rp != NULL; rp = rp->ai_next) {
        *sfd = socket(rp->ai_family, rp->ai_socktype,
                rp->ai_protocol);
        if (*sfd == -1)
            continue;

        if (bind(*sfd, rp->ai_addr, rp->ai_addrlen) == 0)
            break;                  /* Success */

    }

    if (rp == NULL) {               /* No address succeeded */
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }

    

}
/*mode 0= read mode 1= write*/
void message_tcp_client(int client_socket ,int mode){
     /* Send remaining command-line arguments as separate
        datagrams, and read responses from server */
    int valread;
    char buf[BUF_SIZE];

    
    if (mode== 1){
        send(client_socket , "hello" , strlen("hello") , 0 );
        printf("Hello message sent\n");
    }
    else{
        valread = read( client_socket , buf, sizeof(buf));
        printf("%s\n",buf );
    }
    
    
    //for (j = 3; j < argc; j++) {
        //len = strlen(argv[j]) + 1;
                /* +1 for terminating null byte * /

        if (len > BUF_SIZE) {
            fprintf(stderr,
                    "Ignoring long message in argument %d\n", j);
            continue;
        }

        if (write(sfd, argv[j], len) != len) {
            fprintf(stderr, "partial/failed write\n");
            exit(EXIT_FAILURE);
        }

        nread = read(sfd, buf, BUF_SIZE);
        if (nread == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        
        nread = read(sfd, buf, BUF_SIZE);
        if (nread == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }

        printf("Received %zd bytes: %s\n", nread, buf);
    }*/

}
/*mode 0= read mode 1= write*/
void message_tcp_server(int server_socket){
    int  s;
    //struct sockaddr_storage peer_addr;
    //socklen_t peer_addr_len;
    int valread;
    char buf[BUF_SIZE];

    valread = read( server_socket , buf, sizeof(buf));
    printf("%s\n",buf );
    send(server_socket , "hello" , strlen("hello") , 0 );/*just testing hello XD */
    printf("Hello message sent\n");
   /* for (;;) {
        
        peer_addr_len = sizeof(struct sockaddr_storage);
        nread = recvfrom(server_socket, buf, BUF_SIZE, 0,
                (struct sockaddr *) &peer_addr, &peer_addr_len);
        if (nread == -1)
            continue;               /* Ignore failed request * /
        char host[NI_MAXHOST], service[NI_MAXSERV]; s = getnameinfo((struct sockaddr *) &peer_addr,
                        peer_addr_len, host, NI_MAXHOST,
                        service, NI_MAXSERV, NI_NUMERICSERV);
        if (s == 0){
            printf("Received %zd bytes from %s:%s\n",
                    nread, host, service);
            printf("message was: %s\n", buf);
        }
        else
            fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));

        if (sendto(server_socket, buf, nread, 0,
                    (struct sockaddr *) &peer_addr,
                    peer_addr_len) != nread)
            fprintf(stderr, "Error sending response\n");
    }*/

}

int main(/*int argc, char *argv[]*/)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int sfd,message_s,s,cfd;
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    ssize_t nread;
    char buf[BUF_SIZE];
    //size_t len;

    initialize_hints_and_getaddrinfo(&hints);
    s = getaddrinfo(NULL, "1666", &hints, &result);// not sure if *s or &s idk to check

    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
        }
    start_tcp_server(result,rp,&sfd);
      s = getaddrinfo("127.0.0.1", "1666", &hints, &result);// not sure if *s or &s idk to check

    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
        }
    
    start_tcp_client(result,rp,&cfd);
    if (listen(sfd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
     if((message_s = accept(sfd, (struct sockaddr *)&peer_addr, 
                       (socklen_t*)&peer_addr_len))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(result);   
    fprintf(stdout, "is it here\n");  
   
    message_tcp_client(sfd,0);
    fprintf(stdout, "or here\n");  
    message_tcp_server(message_s);
    message_tcp_client(sfd,1);

   
}


