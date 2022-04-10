#include "header.h"
// types of comments
// !
// ?
// TODO:
// // 
// *

//todo buffer recieves several messages and do a while cycle to process such messages (split them on the \0 and work with a command at a time)
// * needs to have a check if complete command, if command is not complete it does another read ... etc this i know the logic to but it's extra as not explicitly needed
//! partial implementation fails if it reads like half a command
//! need to see how to implement that part

//! making quick resume of the connections so i don't lose myself on making the functions
/*
todo TCP

tcp_c_fd  //  *  connects to the pred 
sends:SELF
recieves: PRED,FND,RSP ---
//////////////////////////////////
accepted_fd //  * accepts a connected from a new client(aka new succ)
sends: nothing //?
recieves:SELF(then binds this to the tcp_s_fd)
//////////////////////////////////
tcp_s_fd//  * connected to the succ
sends:PRED, FND, RSP
recieves:nothing //?  

todo UDP
chord_fd
  s* connects to a chord if one exists
sends:FND,RSP,"ACK" //  * ACK IS ONLY WHEN IT RECIEVES A EPRED
recieves:"ACK",EPRED

udp_s_fd
sends:"ACK", EPRED
recieves:"ACK",FND,RSP, EFND//  * when EFND we need to store the addres (maybe store up to 5 addresses(?))

*/
//! making quick resume of the connections so i don't lose myself on making the functions


//! change key/ port in running time function 
//! on the find function check if the searched number is valid
//*- same for bentry


//! check for every user input if command is = to proper command or abreviation
 //todo help command
bool check_free_position(int* my_searches, bool mode,int* n){
    static int i = 4;
    static int k = -1;
    int count =100;
    
    if(mode){
        do{
            if (i == 99)
                i=4;
            i++;
            count++;
        }while((my_searches[i]!=-1)||(count ==200));
       *n =i;
       return((count==200 )&&(my_searches[i]!=-1));
    }
    else{
        do{
            if (k == 5)
                k=-1;
            k++;
            count++;
        }while((my_searches[k]!=-1)||(count ==110));
        *n =k;
        return((count==110 )&&(my_searches[i]!=-1));
    }     
}        
//Adds clients that connect for the first time to the server and saves their info in a array of clients
//? have to revise
void add_client(client_addr_t * clients_data,int n , SA_in client_addr) {
    inet_ntop( AF_INET, &client_addr.sin_addr, clients_data[n].addr, INET_ADDRSTRLEN );
    
    clients_data[n].port = (int) ntohs(client_addr.sin_port);
    printf("IP address: %s\n", clients_data[n].addr);

    printf("Port: %d\n", clients_data[n].port);

    /* Accept client request */
   /* int client_socket = accept(server_socket, 
        (struct sockaddr *)&client_addr, &client_len);

    char hoststr[NI_MAXHOST];
    char portstr[NI_MAXSERV];

    int rc = getnameinfo((struct sockaddr *)&client_addr, 
        client_len, hoststr, sizeof(hoststr), portstr, sizeof(portstr), 
        NI_NUMERICHOST | NI_NUMERICSERV);

    if (rc == 0) 
        printf("New connection from %s %s", hoststr, portstr);*/

}

// gets line from stdin given by user
void get_info_from_client( char* fcommand){
    size_t len = 1000; 
    getline(&fcommand,&len,stdin); // * gets a full line until '\n' is met
    fcommand[strlen(fcommand)-1]= '\0';//* removes the \n 
}

//prints the current status of the client with the relevant node/ring information 
void show_status(bool in_a_ring, node me, node succ, node pred, node chord,time_t uptime,int chord_fd,int tpc_c_fd,int tpc_s_fd){
    system("clear");
    printf("%-15s\t%-15s\t%-15s\n\n"," "," ","STATUS");
    printf("%-15s\t%-15s\t%-15s\t%-15s\t%-15s\n","     ", "KEY","IP","PORT","socket");
    printf("mine:       \t%-15s\t%-15s\t%-15s\n",me.key , me.IP,me.PORT); 
   if(in_a_ring){
        printf("sucessor:   \t%-15s\t%-15s\t%-15s\t%-15d\n",succ.key , succ.IP,succ.PORT,tpc_s_fd);
        printf("predecessor:\t%-15s\t%-15s\t%-15s\t%-15d\n",pred.key , pred.IP,pred.PORT,tpc_c_fd);
        if(chord_fd != 0) printf("atalho:       \t%-15s\t%-15s\t%-15s\t%-15d\n",chord.key , chord.IP,chord.PORT,chord_fd);
        printf("--------------------------------------------------------");
        printf("\nBeen in the ring for:%ld minutes and %ld seconds\n" , uptime/60,uptime%60);
    }
    
    if(!in_a_ring){
            printf("--------------------------------------------------------");
        printf("\nNot in a ring\n");
    }
    else if(strcmp(me.key, succ.key)==0){
        printf("\n1 node(me) in the ring\n");
    }else {
        if(strcmp(succ.key,pred.key)==0){
            printf("\n2 nodes in the ring\n");
        }
        else printf("\n3 or more, nodes in the ring\n");
    }
}

// checks the distance between the node and another node, returns the distance value( if mu_key =5 and dest key= 6 distance is 1)
int check_distance(char* origin, char* searched_key){
    int my_key_n = strtol(origin, NULL, 10);// * converts the my_key str into decimal
    int dest_key_n = strtol(searched_key, NULL, 10);// * converts the dest_key str into decimal
    int result = (dest_key_n-my_key_n)%MAX_KEY;// * determines the distance between the keys formula= (n-j)mode 32 
    if (result <0) result += MAX_KEY; //* if the distance is negative adds 32.
    return result;//* returns the distance value
}

