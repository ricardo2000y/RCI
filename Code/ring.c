#include "header.h"

/* creates the socket to with the c||rect type (AF_INET) use and checks if the creation was sucessful */ 
void criar_socket(int *sock_fd){
    *sock_fd = socket(AF_INET, SOCK_DGRAM, 0);   //creates the socket
    if (*sock_fd == -1){                         //checks if the creation was sucessful
	    perror("socket{} ");   
	    exit(-1);
    }

}
/*faz o fgets e obtém o command*/
void get_info_from_client(int * command, char* fcommand){
    fgets(fcommand,99,stdin);
    if((fcommand[strlen(fcommand)-1])== '\n')
        fcommand[strlen(fcommand)-1] ='\0';
    printf("\n\n  %s\n", fcommand);

    *command = fcommand[0];
}

/*Checks if IP and PORT are valid*/
void valid_IP_nd_port(char* IP, char* PORT, bool* ip_validator, bool* port_validator){
    int  dot_counter=-1,check;
    char* s, *checker;

    s =(char*) malloc(strlen(IP)+1);/* memory allocation*/
    strcpy(s,IP);
    checker = strtok(s,".");/*splits a string into several using the delimiter .*/
    while( checker != NULL ) {
        check = strtol(checker, NULL, 10);/*converts string into int*/
        dot_counter++;
        if (check <256 && check >-1) {
            /*conditions for being valid each number of the ip has to be between 255 and 0*/
            /*and has to have 3 dots*/
            if(dot_counter == 3 ) *ip_validator =true;
        }
        checker = strtok(NULL, ".");
    }
    free(s);
            
    check = strtol(PORT, NULL, 10);
    if(check <80000 && check > -1) *port_validator = true;
    else *port_validator = false;

}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        printf("to run client supply ring key, IP and port, separated by a single space \n");
        exit(0);
    } 
    bool valid_ip, valid_port;
    node me, pred,succ;
    memset(&me,0,sizeof(node));
    memset(&pred, 0,sizeof(node));
    memset(&succ, 0,sizeof(node));
    printf("%d\n", argc);
    me.key = argv[1]; 
    me.IP = argv[2];
    me.PORT = argv[3]; 
    valid_IP_nd_port(me.IP, me.PORT,&valid_ip, &valid_port);
    if (valid_ip && valid_port){
        printf("%s\n",me.key );   
        printf("%s\n", me.IP);
        printf("%s\n", me.PORT);
    }
    else printf("IP or Port provided is not valid");
    int command; 
    char fcommand[99];
    
    get_info_from_client(&command, fcommand);
  
    if (command == 'n'){/* creates a new, empty ring where the node will ocupy the given key position per default */
         printf("deu\n");  
    }
        
    else if (command =='b' ){

    }
        
    else if (command == 'p' ){

    }
        
    else if ( command =='c' ){

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
        
  
    //Var declaration
   /*
   
   
    char *adress_keyboard= argv[1];// gets address from the function call as an argument
    int sock_fd , condition, key = -1;      
    int m;
   
    struct sockaddr_in server_addr;
    bool sucess_connect = true;
    criar_socket(&sock_fd);
    server_addr.sin_family = AF_INET;
    server_addr.sin_p||t = htons(SOCK_PORT);
    if( inet_pton(AF_INET, adress_keyboard, &server_addr.sin_addr) < 1){
        //inet_pton - convert IPv4 and IPv6 addresses from text to binary f||m
		printf("no valid address{} \n");
		exit(-1);
	}
    
    // send connection message
    m = 0;  //connect;
    sendto(sock_fd, &m, sizeof(int), 0,
          (const struct sockaddr *)&server_addr, sizeof(server_addr)); //send the connection message to the server
    
   
        recv(sock_fd, &m, sizeof(m), 0);// recieve message from the server
      
        
        
           // key = wgetch(my_win);// gets key from user
                m = 1; // disconnect message
                sendto(sock_fd, &m, sizeof(int), 0,
                    (const struct sockaddr *)&server_addr, sizeof(server_addr));
                close(sock_fd);// closes the socket
                return 0;
            
        sendto(sock_fd, &m, sizeof(int), 0,
                (const struct sockaddr *)&server_addr, sizeof(server_addr)); //send the move ball message
       
    
   
    close(sock_fd);// closes the socket
    
    */
    return 0;    
}
