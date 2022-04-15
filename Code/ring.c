#include "header.h"

//! making quick resume of the connections 
/*
!TCP
tcp_c_fd  //  *  connects to the pred 
sends:SELF
recieves: PRED,FND,RSP ---
//////////////////////////////////
accepted_fd //  * accepts a connected from a new client(aka new succ)
sends: nothing 
recieves:SELF(then binds this to the tcp_s_fd)
//////////////////////////////////
tcp_s_fd//  * connected to the succ
sends:PRED, FND, RSP
recieves:nothing 
!UDP
chord_fd
  s* connects to a chord if one exists
sends:FND,RSP,"ACK" //  * ACK IS ONLY WHEN IT RECIEVES A EPRED
recieves:"ACK",EPRED

udp_s_fd
sends:"ACK", EPRED
recieves:"ACK",FND,RSP, EFND//  * when EFND we need to store the addres (maybe store up to 5 addresses(?))

*/
//! making quick resume of the connections

//todo help command

//checks array where the keys are stored for a free spot, returning that value by address; if no free position is found returns false
bool check_free_position(int* my_searches, bool mode,int* n){
    static int i = 4;
    static int k = -1;
    int count =100;//* control variable to check for full array
    
    if(mode){
        do{
            if (i == 99)
                i=4;//* positions 0-5 are for EFND  started finds
            i++;
            count++;
        }while((my_searches[i]!=-1)&&(count <200));
       *n =i;
       return((count==200 )&&(my_searches[i]!=-1));
    }
    else{
        do{
            if (k == 5)
                k=-1;//* positions 0-5 are for EFND  started finds
            k++;
            count++;
        }while((my_searches[k]!=-1)&&(count <110));
        *n =k;
        return((count==110 )&&(my_searches[i]!=-1));
    }     
}        

//saves the address given into the clients_addr struct
void save_address(client_addr_t * EFND_saved_addr,int n , SA_in client_addr) {
    inet_ntop( AF_INET, &client_addr.sin_addr, EFND_saved_addr[n].addr, INET_ADDRSTRLEN );  
    EFND_saved_addr[n].port = (int) ntohs(client_addr.sin_port);
}

// gets line from stdin given by user
void get_info_from_client( char* fcommand){
    size_t len = 1000; 
    getline(&fcommand,&len,stdin);  // * gets a full line until '\n' is met
    fcommand[strlen(fcommand)-1]= '\0'; // * removes the \n 
}

//prints the current status of the client with the relevant node and ring (known) information 
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
        // uptime/60 is time in minutes and the remainder of uptime/60 is seconds
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

// checks the distance between origin and the given searched_key, returns the distance value
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
    int  check;//* used to check if the port/key are valid
    char ipvalidation[INET_ADDRSTRLEN];
    check = strtol(key, NULL, 10);//* converts the key str into integer type
    if(check <MAX_KEY && check > -1) key_validator = true;
    ip_validator = (bool)inet_pton( AF_INET, IP, ipvalidation );// * inetpton returns 0 incase the given ip is not valid 
    check = strtol(PORT, NULL, 10);//* converts the port into decimal
    if(check <65536 && check > 0) port_validator = true;//* checks if the port is between PORT max and min values
    return (ip_validator && port_validator && key_validator);//* returns true if  IP ,key and PORT are valid, false otherwise
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
    if (str_to_split == NULL){ //* routine for bad command
        printf("%s given is incomplete or wrong\n", fcommand);
        free(str_to_free);
        return false;
    }
    split_str_nd_copy_to_new_location(&str_to_split,command_details->command);
    if (str_to_split == NULL){ // *  routine for bad command 
         printf("%s given is incomplete or wrong\n", fcommand);
        free(str_to_free);
        return false;
    }
    split_str_nd_copy_to_new_location(&str_to_split,command_details->key);
    if (str_to_split == NULL){ // *  routine for bad command 
        printf("%s given is incomplete or wrong\n", fcommand);
        free(str_to_free);
        return false;
    }
    split_str_nd_copy_to_new_location(&str_to_split,command_details->IP);
    if (str_to_split == NULL){ // *  routine for bad command 
        printf("%s given is incomplete or wrong\n", fcommand);
        free(str_to_free);
        return false;
    }
    split_str_nd_copy_to_new_location(&str_to_split,command_details->PORT);
    if(str_to_split != NULL){// * routine for bad command 
       printf("%s given is incomplete or wrong\n", fcommand);
        free(str_to_free);
        return false;
    }
    free(str_to_free);
    return true;
}