//Checks if IP and PORT are valid
bool valid_IP_port_key(char* IP, char* PORT,char* key){
    bool port_validator=false, ip_validator=false,key_validator=false;// * control variables 
    int  check;
    char ipvalidation[20];
    check = strtol(key, NULL, 10);
    if(check <32 && check > -1) key_validator = true;
    ip_validator = (bool)inet_pton( AF_INET, IP, ipvalidation );// * inetpton returns 0 incase the given ip is not valid 
    check = strtol(PORT, NULL, 10);//* converts the port into decimal
    if(check <65536 && check > 0) port_validator = true;//* checks if the port is between PORT max and min values
    return (ip_validator && port_validator && key_validator);//* returns true if both IP and PORT are valid, false otherwise
}

// split the given string and stores it on a new string
void split_str_nd_copy_to_new_location(char** str_to_split,char* str_to_save){
    if(*str_to_split!=NULL){
        char*splitted = strsep(str_to_split, " "); 
        strcpy(str_to_save ,splitted);
    }
}

// splits the command given by user saving it in a structure while checking if the command is valid
bool split_command(char * fcommand, command_details_t* command_details){
    char* str_to_free ,*str_to_split;
    str_to_free=str_to_split=strdup(fcommand);
    if (str_to_split == NULL){ // * do routine for bad command 
        printf("%s given is incomplete or wrong", fcommand);
        free(str_to_free);
        return false;
    }
    split_str_nd_copy_to_new_location(&str_to_split,command_details->command);
    if (str_to_split == NULL){ // * do routine for bad command 
        printf("%s given is incomplete or wrong", fcommand);
        free(str_to_free);
        return false;
    }
    split_str_nd_copy_to_new_location(&str_to_split,command_details->key);
    if (str_to_split == NULL){ // * do routine for bad command 
        printf("%s given is incomplete or wrong", fcommand);
        free(str_to_free);
        return false;
    }
    split_str_nd_copy_to_new_location(&str_to_split,command_details->IP);
    if (str_to_split == NULL){ // * do routine for bad command 
        printf("%s given is incomplete or wrong", fcommand);
        free(str_to_free);
        return false;
    }
    split_str_nd_copy_to_new_location(&str_to_split,command_details->PORT);
    printf("%s\n%s\n%s\n%s\n",command_details->command, command_details->IP, command_details->key, command_details->PORT);
    if(str_to_split != NULL){// * do routine for bad command 
       printf("%s given is incomplete or wrong", fcommand);
        free(str_to_free);
        return false;
    }
    free(str_to_free);
    return true;
}

// splits the command given by user saving it in a structure while checking if the command is valid
bool split_find_command(char * fcommand, command_details_t* command_details){
    char* str_to_free ,*str_to_split;
    str_to_free=str_to_split=strdup(fcommand);
    split_str_nd_copy_to_new_location(&str_to_split,command_details->command);
    if (str_to_split == NULL){ // * do routine for bad command 
        printf("%s given is incomplete or wrong", fcommand);
        free(str_to_free);
        return false;
    }
    split_str_nd_copy_to_new_location(&str_to_split,command_details->searched_key);
    if (str_to_split != NULL){ // * do routine for bad command 
        printf("%s given is incomplete or wrong", fcommand);
        free(str_to_free);
        return false;
    }
    free(str_to_free);
    return true;
}

// copys the data from one node to other, useful when entering the ring, recieving new person or creating a ring/chord
void copy_node_info(node *origin, node *copy){
    strcpy(copy->IP,origin->IP);
    strcpy(copy->key,origin->key);
    strcpy(copy->PORT,origin->PORT);
}

