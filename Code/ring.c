#include "header.h"

/* creates the socket to with the corect type (true = dgram, false =stream) and checks if the creation was sucessful */ 
void criar_socket(int *sock_fd, bool mode){/*true = dgram false = stream*/
    if(mode)
        *sock_fd = socket(AF_INET, SOCK_DGRAM, 0);   //creates the socket
    else  *sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock_fd == -1){                         //checks if the creation was sucessful
	    perror("socket{} ");   
	    exit(-1);
    }

}
/*faz o fgets e obtém o command*/
void get_info_from_client(int * command, char* fcommand){
    int n=0;
    while ((fcommand[n++] = getchar()) != '\n')
    fcommand[n] ='\0';
    printf("\n\n  %s\n", fcommand);

    *command = fcommand[0];
    
}

/*Checks if IP and PORT are valid*/
void valid_IP_nd_port(char* IP, char* PORT, bool* ip_validator, bool* port_validator){
    int  check;
    SA_in * ipvalidation;
    ipvalidation = (SA_in*) malloc(sizeof(SA_in));
    *ip_validator =inet_pton( AF_INET, IP, ipvalidation );
    free(ipvalidation);
    check = strtol(PORT, NULL, 10);
    if(check <65536 && check > 0) *port_validator = true;
    else *port_validator = false;
    
}

void copy_node_info(node *origin, node *copy){
    copy->IP= origin->IP;
    copy->key= origin->key;
    copy->PORT = origin->PORT;
}

void new(node *me, node *pred, node *succ){
    copy_node_info(me,pred);
    copy_node_info(me,succ);
    printf("%s\n %s\n %s\n",  pred->key,pred->IP,pred->PORT);
    /*start tcp client and server(connect one to the other is unecessary(?)(have a variable knowing node is only one node)) + udp server*/
}

void split_command(char * fcommand, command_details_t* command_details){
    char* splitted;
    char* str;/*=(char*) malloc(sizeof(*fcommand)+1);*/
    str = strdup(fcommand);
    splitted = strsep(&str, " ");
    command_details->key=strdup (splitted);
    splitted = strsep(&str, " ");
    command_details->IP=strdup (splitted);
    splitted = strsep(&str, " ");
    command_details->PORT=strdup (splitted);
    printf("%s,%s,%s", command_details->key,command_details->key,command_details->PORT);
    free(str);
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        printf("to run client supply ring key, IP and port, separated by a single space \n");
        exit(0);
    } 
    bool valid_ip, valid_port;
    node me, pred,succ;
    command_details_t command_details;
    memset(&me,0,sizeof(node));
    memset(&pred, 0,sizeof(node));
    memset(&succ, 0,sizeof(node));
    memset(&command_details, 0,sizeof(command_details_t));
    printf("%d\n", argc);
    me.key= strdup(argv[1]);
    me.IP= strdup(argv[2]);
    me.PORT= strdup(argv[3]);
    
    valid_IP_nd_port(me.IP, me.PORT,&valid_ip, &valid_port);
    if (valid_ip && valid_port){
        printf("%s\n",me.key );   
        printf("%s\n", me.IP);
        printf("%s\n", me.PORT);
    }
    else printf("IP or Port provided is not valid");
    int command; 
    char* fcommand=(char*) malloc(sizeof(char) *100);
    
   get_info_from_client(&command, fcommand);
    printf("\n\n  %s\n", fcommand);
    if (command == 'n'){/* creates a new, empty ring where the node will ocupy the given key position per default */
        new(&me, &succ,&pred);
    }
        
    else if (command =='b' ){/*enters the ring knowing only one random node*/

    }
        
    else if (command == 'p' ){/*enters the ring know it's pred_key*/

    }
        
    else if ( command =='c' ){/*creates a udp shortcut to a given key, key_ip, key_port*/ 
        /*use strtok to split str into key/ip/port*/ 
        split_command(fcommand,&command_details);/*need to add a checker for valid command so it can exit nicely*/
    }
        
    else if( command == 'd' /*'e'*/){}/*problema porque e é short form para exit && echord prof tem que mudar*/
        
    else if(command =='s' ){

    }
        
    else if(command =='f' ){
        
    }
        
    else if(command =='l'){

    }
        
    else if(command =='e' ){
        /*exit => close all sockets and return 0*/
    }/*problema porque e é short form para exit && echord prof tem que mudar*/

    else{
        /*insert some stuff saying invalid command*/
    }
        
    return 0;    
}