/* splits the command given by user saving it in a structure while checking if the command is valid
mode : 0=> find ; 1=> key ; 2 => IP*/
bool split_small_command(char * fcommand, command_details_t* command_details,int mode){
    char* str_to_free ,*str_to_split;
    str_to_free=str_to_split=strdup(fcommand);
    split_str_nd_copy_to_new_location(&str_to_split,command_details->command);
    if (str_to_split == NULL){ // * routine for bad command 
        printf("%s given is incomplete or wrong\n", fcommand);
        free(str_to_free);
        return false;
    }
    if (mode==0) split_str_nd_copy_to_new_location(&str_to_split,command_details->searched_key);
    else if (mode==1) split_str_nd_copy_to_new_location(&str_to_split,command_details->key);
    else split_str_nd_copy_to_new_location(&str_to_split,command_details->IP);
    if (str_to_split != NULL){ // *  routine for bad command 
        printf("%s given is incomplete or wrong\n", fcommand);
        free(str_to_free);
        return false;
    }
    free(str_to_free);
    return true;
}

// copys the data from origin node to the copy
void copy_node_info(node *origin, node *copy){
    strcpy(copy->IP,origin->IP);
    strcpy(copy->key,origin->key);
    strcpy(copy->PORT,origin->PORT);
}

// start a tcp_client which connects with the predecessor and sends the message given
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
		//inet_pton - convert IPv4 and IPv6 addresses from text to binary form  returns 0 if given adress isn't valid 
		printf("Adress is not valid.\n");
		exit(0);
  	} 
    else{
        if (connect(*tcp_c_fd, (SA*)tcp_servaddr, sizeof(*tcp_servaddr)) != 0) {
		    printf("Connection with the server failed.\n");
            exit(0);
        }
        else{
            printf("Connected to the server.\n");
            write(*tcp_c_fd,message, strlen(message));
        }       
    }  
}