void tcp_client(int *tcp_c_fd,SA_in *tcp_servaddr, char * port_, char*addr, char* message){
    if ( (*tcp_c_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
        perror("socket creation failed");
        exit(0);
    }
    // Filling server information
    tcp_servaddr->sin_family = AF_INET;
    int port = strtol(port_, NULL, 10);
    tcp_servaddr->sin_port = htons(port);
	if (inet_pton( AF_INET, addr,  (in_addr_t*) &tcp_servaddr->sin_addr.s_addr) < 1) {
		//inet_pton - convert IPv4 and IPv6 addresses from text to binary form 
		// returns 0 if given adress isn't valid 
		printf("Adress not valid.\n");
		exit(-1);
  	} 
    else{
        if (connect(*tcp_c_fd, (SA*)tcp_servaddr, sizeof(*tcp_servaddr)) != 0) {
		printf("Connection with the server failed.\n");
		
	}
	else
		printf("Connected to the server.\n");
        //? maybe no write here and just do it outside so it's explicit
        write(*tcp_c_fd,message, strlen(message));
    }  
	

}

//todo mudar o nome para algo que diga explicitamente UDP_chord
void init_udp_client(int* sockfd,  char* port_ch ,char* addr){
     // Creating socket file descriptor
    SA_in servaddr; 
    if ( (*sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    // Filling server information
    servaddr.sin_family = AF_INET;
    int port = strtol(port_ch, NULL, 10);
    servaddr.sin_port = htons(port);
    // establishing a timeout
    struct timeval tv;
    tv.tv_sec =0;
    tv.tv_usec =50000; // *0.05 segundos posso mudar o valor 
    setsockopt(*sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    //! insert the check for inet_pton
    if (inet_pton( AF_INET, addr, (in_addr_t*) &servaddr.sin_addr.s_addr) < 1) {
		//inet_pton - convert IPv4 and IPv6 addresses from text to binary form 
		// returns 0 if given adress isn't valid 
		
  	} 
    if (connect(*sockfd,(const SA*)&servaddr,sizeof(servaddr))==-1) {
        printf("failed connect udp_client.\n");
    }  
}

/*void send_recieve_udp_message(bool mode,int sock_fd, SA_in* servaddr, char* buff,size_t message_size){
	int n;
    socklen_t len = sizeof(*servaddr);
    
    

    if(mode){
        sendto(sock_fd, buff, strlen(buff),
            MSG_CONFIRM, (const struct sockaddr *) servaddr, len);
        n = recvfrom(sock_fd, buff, message_size, 
            MSG_WAITALL, (struct sockaddr *) servaddr,&len);
        // * here we are recieving an ACK size is 3 or 4 so not going to use buff (?)
    
    }else{
        n = recvfrom(sock_fd, buff, message_size, 
        MSG_WAITALL, (struct sockaddr *) servaddr,&len);
        sendto(sock_fd,"ACK", 4,
            MSG_CONFIRM, (const struct sockaddr *) servaddr, len);
        // * here we are SENDING an ACK size is 3 or 4 so not going to use buff (?)
    }
}*/

//routine for creating a new node, has to initialize the successor and predecessor nodes
// TODO: add more stuff need to check what is missing for new (pbbly had stuff to the select mask (the new sockets from the udp/tcp(listen_sd) servers))
// * most likely start a tcp/udp server listening to possible connections 
void new(node *me, node *pred, node *succ,node* temp_node){
    copy_node_info(me,pred);// * initialize predecessor = me
    copy_node_info(me,succ);// * initialize successor = me
    copy_node_info(me,temp_node);
}

void bentry_routine(char* fcommand, command_details_t* command_details,int* chord_fd,char*buff,char* key){
    if(split_command(fcommand,command_details)){
        if((strcmp(command_details->command,"bentry")==0)||(strcmp(command_details->command,"b")==0)){
             sprintf(buff,"%s %s","EFND", key);
            init_udp_client(chord_fd,command_details->PORT,command_details->IP);
            
            send(*chord_fd,buff,strlen(buff),MSG_CONFIRM);
            recv(*chord_fd,buff,4,0);
            if(*buff!='A'){
                printf("No ACK, retry bentry");
                close(*chord_fd);
                *chord_fd=0;
            }
        }
        else printf("bad command\n");
    }

}

//routine for pentry splitting the command till sending the SELF
void pentry(char* fcommand,command_details_t* command_details,node* pred,node* me,char* buff,int* tcp_c_fd,SA_in* tcp_servaddr){
    if(split_command(fcommand,command_details)){
        if((strcmp(command_details->command,"pentry")==0)||strcmp(command_details->command,"p")){
            strcpy(pred->IP,command_details->IP);
            strcpy(pred->key,command_details->key);
            strcpy(pred->PORT,command_details->PORT);
            sprintf(buff,"%s %s %s %s\n","SELF", me->key,me->IP,me->PORT);
            tcp_client(tcp_c_fd,tcp_servaddr,pred->PORT,pred->IP,buff);
        }
        else printf("bad command\n");
    }   
}

void leave_ring(node me, node* succ, node* pred, char* buff,int* chord_fd,int* tcp_c_fd, int* accepted_socket, int* tcp_s_fd ,fd_set* mask_copy,int* udp_fd, int* listen_fd, bool* locked){
    //leave_ring(me,&succ,&pred,buff,&chord_fd,&tcp_c_fd,&accepted_socket,&tcp_s_fd);
    if(strcmp(me.key, succ->key)!=0){
        sprintf(buff,"%s %s %s %s\n","PRED", pred->key,pred->IP,pred->PORT);
        write(*tcp_s_fd,buff,strlen(buff));
    }
    memset(pred, 0,sizeof(node));
    memset(succ, 0,sizeof(node));
    //todo put close all sockets inside a func 
    //? pbbly unecessary
    if (*chord_fd){
        FD_CLR(*chord_fd, mask_copy);
        close(*chord_fd);
        *chord_fd=0;
        
    } 
    if (*tcp_c_fd){
        FD_CLR(*tcp_c_fd, mask_copy);
        close(*tcp_c_fd);
        *tcp_c_fd=0;
        
    } 
    if (*accepted_socket){
        FD_CLR(*accepted_socket, mask_copy);
        close(*accepted_socket);
        *accepted_socket=0;
        
    } 
    if(*tcp_s_fd){
        FD_CLR(*tcp_s_fd, mask_copy);
        close(*tcp_s_fd);
        *tcp_s_fd =0;
    }
    FD_CLR(*listen_fd, mask_copy);
    FD_CLR(*udp_fd, mask_copy);
    close(*listen_fd);
    close(*udp_fd);
    *locked= false;
}    


void exit_routine(int* chord_fd,int* tcp_c_fd,int* accepted_socket,int* listen_fd, int* udp_fd){
     
    if (*chord_fd){
        close(*chord_fd);
        *chord_fd=0;
    } 
    if (*tcp_c_fd){
        close(*tcp_c_fd);
        *tcp_c_fd=0;
    } 
    if (*accepted_socket){
        close(*accepted_socket);
        *accepted_socket=0;
    } 
    close(*listen_fd);
    close(*udp_fd);
    exit(0);
}

//starting routine gets the argv and checks it's validity storing it in me
void start_routine(node* me, node*pred, node* succ,node* temp_node,node* chord,command_details_t* command_details,char *buffer,
                    char** argv,char* fcommand,char* buff,fd_set* mask,fd_set* mask_copy, int* my_searches, size_t buffer_size,client_addr_t* EFND_saved_addr){
    memset(me,0,sizeof(node));
    memset(pred, 0,sizeof(node));
    memset(succ, 0,sizeof(node));
    memset(temp_node,0,sizeof(node));
    memset(chord,0,sizeof(node));
    memset(command_details, 0,sizeof(command_details_t));
    memset(fcommand,0,sizeof(char)*100);
    memset(buff,0,sizeof(char)*60);
    memset(my_searches,-1,sizeof(int)*100);
    memset(mask,0,sizeof(fd_set));
    memset(mask_copy,0,sizeof(fd_set));   
    memset(EFND_saved_addr,0,sizeof(client_addr_t)*5);
    memset(buffer,0,buffer_size);
    strcpy(me->key,argv[1]);
    strcpy(me->IP,argv[2]);
    strcpy(me->PORT,argv[3]);
    if (valid_IP_port_key(me->IP, me->PORT,me->key)){
        printf("%s\t%s\t%s\n",me->key , me->IP,me->PORT);   
    }
    else{
        printf("IP or Port or key provided is not valid\n");
        exit(0);
    } 
    }

void split_FND_RSP( command_details_t* command_details,char *str){ 
    //* onde k é o procurado// destinatario
    //* n número de sequencia 
    //* i i.ip i.port // info de quem iniciou a procura/resposta
    char* str_to_free ,*str_to_split;
    str_to_free=str_to_split=strdup(str);
    split_str_nd_copy_to_new_location(&str_to_split,command_details->command);
    split_str_nd_copy_to_new_location(&str_to_split,command_details->searched_key);
    split_str_nd_copy_to_new_location(&str_to_split,command_details->n);
    split_str_nd_copy_to_new_location(&str_to_split,command_details->key);
    split_str_nd_copy_to_new_location(&str_to_split,command_details->IP);
    split_str_nd_copy_to_new_location(&str_to_split,command_details->PORT);
    free(str_to_free);
}

void split_self_pred_m(char*buff, node * node, char* compare,command_details_t* command_details){  
    char* str_to_free ,*str_to_split;
    str_to_free = str_to_split=strdup(buff);
    split_str_nd_copy_to_new_location(&str_to_split,command_details->command);
    if(strcmp(command_details->command,compare)==0){
        split_str_nd_copy_to_new_location(&str_to_split,node->key);
        split_str_nd_copy_to_new_location(&str_to_split,node->IP);
        split_str_nd_copy_to_new_location(&str_to_split,node->PORT);
    } 
    free(str_to_free);
   
}

void answer_to_a_EFND(client_addr_t* address,int udp_s_fd, char* buff){
    SA_in cli_addr;
    char ack[4];
    cli_addr.sin_family =AF_INET;
    inet_pton( AF_INET ,address->addr, (in_addr_t*) &cli_addr.sin_addr );
	cli_addr.sin_port = htons(address->port);
    sendto(udp_s_fd, buff,strlen(buff),0,(const struct sockaddr * ) &cli_addr,INET_ADDRSTRLEN);
    recv(udp_s_fd,ack,4,0);
}




//! type might change and so will the name 
// case 0  PREDcase 1 RSP(me)(or EFND), case 2 not mine nor my pred
// on the variable (bool one) true = tcp false = udp 
// if so is possible change type to int on the mode and have 3  modes maybe ? 
int common_code_FND_EFND(bool mode,node me, node succ, node pred, node chord, command_details_t command,bool* tcp_or_udp,int chord_fd){
    int succ_distance =check_distance(succ.key,command.searched_key);
    int me_distance= check_distance(me.key,command.searched_key);
    int chord_distance;
    if(chord_fd !=0)
        chord_distance = check_distance(chord.key,command.searched_key);
    
    if((chord_fd==0)||(succ_distance)<=  chord_distance) {
        *tcp_or_udp = 1;
    }else{
        *tcp_or_udp =0;
    }

    if(mode){
        if(check_distance(pred.key,command.searched_key)<= me_distance){
            if(strcmp(pred.key,command.searched_key)!=0)
            return 0;
        }
    }    
    if(me_distance<= succ_distance){ 
        if(strcmp(me.key,command.searched_key)!=0)
            return 1;
    }

    return 2;
    
}


void process_FND_RSP(char* buff,node me,node succ,node pred,node chord, int tcp_fd, int chord_fd, int* my_searches, client_addr_t* EFND_saved_addr,int udp_fd ){//? maybe pass command as arg so i can have access to it outside this func(most likely + easy change so for now i'll keep like is)

    command_details_t command;
    char str[100];
    int check,key;
    bool tcp_or_udp;
    strcpy(str,buff);
    split_FND_RSP(&command,str);
    if (strcmp(command.command,"FND")==0){
        if(strcmp(me.key,command.key)!=0){
            
                check = common_code_FND_EFND(false,me,succ,pred,chord,command,&tcp_or_udp,chord_fd);
                if (check==1){//object is mine
                    sprintf(buff,"%s %s %s %s %s %s","RSP",command.key, command.n,me.key ,me.IP, me.PORT);
                }
                if((chord_fd==0)||(check_distance(succ.key,command.searched_key))>= check_distance(chord.key,command.searched_key)) {
                    strcat(buff,"\n\0");
                    write(tcp_fd,buff,strlen(buff));
                }else{
                    send(chord_fd,buff,strlen(buff),MSG_CONFIRM);
                    recv(chord_fd,buff, 4,0);
                    if(*buff!='A'){
                        printf("No ACK, sending via tcp\n");
                        strcat(buff,"\n\0");
                        write(tcp_fd,buff,strlen(buff));
                    }
                }
                
        }else{
            my_searches[strtol(command.n, NULL, 10)]=-1;
        }
    }
    else if(strcmp(command.command,"RSP")==0){
        if (strcmp(me.key,command.searched_key)==0){
            check =strtol(command.n, NULL, 10); 
            if(check<5){
                if(my_searches[check]!=-1){
                    sprintf(buff,"%s %s %s %s","EPRED",command.key, command.IP,command.PORT);
                    answer_to_a_EFND(&EFND_saved_addr[check],udp_fd,buff);
                    //this key search was started by a EFND
                }
            }
            else{
                if(my_searches[check]!=-1){
                    key = my_searches[check];
                    printf("%s %d:\n\t%s %s %s\n","Resposta ao find",key,command.key,command.IP, command.PORT);
                }    
            }
            my_searches[check]=-1;                   
        }
        else if(strcmp(me.key,command.key)==0){
            printf("some RSP didn't get to the destination after a full round trip on the ring\n");
        }
        else{
            if((chord_fd==0)||(check_distance(succ.key,command.searched_key))< check_distance(chord.key,command.searched_key)) {
                strcat(buff,"\n\0");
                write(tcp_fd,buff,strlen(buff));
            }else{
                send(chord_fd,buff,strlen(buff),MSG_CONFIRM);
                recv(chord_fd,buff, 4,0);
                if(*buff!='A'){
                    printf("No ACK, sending via tcp\n");
                    strcat(buff,"\n\0");
                    write(tcp_fd,buff,strlen(buff));
                }
            }
        }
    }
    
    
}


void process_EFND_EPRED(bool in_a_ring,node me,node* pred,node succ,node chord, command_details_t *command_details,
                        char* buff, int tcp_fd, int chord_fd,int udp_s_fd,client_addr_t* address,int* my_searches,
                        bool chord_created,int* tcp_c_fd, SA_in* tcp_addr,SA_in quick_bentry){

    char* str_to_free ,*str_to_split, ack[4];
    int check;
    str_to_free=str_to_split=strdup(buff);
    split_str_nd_copy_to_new_location(&str_to_split,command_details->command);
    bool tcp_or_udp;
    if (in_a_ring){
        if (strcmp("EFND",command_details->command)==0){
             split_str_nd_copy_to_new_location(&str_to_split,command_details->searched_key);
           check =common_code_FND_EFND(true,me,succ,*pred,chord,*command_details,&tcp_or_udp,chord_created);
            if(check==1){
               // respond via UDP with my info 
               if(strcmp(me.key,command_details->searched_key)!=0){
                    memset(buff,0,sizeof(char)*100);
                    sprintf(buff,"%s %s %s %s","EPRED",me.key, me.IP,me.PORT);
                    sendto(udp_s_fd,buff,strlen(buff),0,(const struct sockaddr*) &quick_bentry,INET_ADDRSTRLEN);
                    recv(udp_s_fd,ack, 4,0);
               }
            }
           else if(check==0){
               //respond via UDP with my pred info
                if(strcmp(pred->key,command_details->searched_key)!=0){
                    memset(buff,0,sizeof(char)*100);
                    sprintf(buff,"%s %s %s %s\n","EPRED",pred->key, pred->IP,pred->PORT);
                    sendto(udp_s_fd,buff,strlen(buff),0,(const struct sockaddr* ) &quick_bentry,INET_ADDRSTRLEN);
                    recv(udp_s_fd,ack, 4,0);
               }
                
            }
            else if(!check_free_position(my_searches,0,&check)){
                my_searches[check] = strtol(command_details->searched_key, NULL, 10);
                add_client(address,check,quick_bentry);
                
                memset(buff,0,sizeof(char)*100);        
                sprintf(buff,"%s %s %d %s %s %s","FND",command_details->searched_key,check,me.key, me.IP, me.PORT);
                if(tcp_or_udp) {
                    strcat(buff,"\n\0");
                    write(tcp_fd,buff,strlen(buff));
                }else{
                    send(chord_fd,buff,strlen(buff),MSG_CONFIRM);
                    recv(chord_fd,ack, 4,0);
                    if(*buff!='A'){
                        printf("No ACK, sending via tcp\n");
                        strcat(buff,"\n\0");
                        write(tcp_fd,buff,strlen(buff));
                    }
                }         
            }
        }   
    }else if(strcmp("EPRED",command_details->command)==0){
        split_str_nd_copy_to_new_location(&str_to_split,pred->key);
        split_str_nd_copy_to_new_location(&str_to_split,pred->IP);
        split_str_nd_copy_to_new_location(&str_to_split,pred->PORT);
        sprintf(buff,"%s %s %s %s\n","SELF", me.key,me.IP,me.PORT);
        tcp_client(tcp_c_fd,tcp_addr,pred->PORT,pred->IP,buff);
    }
    free(str_to_free);
}

//initializes the mask with the req fd's
void mask_init(fd_set *mask_copy,int*maxfd,int udp_fd,int listen_fd,int chord_fd,int accepted_socket,int tcp_c_fd){
    *maxfd =0;
    FD_SET(0,mask_copy);
    if(listen_fd) {
        FD_SET(listen_fd, mask_copy);
        *maxfd=max(*maxfd,listen_fd);
    }
    if(udp_fd){
        FD_SET(udp_fd, mask_copy);
        *maxfd=max(*maxfd,udp_fd);
    }
    if(chord_fd){
        FD_SET(chord_fd, mask_copy);
        *maxfd=max(*maxfd,chord_fd);
    }
    if(accepted_socket){
        FD_SET(accepted_socket, mask_copy);
        *maxfd=max(*maxfd,accepted_socket);
    }
    if(tcp_c_fd){
        FD_SET(tcp_c_fd, mask_copy);
        *maxfd=max(*maxfd,tcp_c_fd);
    }
}

void  init_tcp_server(int *listen_fd, int PORT){
	SA_in tcp_servaddr;  
	*listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (*listen_fd == -1) {
		printf("Socket creation failed.\n");
		exit(0);
	}
	else
		printf("Socket successfully created.\n");
	bzero(&tcp_servaddr, sizeof(tcp_servaddr));
	// assign IP, PORT
	tcp_servaddr.sin_family = AF_INET;
	tcp_servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	tcp_servaddr.sin_port = htons(PORT);
    if ((setsockopt(*listen_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int))) < 0){
        perror("setsockopt(SO_REUSEADDR) failed");
    }
        
	// Binding newly created socket to given IP and verification
	if ((bind(*listen_fd, (SA*)&tcp_servaddr, sizeof(tcp_servaddr))) != 0) {
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

void  init_udp_server(int *udp_fd, int PORT, SA_in *tcp_servaddr){
	//SA_in tcp_servaddr;
// Creating socket file descriptor
    if ( (*udp_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    // Filling server information
    tcp_servaddr->sin_family    = AF_INET; // IPv4
    tcp_servaddr->sin_addr.s_addr = INADDR_ANY;
    tcp_servaddr->sin_port = htons(PORT); 
    if ((setsockopt(*udp_fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int))) < 0){
        perror("setsockopt(SO_REUSEADDR) failed");
    }
        struct timeval tv;
    tv.tv_sec =0;
    tv.tv_usec =50000; // *0.05 segundos posso mudar o valor 
    setsockopt(*udp_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    // Bind the socket with the server address
    if ( bind(*udp_fd, (const struct sockaddr *)tcp_servaddr, sizeof(*tcp_servaddr)) < 0 ){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        printf("to run client supply ring key, IP and port, separated by a single space \n");
        exit(0);
    } 
    // VAR INIT
    fd_set mask, mask_copy;
    int udp_fd=0, listen_fd=0,tcp_s_fd=0,maxfd=0,chord_fd=0,port,temp_tcp_c_fd=0,tcp_c_fd=0, n,accepted_socket=0,check,my_searches[100];
    time_t entered_ring= time(NULL);
    node me, pred,succ, temp_node,  chord;
    command_details_t command_details;
    bool in_a_ring = false, tcp_or_udp, locked =false;
    char fcommand[100],buff[60], buffer[256];
    SA_in tcp_client_addr, tcp_servaddr, udp_server_addr;
    size_t message_size =sizeof(char)*60;
    size_t buffer_size =sizeof(char)*256;
    socklen_t len = sizeof(SA_in);
    client_addr_t EFND_saved_addr[5];
    // ! TEST
    int pointer;
    char* handler;
    char* buffer_to_split,*buffer_to_free ;
    // VAR INIT

    /*start_routine(&me,&pred,&succ,&temp_node,&chord,&command_details,argv,fcommand,buff,&mask,&mask_copy,my_searches,message_size,EFND_saved_addr);
    port = strtol(me.PORT, NULL, 10);
    init_tcp_server(&listen_fd, port);
    init_udp_server(&udp_fd, port,&udp_server_addr);
    */
    for(;;){
        if(!locked){
            start_routine(&me,&pred,&succ,&temp_node,&chord,&command_details,buffer,argv,fcommand,buff,&mask,&mask_copy,my_searches,buffer_size,EFND_saved_addr);
            port = strtol(me.PORT, NULL, 10);
            init_tcp_server(&listen_fd, port);
            init_udp_server(&udp_fd, port,&udp_server_addr);
            locked =true;
        }
        mask_init(&mask_copy,&maxfd,udp_fd,listen_fd,chord_fd,accepted_socket,tcp_c_fd);
        mask = mask_copy;
        memset(buffer,0,buffer_size);
        memset(buff,0,message_size);
        memset(&command_details,0,sizeof(command_details_t));
        select(maxfd+1, &mask, (fd_set*)NULL, (fd_set*)NULL, (struct timeval *)NULL);
        if(FD_ISSET(0,&mask)){
            FD_CLR(0, &mask_copy);
            get_info_from_client( fcommand);
            if (*fcommand == 'n'){/* creates a new, empty ring where the node will ocupy the given key position per default */
                if((strcmp(fcommand,"new")==0)||(strcmp(fcommand,"n")==0)){
                     if(!in_a_ring){
                    in_a_ring =true; 
                    entered_ring = time(NULL);
                    new(&me, &succ,&pred,&temp_node);
                    }
                    else printf("Already in a ring\n");
                }
                else printf("bad command\n");
               
            }
                
            else if (*fcommand =='b' ){/*enters the ring knowing only one random node*/
                bentry_routine(fcommand, & command_details,&chord_fd, buff,me.key);
            }
                
            else if (*fcommand == 'p' ){/*enters the ring know it's pred_key*/
                if (in_a_ring== false){
                    pentry(fcommand,&command_details,&pred,&me,buff,&tcp_c_fd,&tcp_servaddr);
                }
            }
                
            else if (*fcommand =='c' ){/*creates a udp shortcut to a given key, key_ip, key_port*/                
                if(chord_fd==0){
                    if(split_command(fcommand,&command_details)){
                        if((strcmp(command_details.command,"chord")==0)||(strcmp(command_details.command,"c")==0)){
                            strcpy(chord.IP,command_details.IP);
                            stpcpy(chord.key,command_details.key);
                            strcpy(chord.PORT,command_details.PORT);
                            init_udp_client(&chord_fd,command_details.PORT,command_details.IP);
                        }
                        else printf("bad command\n");
                    }
                }else printf("Already have a chord, delete that one before making a new one\n");    
            }
                
            else if(*fcommand == 'd' ){//echord 
                if(chord.IP!=0){
                    if((strcmp(command_details.command,"delchord")==0)||(strcmp(command_details.command,"d")==0)){
                        FD_CLR(chord_fd,&mask_copy);
                        close(chord_fd);
                        memset(&chord, 0,sizeof(node));
                        chord_fd= 0;  
                    }
                    else printf("bad command\n");
                }
            }
                
            else if(*fcommand =='s' ){// show current status
                if((strcmp(fcommand,"show")==0)||(strcmp(fcommand,"s")==0)){
                    time_t uptime =time(NULL)- entered_ring;
                    show_status(in_a_ring, me,succ,pred,chord,uptime,chord_fd,tcp_c_fd,tcp_s_fd);                    
                }
                else printf("bad command\n");
            }
                
            else if(*fcommand =='f' ){

                //todo check if i can combine all this crap into a single function outside of main (getting rid of the extra variables)
                //* such as check and tcp or udp wich can be replaced by a fuction returning 1 or 0 depending on the case 
                // for now it shall do the job still need to implement all the message sending and writting the output 
                // on the mine/pred key (find a good way to present it to the user !)
                if(split_find_command(fcommand,&command_details)){
                    if((strcmp(command_details.command,"find")==0)||(strcmp(command_details.command,"f")==0)){
                        check = common_code_FND_EFND(1,me,succ,pred,chord,command_details,&tcp_or_udp,chord_fd);
                        if(check==1){// verifies if it is mine or not
                            printf("%s %s:\n\t%s %s %s\n","Resposta ao find",command_details.searched_key,me.key,me.IP, me.PORT);
                            //key is mine
                        }else if (check ==0){
                            printf("%s %s:\n\t%s %s %s\n","Resposta ao find",command_details.searched_key,pred.key,pred.IP, pred.PORT);
                        
                            //key is from my pred
                        }
                        // * starts a FND so need to save the searched key in the array and update the n
                        else if(!check_free_position(my_searches,1,&check)){
                                my_searches[check]= strtol(command_details.searched_key, NULL, 10);
                            
                            if(tcp_or_udp){
                                // is tcp to succ
                                sprintf(buff,"%s %s %d %s %s %s\n","FND",command_details.searched_key,check,me.key, me.IP, me.PORT);
                                write(tcp_s_fd, buff, strlen(buff));
                            }
                            else{
                                sprintf(buff,"%s %s %d %s %s %s","FND",command_details.searched_key,check,me.key, me.IP, me.PORT);
                                send(chord_fd,buff,strlen(buff),MSG_CONFIRM);
                                recv(chord_fd,buff, 4,0);
                                if (*buff!='A'){
                                    printf("sending via chord failed, sending via tcp\n");
                                    strcat(buff,"\n\0");
                                    write(tcp_s_fd, buff, strlen(buff));
                                }
                                //is via udp to chord //
                            }
                        }
                        else printf("Can't start a FND atm cus storage is full\n");

                    }
                    else printf("bad command\n");
                }    
            }
                
            else if(*fcommand =='l'){
               if(in_a_ring){
                    if((strcmp(command_details.command,"leave")==0)||(strcmp(command_details.command,"l")==0)){
                        leave_ring(me,&succ,&pred,buff,&chord_fd,&tcp_c_fd,&accepted_socket,&tcp_s_fd,&mask_copy,&udp_fd,&listen_fd,&locked);
                        in_a_ring= false;
                    }
                    else printf("bad command\n");
                }
                else printf("can't leave cus not in a ring\n");
            }

            else if(*fcommand =='e' ){// exit
                if((strcmp(command_details.command,"exit")==0)||(strcmp(command_details.command,"e")==0)){
                    exit_routine(&chord_fd,&tcp_c_fd,&accepted_socket,&listen_fd, &udp_fd);     
                    //exit => close all sockets and return 0
                }
                else printf("bad command\n");
            }

            else{
                printf("Command given, \" %s\",  is not valid.\n", fcommand);
            }
        }
        else if(listen_fd && FD_ISSET(listen_fd,&mask)){
            FD_CLR(listen_fd, &mask_copy);
            if(accepted_socket!=0)close(accepted_socket);
            accepted_socket = accept(listen_fd, (SA*)&tcp_client_addr, &len);
            if (accepted_socket < 0) {
                printf("Server accept failed.\n");
                exit(0);
            }      
        }     
        else if(udp_fd && FD_ISSET(udp_fd,&mask)){
            FD_CLR(udp_fd, &mask_copy);
            if((n = recvfrom(udp_fd, (char *)buff, message_size, MSG_WAITALL, ( struct sockaddr *) &udp_server_addr,&len)) !=0){
                sendto(udp_fd, "ACK", 4, MSG_CONFIRM, (const struct sockaddr *) &udp_server_addr,len);
                buff[strcspn(buff, "\n")] = 0;
                printf("%s\n", buff);
                if((*buff=='F')||(*buff=='R')){
                    process_FND_RSP(buff,me,succ,pred,chord,tcp_s_fd,chord_fd,my_searches,EFND_saved_addr,udp_fd);
                }
                else if (*buff=='E'){
                    process_EFND_EPRED(in_a_ring,me,&pred, succ,chord,&command_details,buff,tcp_s_fd,chord_fd,udp_fd,
                                        EFND_saved_addr,my_searches,chord_fd,&tcp_c_fd,&tcp_servaddr,udp_server_addr);
                }
            }
        }
        else if(chord_fd && FD_ISSET(chord_fd,&mask)){
            FD_CLR(chord_fd, &mask_copy);
            if((n = recv(chord_fd, (char *)buff, message_size, MSG_WAITALL)) !=0){
                send(chord_fd, "ACK", 4, MSG_CONFIRM);
                buff[strcspn(buff, "\n")] = 0;
                printf("%s\n", buff);
                if (*buff=='E'){
                        process_EFND_EPRED(in_a_ring,me,&pred, succ,chord,&command_details,buff,tcp_s_fd,chord_fd,udp_fd,
                                            EFND_saved_addr,my_searches,chord_fd,&tcp_c_fd,&tcp_servaddr,udp_server_addr);
                    }     
            }
            close(chord_fd);
            chord_fd=0;
        }
        else if(accepted_socket && FD_ISSET(accepted_socket, &mask)){
            FD_CLR(accepted_socket, &mask_copy);
            if((n = read(accepted_socket, buff, message_size)) != 0){
                if(n == -1){
                    printf("Reading error.\n");
                    exit(1);
                }
                buff[strcspn(buff, "\n")] = 0;
                printf("%s\n", buff);
                //! check this here
                if (*buff =='S'){
                split_self_pred_m( buff, &temp_node,"SELF",&command_details);
                    if(in_a_ring){
                        if((strcmp(me.key, succ.key))==0){
                            sprintf(buff,"%s %s %s %s\n","SELF", me.key,me.IP,me.PORT);
                            copy_node_info(&temp_node,&pred);
                            tcp_client(&tcp_c_fd,&tcp_servaddr,temp_node.PORT,temp_node.IP,buff);
                            
                        }
                        else {
                            sprintf(buff,"%s %s %s %s\n","PRED", temp_node.key,temp_node.IP,temp_node.PORT);
                            write(tcp_s_fd,buff,strlen(buff));
                            close(tcp_s_fd);                            
                        }
                    }
                    else{
                        in_a_ring=1;
                        entered_ring = time(NULL);
                    } 
                    copy_node_info(&temp_node,&succ);
                    tcp_s_fd = accepted_socket;
                    accepted_socket=0; 
                }  

            }
            else {
                close(accepted_socket);
                accepted_socket= 0;
            }
        }
        else if(tcp_c_fd && FD_ISSET(tcp_c_fd, &mask)){
            FD_CLR(tcp_c_fd, &mask_copy);
    
            if((n = read(tcp_c_fd, buffer, buffer_size)) != 0){
                if(n == -1){
                    printf("Reading error.\n");
                    exit(1);
                }
                buffer_to_free=buffer_to_split= strdup(buffer);
                do{ 
                    
                    handler=strsep(&buffer_to_split,"\n");
                    if(handler!=0){
                        strcpy(buff,handler);
                        
                        /*if(*buffer !='\0'){
                            pointer=strlen(buff);
                        }*/
                        buff[strcspn(buff, "\n")] = 0;
                        printf("%s\n", buff);
                        if(*buff=='P'){ 
                            split_self_pred_m(buff,&pred,"PRED",&command_details);
                            sprintf(buff,"%s %s %s %s\n","SELF", me.key,me.IP,me.PORT);
                            close(tcp_c_fd);
                            tcp_client(&temp_tcp_c_fd,&tcp_servaddr,pred.PORT, pred.IP,buff);
                            tcp_c_fd=temp_tcp_c_fd;
                            temp_tcp_c_fd=0;
                        }
                        else if((*buff=='F')||(*buff=='R')){
                            //! here
                            process_FND_RSP(buff,me,succ,pred,chord,tcp_s_fd,chord_fd,my_searches,EFND_saved_addr,udp_fd);
                        }
                        
                    }
                    /*else if(*buffer_to_split!='\0'){
                        strcpy(buff,buffer_to_split);
                        free(buffer_to_free);
                        if((n = read(tcp_c_fd, buffer, buffer_size)) != 0){
                            if(n == -1){
                                printf("Reading error.\n");
                                exit(1);
                            }
                            buffer_to_free=buffer_to_split= strdup(buffer);
                        }
                    }*/
                      
                }while(*buffer_to_split!='\0'); 
                free(buffer_to_free);
            }
            else {
                close(tcp_c_fd);
                tcp_c_fd = 0;
            }
        }
    }
    return 0;    
}
