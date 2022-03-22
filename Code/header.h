#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>  
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h> 
#include <stdio.h>

#define MAX_KEY 32
#define SOCK_PORT 6969
#define SA struct sockaddr
#define SA_in struct sockaddr_in
typedef struct {
    /*struct sockaddr_in;*/
    char* key;
    char* IP;
    char* PORT;
} node,command_details_t;