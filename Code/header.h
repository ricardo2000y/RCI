
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
#include <sys/select.h>
#include <time.h>

#define MAX_KEY 32
#define SOCK_PORT 6969
#define SA struct sockaddr
#define SA_in struct sockaddr_in
#define max(A,B) ((A)>=(B)?(A):(B))
typedef struct {  
    char key[5];
    char IP[20];
    char PORT[10];
} node;
typedef struct {
    char command[10];
    char n[5];
    char searched_key[5];
    char key[5];
    char IP[16];
    char PORT[10];
}command_details_t;

typedef struct{
    char message[100];
    SA_in addr;
    socklen_t addr_len;
}message_info_t;