// start a tcp_client which connects with chord, and is also used for bentry
void init_udp_client(int* chord_fd,  char* port_ch ,char* addr){
     // Creating socket file descriptor
    SA_in servaddr; 
    if ( (*chord_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
    // Filling server information
    servaddr.sin_family = AF_INET;
    int port = strtol(port_ch, NULL, 10);
    servaddr.sin_port = htons(port);
    // establishing a timeout
    struct timeval timeout;
    timeout.tv_sec =0;
    timeout.tv_usec =TIMEOUT; // *0.1 secs
    setsockopt(*chord_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof (timeout));
    if (inet_pton( AF_INET, addr, (in_addr_t*) &servaddr.sin_addr.s_addr) < 1) {
		//inet_pton - convert IPv4 and IPv6 addresses from text to binary form 
		// returns 0 if given adress isn't valid 
		printf("Adress is not valid.\n");
        close(*chord_fd);
        *chord_fd=0;
  	} 
    else if (connect(*chord_fd,(const SA*)&servaddr,sizeof(servaddr))==-1) {
        printf("Failed to connect chord/udp_client.\n");
    }  
}


//routine for creating a new node, has to initialize the successor and predecessor nodes
void new(node *me, node *pred, node *succ,node* temp_node){
    copy_node_info(me,pred);// * initialize predecessor = me
    copy_node_info(me,succ);// * initialize successor = me
    copy_node_info(me,temp_node);
}

//routine for bentry - sending a EFND by UPD using the chord_fd
void bentry_routine(char* fcommand, command_details_t* command_details,int* chord_fd,char*buff,char* key){
    int check;
    if(split_command(fcommand,command_details)){
        if((strcmp(command_details->command,"bentry")==0)||(strcmp(command_details->command,"b")==0)){
            check = strtol(command_details->key, NULL, 10);
            if(check <32 && check > -1) {
                sprintf(buff,"%s %s","EFND", key);
                init_udp_client(chord_fd,command_details->PORT,command_details->IP);
                send(*chord_fd,buff,strlen(buff),MSG_CONFIRM);
                recv(*chord_fd,buff,4,0);
                if(*buff!='A'){
                    printf("No ACK received, retry bentry\n");
                    close(*chord_fd);
                    *chord_fd=0;
                }
            }else printf("The key given is not within 0 and 31\n");
 
        } else printf("Invalid command\n");
    }

}

//routine for bentry - sending a SELF bys TCP using the tcp_c_fd
void pentry(char* fcommand,command_details_t* command_details,node* pred,node* me,char* buff,int* tcp_c_fd,SA_in* tcp_servaddr){
    int check;
    if(split_command(fcommand,command_details)){
        if((strcmp(command_details->command,"pentry")==0)||(strcmp(command_details->command,"p")==0)){
            check = strtol(command_details->key, NULL, 10);
            if(check <32 && check > -1){
                strcpy(pred->IP,command_details->IP);
                strcpy(pred->key,command_details->key);
                strcpy(pred->PORT,command_details->PORT);
                sprintf(buff,"%s %s %s %s\n","SELF", me->key,me->IP,me->PORT);
                tcp_client(tcp_c_fd,tcp_servaddr,pred->PORT,pred->IP,buff);
            }
            else printf("The key given is not within 0 and 31\n"); 
        }
        else printf("Invalid command\n");
    }   
}

//routine for leave - closes all the necessary sockets and sends PRED to the succ if the node is not alone in the ring 
void leave_ring(node me, node* succ, node* pred, char* buff,int* chord_fd,int* tcp_c_fd, int* accepted_socket, int* tcp_s_fd ,fd_set* mask_copy,int* udp_fd, int* listen_fd, bool* locked, bool*in_a_ring){
    
    if((strcmp(me.key, succ->key)!=0)&& *in_a_ring){
        sprintf(buff,"%s %s %s %s\n","PRED", pred->key,pred->IP,pred->PORT);
        write(*tcp_s_fd,buff,strlen(buff));
    }
  
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
    *in_a_ring=false;
    *locked= false;
}    

//routine for exit - closes all the sockets and exits with 0
void exit_routine(int* chord_fd,int* tcp_c_fd,int* accepted_socket,int* listen_fd, int* udp_fd, int* tcp_s_fd){
    
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
    if(*tcp_s_fd){
        close(*tcp_s_fd);
        *tcp_s_fd =0;
    }
    close(*listen_fd);
    close(*udp_fd);
    exit(0);
}

//starting routine - checks the validity of the IP ,PORT and IP and memset's all the relevant ring structures
void start_routine(node* me, node*pred, node* succ,node* temp_node,node* chord,command_details_t* command_details,char *buffer,
                    node*temp_me,char* fcommand,char* buff,fd_set* mask,fd_set* mask_copy, int* my_searches, size_t buffer_size,client_addr_t* EFND_saved_addr, bool *mode){
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
    copy_node_info(temp_me,me);
    if(*mode){
        if (!valid_IP_port_key(me->IP, me->PORT,me->key)){ 
            printf("IP or Port or key provided is not valid\n");
            exit(0);
        }
        else printf("%s %s %s\n",me->key,me->IP, me->PORT);
    }else printf("%s %s %s\n",me->key,me->IP, me->PORT);
 
}

//splits FND'S and RSP's
void split_FND_RSP( command_details_t* command_details,char *str){ 
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

//splits SELF's and PRED's
void split_SELF_PRED(char*buff, node * node, char* compare,command_details_t* command_details){  
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

/* return= 0  PRED return= 1 MINE, else return= 2  
mode: if true it's a EFND or find else it's a FND*/
int common_code_FND_EFND(bool mode,node me, node succ, node pred, command_details_t command){
    int succ_distance =check_distance(succ.key,command.searched_key);
    int me_distance= check_distance(me.key,command.searched_key);

    if(mode){
        if(check_distance(pred.key,command.searched_key)<= me_distance){
            if(strcmp(pred.key,command.searched_key)!=0) return 0;
        }
    }    
    if(me_distance<= succ_distance){ 
        if(strcmp(me.key,command.searched_key)!=0)
            return 1;
    }
    return 2;
}

//sends the message having determinated the best path and uses timeout in case ack is not recieved 
void send_message_FND_RSP(int chord_fd,int tcp_fd, char*buff,char* succ_key, char* chord_key,char*seached_key){
    if((chord_fd==0)||(check_distance(succ_key,seached_key))< check_distance(chord_key,seached_key)) {
        strcat(buff,"\n\0");
        write(tcp_fd,buff,strlen(buff));
    }else{
        send(chord_fd,buff,strlen(buff),MSG_CONFIRM);
        recv(chord_fd,buff, 4,0);
        if(*buff!='A'){
            printf("No ACK received, sending fnd via tcp\n");
            strcat(buff,"\n\0");
            write(tcp_fd,buff,strlen(buff));
        }
    }
}

// processes FND's and RSP's 
void process_FND_RSP(char* buff,node me,node succ,node pred,node chord, int tcp_fd, int chord_fd, int* my_searches, client_addr_t* EFND_saved_addr,int udp_fd ){//? maybe pass command as arg so i can have access to it outside this func(most likely + easy change so for now i'll keep like is)
    SA_in EFND_client;
    command_details_t command;
    char str[100];
    int check,key;
    strcpy(str,buff);
    split_FND_RSP(&command,str);
    if (strcmp(command.command,"FND")==0){
        if(strcmp(me.key,command.key)!=0){
            check = common_code_FND_EFND(false,me,succ,pred,command);
            if (check==1){//object is mine
                sprintf(buff,"%s %s %s %s %s %s","RSP",command.key, command.n,me.key ,me.IP, me.PORT);
            }
            send_message_FND_RSP(chord_fd,tcp_fd,buff,succ.key,chord.key,command.searched_key);  
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
                    EFND_client.sin_family=AF_INET;
                    inet_pton( AF_INET ,EFND_saved_addr[check].addr, (in_addr_t*) &EFND_client.sin_addr );
                    EFND_client.sin_port = htons(EFND_saved_addr[check].port);
                    sendto(udp_fd, buff,strlen(buff),0,(const struct sockaddr * ) &EFND_client,INET_ADDRSTRLEN);
                    recv(udp_fd,buff,4,0);
                    if(*buff!='A'){
                        printf("No ack recieved when sending a EPRED\n");
                    }
                }
            }
            else{
                if(my_searches[check]!=-1){
                    key = my_searches[check];
                    printf("%s %d: %s %s (%s:%s)\n","Key ",key,"node",command.key,command.IP, command.PORT);
                }    
            }
            my_searches[check]=-1;                   
        }
        else if(strcmp(me.key,command.key)==0){
            printf("Some RSP message didn't get to the destination after a full round trip on the ring\n");
        }
        else{
            send_message_FND_RSP(chord_fd,tcp_fd,buff,succ.key,chord.key,command.searched_key);
        }
    }

}

//processes EFND's and EPRED's 
void process_EFND_EPRED(bool in_a_ring,node me,node* pred,node succ,node chord, command_details_t *command_details,
                        char* buff, int tcp_fd, int chord_fd,int udp_s_fd,client_addr_t* address,int* my_searches,
                        int* tcp_c_fd, SA_in* tcp_addr,SA_in quick_bentry,bool* pentry_or_new){

    char* str_to_free ,*str_to_split;
    int check;
    str_to_free=str_to_split=strdup(buff);
    split_str_nd_copy_to_new_location(&str_to_split,command_details->command);
    
    if (in_a_ring){
        if (strcmp("EFND",command_details->command)==0){
            split_str_nd_copy_to_new_location(&str_to_split,command_details->searched_key);
            check =common_code_FND_EFND(true,me,succ,*pred,*command_details);
            if(check==1){
               // respond via UDP with my info 
               if(strcmp(me.key,command_details->searched_key)!=0){
                    memset(buff,0,sizeof(char)*100);
                    sprintf(buff,"%s %s %s %s","EPRED",me.key, me.IP,me.PORT);
                    sendto(udp_s_fd,buff,strlen(buff),0,(const struct sockaddr*) &quick_bentry,INET_ADDRSTRLEN);
                    recv(udp_s_fd,buff, 4,0);
               }
            }else if(check==0){
               //respond via UDP with my pred info
                if(strcmp(pred->key,command_details->searched_key)!=0){
                    memset(buff,0,sizeof(char)*100);
                    sprintf(buff,"%s %s %s %s\n","EPRED",pred->key, pred->IP,pred->PORT);
                    sendto(udp_s_fd,buff,strlen(buff),0,(const struct sockaddr* ) &quick_bentry,INET_ADDRSTRLEN);
                    recv(udp_s_fd,buff, 4,0);
                }     
            }else if(!check_free_position(my_searches,0,&check)){
                my_searches[check] = strtol(command_details->searched_key, NULL, 10);
                save_address(address,check,quick_bentry);
                memset(buff,0,sizeof(char)*100);        
                sprintf(buff,"%s %s %d %s %s %s","FND",command_details->searched_key,check,me.key, me.IP, me.PORT);
                send_message_FND_RSP(chord_fd,tcp_fd,buff,succ.key,chord.key,command_details->searched_key);   
            }
        }      
    }else if(strcmp("EPRED",command_details->command)==0){
        *pentry_or_new= true;
        split_str_nd_copy_to_new_location(&str_to_split,pred->key);
        split_str_nd_copy_to_new_location(&str_to_split,pred->IP);
        split_str_nd_copy_to_new_location(&str_to_split,pred->PORT);
        sprintf(buff,"%s %s %s %s\n","SELF", me.key,me.IP,me.PORT);
        tcp_client(tcp_c_fd,tcp_addr,pred->PORT,pred->IP,buff);
    }
    free(str_to_free);
}

//initializes the mask with the req fd's updating the max_fd
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

//starts the tcp server
void init_tcp_server(int *listen_fd, int PORT){
	SA_in tcp_servaddr;  
	*listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (*listen_fd == -1) {
		printf("Socket creation failed.\n");
		exit(0);
	}else printf("Socket successfully created.\n");
	bzero(&tcp_servaddr, sizeof(tcp_servaddr));
	// assign IP, PORT , and type to the address
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
	}else printf("Socket successfully binded.\n");
	// Now server is ready to listen and verification
	if ((listen(*listen_fd, 5)) != 0) {
		printf("Listen failed.\n");
		exit(0);
	}else printf("Server listening.\n");
}

//starts the udp server
void init_udp_server(int *udp_fd, int PORT, SA_in *tcp_servaddr){
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
    struct timeval timeout;
    timeout.tv_sec =0;
    timeout.tv_usec =TIMEOUT; // 0.1 segundos
    setsockopt(*udp_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof (timeout));
    // Bind the socket with the server address
    if ( bind(*udp_fd, (const struct sockaddr *)tcp_servaddr, sizeof(*tcp_servaddr)) < 0 ){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}

//instructions to work witj the programm ring
void help(){
    system("clear");
    printf("                    HELP\n");
    printf("\r%-12s %-10s %-35s %-40s\n","Command:", "Short form:", "Arguments:", "Short Explanation");
    printf("\r%-12s %-10s %-35s %-40s\n","new","n", "-" ,"Creates ring with a single node");
    printf("\r%-12s %-10s %-35s %-40s\n","leave", "l" ,"-", "Node leaves the current ring");
    printf("\r%-12s %-10s %-35s %-40s\n","exit" ,"e" ,"-", "Exits the program (after leave)");
    printf("\r%-12s %-10s %-35s %-40s\n","delchord", "d" ,"-" ,"Deletes the chord if one exists");
    printf("\r%-12s %-10s %-35s %-40s\n","chord", "c", "chord.key chord.IP chord.PORT" ,"Starts a UDP connection with the given node");
    printf("\r%-12s %-10s %-35s %-40s\n","pentry", "p", "pred.key pred.IP pred.PORT", "Enters the ring knowing the predecessor node");
    printf("\r%-12s %-10s %-35s %-40s\n","bentry", "b", "node.key node.IP node.PORT", "Enters the ring without knowing the predecessor, sends the request to the given node");
    printf("\r%-12s %-10s %-35s %-40s\n","find" ,"f" ,"searched_key" ,"Finds an object inside the ring");
    printf("\r%-12s %-10s %-35s %-40s\n","key", "k", "new_me.key", "Changes the key of the node while not in a ring with other nodes");
    printf("\r%-12s %-10s %-35s %-40s\n","ip" ,"i" ,"new_me.IP", "Changes the IP of the node while not in a ring with other nodes");
    printf("\r%-12s %-10s %-35s %-40s\n","reset", "r", "new_me.key new_me.IP new_me.PORT" ,"Changes the key, IP and PORT of the node while not in a ring with other nodes");
}

int main(int argc, char *argv[]){
    
    //checks the number os arguments
    if (argc != 4) {
        printf("To run client, insert ring key, IP and port, separated by a single space \n");
        exit(0);
    } 
    
    // VAR INIT
    fd_set mask, mask_copy;
    int udp_fd=0, listen_fd=0,tcp_s_fd=0,maxfd=0,chord_fd=0,port,temp_tcp_c_fd=0,tcp_c_fd=0, n,accepted_socket=0,check,my_searches[100];
    time_t entered_ring= time(NULL);
    node me, pred,succ, temp_node,  chord,*temp_pred,*temp_me;
    command_details_t command_details;
    bool in_a_ring = false, locked =false, pentry_or_new = false,validate_ip=true,first_iteration=true;
    char fcommand[100],buff[60], buffer[256],* handler=NULL,* tester,* buffer_to_split ,*buffer_to_free;
    SA_in tcp_client_addr, tcp_servaddr, udp_server_addr;
    size_t message_size =sizeof(char)*60, buffer_size =sizeof(char)*256,pointer =0;
    socklen_t len = sizeof(SA_in);
    client_addr_t EFND_saved_addr[5];
    struct timeval timeout;
    timeout.tv_sec =0;
    timeout.tv_usec =TIMEOUT; // *0.1 segundos
    // VAR INIT
    
    for(;;){
        
        if(!locked){
            temp_me=(node*) calloc (1,sizeof(node));
            if(first_iteration){
                strcpy(temp_me->key,argv[1]);
                strcpy(temp_me->IP,argv[2]);
                strcpy(temp_me->PORT,argv[3]);
                first_iteration=false;
            }else{
                copy_node_info(&me,temp_me);
            }
            start_routine(&me,&pred,&succ,&temp_node,&chord,&command_details,buffer,temp_me,fcommand,buff,&mask,&mask_copy,my_searches,buffer_size,EFND_saved_addr,&validate_ip);
            free(temp_me);
            port = strtol(me.PORT, NULL, 10);
            init_tcp_server(&listen_fd, port);
            init_udp_server(&udp_fd, port,&udp_server_addr);
            locked =true;
        }
        
        //initialize masks and select
        mask_init(&mask_copy,&maxfd,udp_fd,listen_fd,chord_fd,accepted_socket,tcp_c_fd);
        mask = mask_copy;
        memset(buffer,0,buffer_size);
        memset(buff,0,message_size);
        memset(&command_details,0,sizeof(command_details_t));
        select(maxfd+1, &mask, (fd_set*)NULL, (fd_set*)NULL, (struct timeval *)NULL);
        
        if(FD_ISSET(0,&mask)){
            FD_CLR(0, &mask_copy);
            get_info_from_client( fcommand);
            
            //command new - creates a new ring with the current node
            if (*fcommand == 'n'){
                if((strcmp(fcommand,"new")==0)||(strcmp(fcommand,"n")==0)){
                     if(!in_a_ring){
                        in_a_ring =true;
                        pentry_or_new=true; 
                        entered_ring = time(NULL);
                        new(&me, &succ,&pred,&temp_node);
                    }else printf("Already in a ring\n");
                }else printf("Invalid command\n");
            }
            
            //command bentry - enters the ring not knowing it's pred key, sends message to given node (UDP)
            else if (*fcommand =='b' ){
                bentry_routine(fcommand, & command_details,&chord_fd, buff,me.key);
            }    

            //command bentry - enters the ring enters the ring knowing it's pred_key 
            else if (*fcommand == 'p' ){
                if (in_a_ring== false){
                    pentry(fcommand,&command_details,&pred,&me,buff,&tcp_c_fd,&tcp_servaddr);
                    pentry_or_new = true;
                }
            }  

            //command chord - creates a udp shortcut to a given key, key_ip, key_port  
            else if (*fcommand =='c' ){             
                if(chord_fd==0){
                    if(split_command(fcommand,&command_details)){
                        if((strcmp(command_details.command,"chord")==0)||(strcmp(command_details.command,"c")==0)){
                            check = strtol(command_details.key, NULL, 10);
                            if(check <32 && check > -1) {
                                strcpy(chord.IP,command_details.IP);
                                stpcpy(chord.key,command_details.key);
                                strcpy(chord.PORT,command_details.PORT);
                                init_udp_client(&chord_fd,command_details.PORT,command_details.IP);
                            }else printf("The key given is not within 0 and 31\n");   
                        }else printf("Invalid command\n");
                    }
                }else printf("Already have a chord, delete that one before making a new one\n");    
            } 

            //command delete chord - deletes given chord
            else if(*fcommand == 'd' ){
                if(chord_fd){
                    if((strcmp(fcommand,"delchord")==0)||(strcmp(fcommand,"d")==0)){
                        FD_CLR(chord_fd,&mask_copy);
                        close(chord_fd);
                        memset(&chord, 0,sizeof(node));
                        chord_fd= 0;  
                    }
                    else printf("Invalid command\n");
                }
            } 

            //command show - shows current status
            else if(*fcommand =='s' ){
                if((strcmp(fcommand,"show")==0)||(strcmp(fcommand,"s")==0)){
                    time_t uptime =time(NULL)- entered_ring;
                    show_status(in_a_ring, me,succ,pred,chord,uptime,chord_fd,tcp_c_fd,tcp_s_fd);                    
                }
                else printf("Invalid command\n");
            }

            //command find - finds the given key; if it does is not a node
            else if(*fcommand =='f' ){
                if(split_small_command(fcommand,&command_details,0)){
                    if((strcmp(command_details.command,"find")==0)||(strcmp(command_details.command,"f")==0)){
                         check = strtol(command_details.searched_key, NULL, 10);
                        if(check <32 && check > -1){
                            check = common_code_FND_EFND(1,me,succ,pred,command_details);
                            if(check==1){//key is mine
                                printf("%s %s: %s %s (%s:%s)\n","Key ",command_details.searched_key,"node",me.key,me.IP, me.PORT); 
                            }else if (check ==0){//key is from my pred
                                printf("%s %s: %s %s (%s:%s)\n","Key ",command_details.searched_key,"node",pred.key,pred.IP, pred.PORT);
                            }else if(!check_free_position(my_searches,1,&check)){// starts a FND so need to save the searched key in the array and update the n
                                my_searches[check]= strtol(command_details.searched_key, NULL, 10);
                                sprintf(buff,"%s %s %d %s %s %s","FND",command_details.searched_key,check ,me.key,me.IP,me.PORT);
                                send_message_FND_RSP(chord_fd,tcp_s_fd,buff,succ.key,chord.key,command_details.searched_key);
                            }else printf("Can't start a FND because storage is full\n"); 
                        }else printf("The key given is not within 0 and 31\n");
                    }else printf("Invalid command\n");
                }    
            }  

            //command find - leaves the ring
            else if(*fcommand =='l'){
               if(in_a_ring){
                    if((strcmp(fcommand,"leave")==0)||(strcmp(fcommand,"l")==0)){
                        leave_ring(me,&succ,&pred,buff,&chord_fd,&tcp_c_fd,&accepted_socket,&tcp_s_fd,&mask_copy,&udp_fd,&listen_fd,&locked,&in_a_ring);        
                    }else printf("Invalid command\n");
                }else printf("Node is not in a ring - impossible execute leave command\n");
            }

            //command exit - terminates the ring
            else if(*fcommand =='e' ){
                if((strcmp(fcommand,"exit")==0)||(strcmp(fcommand,"e")==0)){
                    if(in_a_ring)leave_ring(me,&succ,&pred,buff,&chord_fd,&tcp_c_fd,&accepted_socket,&tcp_s_fd,&mask_copy,&udp_fd,&listen_fd,&locked,&in_a_ring);
                    exit_routine(&chord_fd,&tcp_c_fd,&accepted_socket,&listen_fd, &udp_fd,&tcp_s_fd); //exit => close all sockets and return 0    
                }
                else printf("Invalid command\n");
            }

            //ADITIONAL FUNCTIONS
            
            //command key - changes the key of the node without restarting the programm
            else if(*fcommand =='k'){// change my key in running time 
                if(split_small_command(fcommand,&command_details,1)){
                    if((strcmp(command_details.command,"key")==0)||(strcmp(command_details.command,"k")==0)){
                        check = strtol(command_details.key, NULL, 10);
                        if(check <32 && check > -1) {
                            if(in_a_ring){
                                strcpy(me.key,command_details.key);
                                if((strcmp(me.key,pred.key)==0)||(!in_a_ring)) new(&me,&pred,&succ,&temp_node);
                            }else strcpy(me.key,command_details.key);
                        }else printf("The key given is not within 0 and 31\n");
                    }else printf("Invalid command\n");
                }
            }

            //command key - changes the IP in running time 
            else if(*fcommand == 'i'){
                if(split_small_command(fcommand,&command_details,2)){
                    if((strcmp(command_details.command,"ip")==0)||(strcmp(command_details.command,"i")==0)){
                        if((strcmp(me.key,pred.key)==0)||(!in_a_ring)){
                            temp_me=(node*) malloc (sizeof(node));
                            temp_pred= (node*) malloc(sizeof(node));
                            strcpy(temp_me->key,me.key);
                            strcpy(temp_me->IP,command_details.IP);
                            strcpy(temp_me->PORT,me.PORT);
                            copy_node_info(&pred,temp_pred);                             
                            leave_ring(me,&succ,&pred,buff,&chord_fd,&tcp_c_fd,&accepted_socket,&tcp_s_fd,&mask_copy,&udp_fd,&listen_fd,&in_a_ring,&in_a_ring);
                            validate_ip=true;                            
                            start_routine(&me,&pred,&succ,&temp_node,&chord,&command_details,buffer,temp_me,fcommand,buff,&mask,&mask_copy,my_searches,buffer_size,EFND_saved_addr,&validate_ip);
                            copy_node_info(temp_pred,&pred);
                            free(temp_pred);
                            free(temp_me);
                            port = strtol(me.PORT, NULL, 10);
                            init_tcp_server(&listen_fd, port);
                            init_udp_server(&udp_fd, port,&udp_server_addr);
                        }else printf("Can't change the IP because the node is already in a ring with other nodes\n");
                    }else printf("Invalid command\n");
                }
            }

            //command reset - changes the key,IP and PORT in running time
            else if(*fcommand =='r'){
                if(split_command(fcommand,&command_details)){
                    if((strcmp(command_details.command,"reset")==0)||(strcmp(command_details.command,"r")==0)){
                        check = strtol(command_details.key, NULL, 10);
                        if(check <32 && check > -1) {
                            if((strcmp(me.key,pred.key)==0)||(!in_a_ring)){
                                temp_me= (node*) malloc(sizeof(node));
                                temp_pred= (node*) malloc(sizeof(node));
                                strcpy(temp_me->key,command_details.key);
                                strcpy(temp_me->IP,command_details.IP);
                                strcpy(temp_me->PORT,command_details.PORT);
                                copy_node_info(&pred,temp_pred);
                                validate_ip=in_a_ring;
                                leave_ring(me,&succ,&pred,buff,&chord_fd,&tcp_c_fd,&accepted_socket,&tcp_s_fd,&mask_copy,&udp_fd,&listen_fd,&validate_ip,&validate_ip);
                                validate_ip =true;
                                start_routine(&me,&pred,&succ,&temp_node,&chord,&command_details,buffer,temp_me,fcommand,buff,&mask,&mask_copy,my_searches,buffer_size,EFND_saved_addr,&validate_ip);
                                copy_node_info(temp_pred,&pred);
                                free(temp_pred);
                                free(temp_me);
                                port = strtol(me.PORT, NULL, 10);
                                init_tcp_server(&listen_fd, port);
                                init_udp_server(&udp_fd, port,&udp_server_addr);
                                if (in_a_ring){
                                    new(&me,&pred,&succ,&temp_node);
                                    in_a_ring=true;
                                }
                            }else printf("Can't reset because the node is already in a ring with other nodes\n");
                        }else printf("The key given is not within 0 and 31\n");
                    }else printf("Invalid command\n");
                }    
            }

            // help command - shows instructions to use the programm
            else if(*fcommand =='h'){
                if ((strcmp(fcommand,"help")==0)||(strcmp(fcommand,"h")==0)){
                    help();  
                }
            }

            else printf("Command given, \" %s\",  is not valid.\n", fcommand); 
        }

        //accepts tcp connection
        else if(listen_fd && FD_ISSET(listen_fd,&mask)){
            FD_CLR(listen_fd, &mask_copy);
            if(accepted_socket!=0)close(accepted_socket);
            accepted_socket = accept(listen_fd, (SA*)&tcp_client_addr, &len);
            setsockopt(accepted_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof (timeout));
            if (accepted_socket < 0) {
                printf("Server accept failed.\n");
                exit(0);
            }    
        }     

        //udp connection to process FND, RSP, EFND and EPRED(is also used to send EPRED)and sends ACK
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
                                        EFND_saved_addr,my_searches,&tcp_c_fd,&tcp_servaddr,udp_server_addr,&pentry_or_new);
                }
            }
        }

        //udp connection to process EPRED(is also used to send EFND) and sends ACK
        else if(chord_fd && FD_ISSET(chord_fd,&mask)){
            FD_CLR(chord_fd, &mask_copy);
            if((n = recv(chord_fd, (char *)buff, message_size, MSG_WAITALL)) !=0){
                send(chord_fd, "ACK", 4, MSG_CONFIRM);
                buff[strcspn(buff, "\n")] = 0;
                printf("%s\n", buff);
                if (*buff=='E'){
                    process_EFND_EPRED(in_a_ring,me,&pred, succ,chord,&command_details,buff,tcp_s_fd,chord_fd,udp_fd,
                                        EFND_saved_addr,my_searches,&tcp_c_fd,&tcp_servaddr,udp_server_addr,&pentry_or_new);
                }     
            }
            close(chord_fd);
            chord_fd=0;
        }

        //reads message from tcp_c_fd socket, copies it to a buffer and then split's when finds a \n, processing one command at a time
        //processes SELF 
        else if(accepted_socket && FD_ISSET(accepted_socket, &mask)){
            FD_CLR(accepted_socket, &mask_copy);
            if((n = read(accepted_socket, buffer, message_size)) != 0){
                if(n == -1){
                    printf("Reading error.\n");
                    exit(1);
                }
                buffer_to_split = buffer_to_free= strdup(buffer);
                do{ 
                    pointer=0;
                    if((tester=strchr(buffer_to_split,'\n')) == NULL){
                        if(*buffer_to_split!=0){
                            memset(buffer,0,buffer_size);
                            strcpy(buffer,buffer_to_split);
                            free(buffer_to_free);
                            pointer = strlen(buffer);
                            read(tcp_c_fd, (buffer+pointer), (buffer_size-pointer));
                            if(sizeof(buffer)==pointer)memset(buffer,0,buffer_size);
                            buffer_to_split = buffer_to_free = strdup(buffer);
                        }
                    }
                    handler = strsep(&buffer_to_split,"\n");
                    if(handler!=0){
                        strcpy(buff,handler);
                        buff[strcspn(buff, "\n")] = 0;
                        printf("%s\n", buff);
                        
                        if (*buff =='S'){
                            if (pentry_or_new){
                                split_SELF_PRED( buff, &temp_node,"SELF",&command_details);
                                if(in_a_ring){
                                    if((strcmp(me.key, succ.key))==0){
                                        sprintf(buff,"%s %s %s %s\n","SELF", me.key,me.IP,me.PORT);
                                        copy_node_info(&temp_node,&pred);
                                        tcp_client(&tcp_c_fd,&tcp_servaddr,temp_node.PORT,temp_node.IP,buff);                                
                                    }else {
                                        sprintf(buff,"%s %s %s %s\n","PRED", temp_node.key,temp_node.IP,temp_node.PORT);
                                        write(tcp_s_fd,buff,strlen(buff));                            
                                    }
                                }else{
                                    in_a_ring=1;
                                    entered_ring = time(NULL);
                                } 
                                copy_node_info(&temp_node,&succ);
                                tcp_s_fd = accepted_socket;
                                
                                
                            }else close(accepted_socket);
                            accepted_socket=0;
                        } 
                    }         
                }while(*buffer_to_split!='\0'); 
                free(buffer_to_free);
            }else {
                close(accepted_socket);
                accepted_socket= 0;
            }
        }

        //reads message from tcp_c_fd socket, copies it to a buffer and then split's when finds a \n, processing one command at a time
        // processes PRED , FND and RSP
        else if(tcp_c_fd && FD_ISSET(tcp_c_fd, &mask)){
            FD_CLR(tcp_c_fd, &mask_copy);
            if((n = read(tcp_c_fd, buffer, (buffer_size-1))) != 0){
                if(n == -1){
                    printf("Reading error.\n");
                    exit(1);
                }
                buffer_to_split = buffer_to_free= strdup(buffer);
                do{ 
                    pointer=0;
                    if((tester=strchr(buffer_to_split,'\n')) == NULL){
                        if(*buffer_to_split!=0){
                            memset(buffer,0,buffer_size);
                            strcpy(buffer,buffer_to_split);
                            free(buffer_to_free);
                            pointer = strlen(buffer);
                            read(tcp_c_fd, (buffer+pointer), (buffer_size-pointer));
                            if(sizeof(buffer)==pointer)memset(buffer,0,buffer_size);
                            buffer_to_split = buffer_to_free = strdup(buffer);
                        }
                    }
                    handler=strsep(&buffer_to_split,"\n");
                    if(handler!=0){
                        strcpy(buff,handler);
                        buff[strcspn(buff, "\n")] = 0;
                        printf("%s\n", buff);
                        if(*buff=='P'){ 
                            split_SELF_PRED(buff,&pred,"PRED",&command_details);
                            sprintf(buff,"%s %s %s %s\n","SELF", me.key,me.IP,me.PORT);
                            close(tcp_c_fd);
                            tcp_client(&temp_tcp_c_fd,&tcp_servaddr,pred.PORT, pred.IP,buff);
                            tcp_c_fd=temp_tcp_c_fd;
                            temp_tcp_c_fd=0;
                        }
                        else if((*buff=='F')||(*buff=='R')){
                            process_FND_RSP(buff,me,succ,pred,chord,tcp_s_fd,chord_fd,my_searches,EFND_saved_addr,udp_fd);
                        } 
                    }      
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
