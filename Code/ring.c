
#include "header.h"
// types of comments
// !
// ?
// TODO:
// // 
// *

//! 
//TODO on udp recieved messages remove the \n by default !  use strcspn(): (basicly find the position of the \n and put a \0 there 

//!reminder to always check for bugs before futher development
//TODO0:implementar select
// implica implementar pelo menos tcp código escrito de forma independente ao ring para já
//? para bentry têm que ser feito o store do address que contactou (por causa das chords)
//* guardar num addr_bentry ou algo do genero!

//TODO1: pesquisa de chave mensagens usadas 
//*FND k n i i.IP i.port e RSP k n i i.IP i.port
/*
char output[50];
int length;
int n;*/
//* split string into x commands using the split_str_nd_copy_to_new_location function string is the command recieved by tcp/udp
/*str = strdup(message);
command_details->command=split_str_nd_copy_to_new_location(&str);
* enought times to make it for the full command
? or maybe just re-use the command to send message have to check here

length = sprintf(output, "%s %s %s %s %s %s\n", "FND",command.searched_key, command.n, command.key,command.IP, command.PORT);
write(socket, output, length);

printf("%s",output);
printf("%s",output);
return 0;
*/
//* onde k é o procurado// destinatario
//* n número de sequencia 
//* i i.ip i.port // info de quem iniciou a procura/resposta
//* se tcp meter \n no final da string, se UDP responder com ACK
// 1 2 3 4 5 6 
//TODO2: saída e entrada de um  nó
//* PRED pred pred.IP pred.port\n ///de me para 
//* SELF i i.ip i.port\n ///de me para pred
// 1 2 3 4
//TODO3:descoberta de posição no anel
// para este basta apenas um check por isso no biggies :D 
// because se receber um E e estiver no anel significa EFND k
// else é i EPRED DE RESPOSTA :D
//*EFND k (udp) Realizado aquando da bentry
//* EPRED pred pred.ip pred.port
//* ACK
// recieves 2 arguments
// starts a find with 6 arguments 
//todo4: implement the code for all the ^actions that converts the info into a string using the following code as eg.

//todo now implement the tcp client and udp client and set up 2/3 messages to test

// creates the socket to with the corect type (true = SOCK_DGRAM= UDP false = SOCK_STREAM=TCP) and checks if the creation was sucessful 

// gets line from stdin given by user
void get_info_from_client( char* fcommand){
    size_t len = 10000; //! change this size tho we still will be needing a +-  big size
    getline(&fcommand,&len,stdin); // * gets a full line until '\n' is met
    fcommand[strlen(fcommand)-1]= '\0';
}

// // asdasdasdasd
//! pbbly to remove 

//* para todos os comandos fazer uma função que recebe uma string
//* qqr e a manda corretamente no canal tcp/udp necessário
//* chamada da func será algo tipo
//* send_message(mode, message, (node)destinatario, socket) em que mode indica tcp ou UDP
//* 
//? fora do send 
//* ter um static counter 
//* que vai de 0 A 99

/*void recieve_message(int socket, bool mode, message_info_t* message_info){
    if(mode){// UDP
        recvfrom(socket, (const char )message_info->message, strlen(message_info->message),
             MSG_CONFIRM, (const struct sockaddr *) &message_info->addr,  &message_info->addr_len);
        sendto(socket, "ACK", strlen("ACK"),
             MSG_CONFIRM, (const struct sockaddr *) &message_info->addr,  message_info->addr_len);
    }
    else{
        // read the message from client and copy it in message
        read(socket, message_info->message, sizeof(message_info->message));


    }
    //? need to check if should already split the string to acess commands here/ destination etc
}*/
/*
void send_message (bool mode, char* message, node destinatario, int socket){
    // * extrair  address no modo de UDP caso contrário não é preciso...
    if (mode ){//UDP
        //!from here
        //socklen_t len = sizeof(*servaddr);
        struct sockaddr_in  addr;       
        // Filling server information
        addr.sin_family = AF_INET;
        addr.sin_port = htons(destinatario.PORT);
        inet_pton( AF_INET, destinatario.IP, & addr.sin_addr);
        socklen_t len = sizeof(addr);
        //! message[strlen(message) -1] ='\0'; 
        sendto(socket, (const char )message, strlen(*message),
             MSG_CONFIRM, (const struct sockaddr *) &addr,  len);
        //TODO: meter aqui um recv ou recvfrom e verificar se recebeu ACK caso não receba em 1 segundo reenvia a mensagem

        //! till here
    }
    else{// TCP
        
        write(socket,message,sizeof(message));
    }

}
*/

//! pbbly to remove 
//// dasdasdasd


//prints the current status of the client with the relevant node/ring information 
void show_status(bool in_a_ring, node me, node succ, node pred, node* chord,time_t uptime){
    system("clear");
    printf("%-15s\t%-15s\t%-15s\n\n"," "," ","STATUS");
    
    printf("%-15s\t%-15s\t%-15s\t%-15s\n","     ", "KEY","IP","PORT");
    printf("mine:       \t%-15s\t%-15s\t%-15s\n",me.key , me.IP,me.PORT); 
    if(in_a_ring){
        printf("sucessor:   \t%-15s\t%-15s\t%-15s\n",succ.key , succ.IP,succ.PORT);
        printf("predecessor:\t%-15s\t%-15s\t%-15s\n",pred.key , pred.IP,pred.PORT);
        if(chord !=NULL) printf("atalho:\t%-15s\t%-15s\t%-15s\n",chord->key , chord->IP,chord->PORT);
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
int check_distance(char* my_key, char* dest_key){
    int my_key_n = strtol(my_key, NULL, 10);// * converts the my_key str into decimal
    int dest_key_n = strtol(dest_key, NULL, 10);// * converts the dest_key str into decimal
    int result = (dest_key_n-my_key_n)%MAX_KEY;// * determines the distance between the keys formula= (n-j)mode 32 
    if (result <0) result += MAX_KEY; //* if the distance is negative adds 32.
    return result;//* returns the distance value
}

//Checks if IP and PORT are valid
// ! might have some bug have to re-check it
bool valid_IP_nd_port(char* IP, char* PORT){
    bool port_validator, ip_validator;// * control variables 
    int  check;
    void * ipvalidation=(void*) malloc(sizeof(IP));

    ip_validator = (bool)inet_pton( AF_INET, IP, ipvalidation );// * inetpton returns 0 incase the given ip is not valid 
    //! might not be worth to do at least for tcp/udp connections (because when getting the adress info inet_pton will be used )
    check = strtol(PORT, NULL, 10);//* converts the port into decimal
    if(check <65536 && check > 0) port_validator = true;//* checks if the port is between PORT max and min values
    //! not sure if needed or if all this ports can actually be used in this program context
    else port_validator = false;
    return (ip_validator && port_validator);//* returns true if both IP and PORT are valid, false otherwise
}

// copys the data from one node to other, useful when entering the ring, recieving new person or creating a ring/chord
void copy_node_info(node *origin, node *copy){
    copy->IP= origin->IP;
    copy->key= origin->key;
    copy->PORT = origin->PORT;
}

//routine for creating a new node, has to initialize the successor and predecessor nodes
// TODO: add more stuff need to check what is missing for new (pbbly had stuff to the select mask (the new sockets from the udp/tcp(listen_sd) servers))
// * most likely start a tcp/udp server listening to possible connections 
void new(node *me, node *pred, node *succ){
    copy_node_info(me,pred);// * initialize predecessor = me
    copy_node_info(me,succ);// * initialize successor = me
    // ?start tcp client and server(connect one to the other is unecessary(?)(have a variable knowing node is only one node)) + udp server
}

// split the given string and stores it on a new string
char* split_str_nd_copy_to_new_location(char** str_to_split){
    char* splitted;
    char *str_to_save;
    splitted = strsep(str_to_split, " "); 
    if (splitted == NULL){// do routine for bad command 
        printf("%s given is incomplete program will terminate\n" ,"command");
        exit(0);
    }

    str_to_save = strdup (splitted);
    return str_to_save;
}

//!fucking up on the ip_nd_port_check 
// splits the command given by user saving it in a structure while checking if the command is valid
void split_command(char * fcommand, command_details_t* command_details){
    //char* splitted=NULL;
    char* str=NULL;
    //bool valid;
    //? malloc of the strings inside command is needed or not? 
    str=(char*) malloc(sizeof(char)*100 );
    str = strdup(fcommand);

    command_details->command=split_str_nd_copy_to_new_location(&str);
    command_details->key=split_str_nd_copy_to_new_location(&str);
    command_details->IP=split_str_nd_copy_to_new_location(&str);
    command_details->PORT=split_str_nd_copy_to_new_location(&str);
    
    if(str != NULL){// * do routine for bad command 
        printf("%s given is too big program will terminate", fcommand);
        exit(0);
    }
    //valid = valid_IP_nd_port(command_details->IP,command_details->PORT);
    //if(!valid){
       //  printf("%s given has worng port or ip program will terminate", fcommand);
       // exit(0);
        // do routine for bad command 
     // }
    free(str);
}

// determines if a given object is mine
bool find_key(char * fcommand,char* me, char* succ){
    char* splitted;
    char* str =(char*) malloc(sizeof(char) *100);
    str = strdup(fcommand);// * copies fcommand into str 
    splitted = strsep(&str, " ");// * splits str stopping at each space (" ")
    splitted = strsep(&str, " ");
    if (splitted == NULL){ // * do routine for bad command 
        printf("%s given is incomplete program will terminate", fcommand);
        exit(0);
    }
    free(str);
    int dist_me_k = check_distance(me, splitted);// * determines distance between me and object
    int dist_succ_k= check_distance(succ,splitted);// * determines distance between me and object
     if (dist_me_k <= dist_succ_k)// * if i am closer then object is mine
         return true;
    return false;
}

// returns the max ammong given int's
int max_all(int sock_1,int sock_2, int sock_3,int sock_4){
    sock_1= max(sock_1,sock_2);
    sock_1= max(sock_1,sock_3);
    sock_1= max(sock_1,sock_4);
    return sock_1;
}

//starting routine gets the argv and checks it's validity storing it in me
void start_routine(node* me, node*pred, node* succ,command_details_t* command_details,char**argv){
    memset(me,0,sizeof(node));
    memset(pred, 0,sizeof(node));
    memset(succ, 0,sizeof(node));
    memset(command_details, 0,sizeof(command_details_t));   
    me->key= strdup(argv[1]);
    me->IP= strdup(argv[2]);
    me->PORT= strdup(argv[3]);
    if (valid_IP_nd_port(me->IP, me->PORT)){
        printf("%s\t%s\t%s\n",me->key , me->IP,me->PORT);   
    }
    else printf("IP or Port provided is not valid");//* exit(0) or reenter... 
}


void split_FND_RSP(command_details_t* command,char **str){
    char *splitted;
    splitted= split_str_nd_copy_to_new_location(str);
    command->command= strdup(splitted);
    splitted= split_str_nd_copy_to_new_location(str);
    command->searched_key= strdup(splitted);
    splitted= split_str_nd_copy_to_new_location(str);
    command->n= strdup(splitted);
    splitted= split_str_nd_copy_to_new_location(str);
    command->key= strdup(splitted);
    splitted= split_str_nd_copy_to_new_location(str);
    command->IP= strdup(splitted);
    splitted= split_str_nd_copy_to_new_location(str);
    command->PORT= strdup(splitted);
}

void split_self_pred_m(char*buff, node * node, char* compare){
    char *str=strchr(buff,'\n');
    char *splitted;
    str =strdup(buff);
    splitted= split_str_nd_copy_to_new_location(&str);
    if(strcmp(splitted,compare)==0){
        splitted= split_str_nd_copy_to_new_location(&str);
        node->key= strdup(splitted);
        splitted= split_str_nd_copy_to_new_location(&str);
        node->IP= strdup(splitted);
        splitted= split_str_nd_copy_to_new_location(&str);
        node->PORT= strdup(splitted);     
    }  
}
                 
//! working late so need to recheck + comment from here to
int process_FND_RSP(char* buff,node me,node succ,node* chord){//? maybe pass command as arg so i can have access to it outside this func(most likely + easy change so for now i'll keep like is)
    char *str=strchr(buff,'\n');
    command_details_t command;
    str =strdup(buff);
    split_FND_RSP(&command,&str);
    if (strcmp(command.command,"FND")==0){
        //TODO FND ROUTINE(CASE FOR ME, SUCC OR CHORD)
        if(check_distance(command.searched_key,me.key)==0){
            sprintf(buff,"%s %s %s %s %s %s","RSP",command.searched_key, command.n,me.key ,me.IP, me.PORT);
            //sprintf(output, "%s %s %s %s %s %s\n", "FND",command.searched_key, command.n, command.key,command.IP, command.PORT);
        }
        if(chord==NULL){
            return 1;
        }
        else if(check_distance(command.searched_key,succ.key)<=check_distance(command.searched_key,chord->key)){
            return 1;//! for now 1=tcp
        }
        else{
            strcat(buff,"\n");
            return 2;
        }
    }
    else if(strcmp(command.command,"RSP")==0){
        //TODO RSP ROUTINE(CASE FOR ME, SUCC OR CHORD)
        if (strcmp(me.key,command.searched_key)==0){
            //todo resposta para o gajo udp que me contactou a dizer a resposta ou simplesmente mostra ao user caso seja essa a situação
            // o check vai ser feito pelo n que a busca tiver... tenho que criar ums estrutura para guardar a info de cada pesquisa (talvez uma array de 100 posições)
            //preciso de guardar o adress (cus it's udp only) portanto uma array de addresses chega e o n é incremental e guarda lá dentro o address maybe ? 
            //todo algumas ideias (cansado para implementar atm )
            return 3;
        }
        if(chord==NULL){
            return 1;
        }
        else if(check_distance(command.searched_key,succ.key)<=check_distance(command.searched_key,chord->key)){
            return 1;//! for now 1 = tcp
        }
        else{
            strcat(buff,"\n");
            return 2;
        }
    }
    return 0;
}
//! here 

void process_EFND_EPRED(bool in_a_ring,node *pred, command_details_t *command,char* buff){
//todo all this func is where i stopped for now 
    char *str=strchr(buff,'\n');
    str =strdup(buff);
    char *splitted;
    splitted= split_str_nd_copy_to_new_location(&str);
    command->command= strdup(splitted);
    if (in_a_ring){
        if (strcmp("EFND",command->command)==0){
           splitted= split_str_nd_copy_to_new_location(&str);
           command->key=strdup(splitted);
           
       
           //! start a search for the this key  
        }    
    }else if(strcmp("EPRED",command->command)==0){
        //
        splitted= split_str_nd_copy_to_new_location(&str);
        pred->key=strdup(splitted);
        splitted= split_str_nd_copy_to_new_location(&str);
        pred->IP=strdup(splitted);
        splitted= split_str_nd_copy_to_new_location(&str);
        pred->PORT=strdup(splitted);

    }
}

//initializes the mask with the req fd's
void mask_init(fd_set *mask_copy,int udp_fd,int listen_fd,int tcp_s_fd,int chord_fd){
    FD_SET(0,mask_copy);
    if(listen_fd) FD_SET(listen_fd, mask_copy);
    if(udp_fd)FD_SET(udp_fd, mask_copy);
    if(tcp_s_fd)FD_SET(tcp_s_fd, mask_copy);
    if(chord_fd)FD_SET(chord_fd, mask_copy);   
}

void  init_tcp_server(int *listen_fd, int PORT){
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
	servaddr.sin_port = htons(PORT);//! mandar por argumento

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

void  init_udp_server(int *udp_fd, int PORT){
	SA_in servaddr;
// Creating socket file descriptor
    if ( (*udp_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
       
    // Filling server information
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);//! mandar por argumento
       
    // Bind the socket with the server address
    if ( bind(*udp_fd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}

void tcp_message(int *sockfd, int mode, char** message)
{
	
	if(mode == 0) {
			write(*sockfd, *message, 100000);
	}
	else{
		read(*sockfd, *message, 100000);
		printf("From Server : %s", *message);
	}
}

void init_tcp_client(int *sockfd,SA_in*servaddr, char * port_, char*addr){
    if ( (*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1 ) {
        perror("socket creation failed");
        exit(0);
    }
    // Filling server information
    servaddr->sin_family = AF_INET;
    int port = strtol(port_, NULL, 10);
    servaddr->sin_port = htons(port);
		if (inet_pton( AF_INET, addr,  (in_addr_t*) &servaddr->sin_addr.s_addr) < 1) {
		//inet_pton - convert IPv4 and IPv6 addresses from text to binary form 
		// returns 0 if given adress isn't valid 
		printf("Adress not valid.\n");
		exit(-1);
  	} 
	  if (connect(*sockfd, (SA*)servaddr, sizeof(*servaddr)) != 0) {
				printf("Connection with the server failed.\n");
				exit(0);
			}
			else
				printf("Connected to the server.\n");
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        printf("to run client supply ring key, IP and port, separated by a single space \n");
        exit(0);
    } 
    // VAR INIT
    fd_set mask, mask_copy;
    int udp_fd=0, listen_fd=0,tcp_s_fd=0,maxfd=0,chord_fd=0,port, n;
    time_t entered_ring= time(NULL);
    node me, pred,succ, temp_node, * chord = NULL;
    command_details_t command_details;
    //! tcp client test
    SA_in servaddr;
    int sockfd ;
    char* buffer= (char*)malloc(sizeof(char)*100);
    size_t len_tcp_cli;
    //!
    bool in_a_ring =false;
    char* fcommand=(char*) calloc(1,sizeof(char) *100);
    SA_in tcp_client, udp_client;
    //todo mudar variaveis para definitivas no tpc e udp
    char buff[100];
    socklen_t len = sizeof(SA_in);
    // VAR INIT

    start_routine(&me,&pred,&succ,&command_details,argv);
    port = strtol(me.PORT, NULL, 10);
    init_tcp_server(&listen_fd, port);
    init_udp_server(&udp_fd, port);
    //!mask_init(&mask_copy,udp_fd,listen_fd,tcp_s_fd,chord_fd);
    for(;;){
        mask_init(&mask_copy,udp_fd,listen_fd,tcp_s_fd,chord_fd);
        //FD_ZERO(&mask);
        mask = mask_copy;
        maxfd= max_all(udp_fd,listen_fd,tcp_s_fd,chord_fd);
        select(maxfd+1, &mask, (fd_set*)NULL, (fd_set*)NULL, (struct timeval *)NULL);
        if(FD_ISSET(0,&mask)){
            FD_CLR(0, &mask_copy);
            get_info_from_client( fcommand);
            if (*fcommand == 'n'){/* creates a new, empty ring where the node will ocupy the given key position per default */
                if(!in_a_ring){
                    in_a_ring =true; 
                    entered_ring = time(NULL);
                    new(&me, &succ,&pred);
                    
                }
                else printf("Already in a ring u dumbass\n");
            }
                
            else if (*fcommand =='b' ){/*enters the ring knowing only one random node*/
                split_command(fcommand,&command_details);
            }
                
            else if (*fcommand == 'p' ){/*enters the ring know it's pred_key*/
                split_command(fcommand,&command_details);
            }
                
            else if (*fcommand =='c' ){/*creates a udp shortcut to a given key, key_ip, key_port*/ 
                
                split_command(fcommand,&command_details);/*need to add a checker for valid command so it can exit nicely*/
                //chord = (node*)malloc(sizeof(node));
                // memset(&chord, 0,sizeof(node));
                // 

            }
                
            else if(*fcommand == 'd' /*'e'*/){//echord 
              //!test code for client 
               //while(1){
                    init_tcp_client(&sockfd,&servaddr,"127.0.0.1"/*argv[3]*/,"19" );
                    getline(&buffer,&len_tcp_cli,stdin);
                    buffer[strlen(buffer)-1]= '\0';
                    tcp_message(&sockfd,0, &buffer);
                    tcp_message(&sockfd,1,&buffer);
                    printf("\n%d\n",sockfd);
                    close(sockfd);
              // }
               
                /*close(chord_fd);
                free(chord);
                chord = NULL;
                chord_fd =0;*/
            }/*problema porque e é short form para exit && echord prof tem que mudar*/
                
            else if(*fcommand =='s' ){// show current status
                    time_t uptime =time(NULL)- entered_ring;
                    show_status(in_a_ring, me,succ,pred,chord,uptime);
            }
                
            else if(*fcommand =='f' ){
                if(find_key(fcommand, me.key, succ.key)){// verifies if it is mine or not
                    //send_message()
                }
            }
                
            else if(*fcommand =='l'){
                memset(&pred, 0,sizeof(node));
                memset(&succ, 0,sizeof(node));
                free(chord); 
                in_a_ring= false;
            }

            else if(*fcommand =='e' ){// exit
                if (chord_fd) close(chord_fd);
                free(chord);
                close(listen_fd);
                close(udp_fd);
                exit(0);
                //exit => close all sockets and return 0
            }/*problema porque e é short form para exit && echord prof tem que mudar*/

            else{
            printf("Command given, \" %s\",  is not valid.\n", fcommand);
            }
        }
        else if(FD_ISSET(listen_fd,&mask)){
            FD_CLR(listen_fd, &mask_copy);
            tcp_s_fd = accept(listen_fd, (SA*)&tcp_client, &len);
            if (tcp_s_fd < 0) {
                printf("Server accept failed.\n");
                exit(0);
            }
        }    
        else if(FD_ISSET(tcp_s_fd, &mask)){
            FD_CLR(tcp_s_fd, &mask_copy);
            if((n = read(tcp_s_fd, buff, 100)) != 0){
                if(n == -1){
                    printf("Reading error.\n");
                    exit(1);
                }
                printf("%s\n", buff);
                if (*buff =='S'){
                    
                    if(in_a_ring){
                        split_self_pred_m( buff, &temp_node,"SELF");
                        //todo  send message to succ with a PRED
                        sprintf(buff,"%s %s %s %s\n","PRED", temp_node.key,temp_node.IP,temp_node.PORT);
                        copy_node_info(&temp_node,&succ);
                    }
                    else split_self_pred_m( buff, &succ,"SELF");
                    in_a_ring =true;
                }
                else if(*buff=='P'){
                    split_self_pred_m(buff,&pred,"PRED");
                    //todo
                    sprintf(buff,"%s %s %s %s\n","SELF", me.key,me.IP,me.PORT); 
                    // get addr info or bind info from the pred
                    //!send a self to the pred
                }
                else if((*buff='F')||(*buff='R')){
                    process_FND_RSP(buff,me,succ,chord);
                }
                write(tcp_s_fd,buff,sizeof(buff));
                
            }
            close(tcp_s_fd);
            tcp_s_fd = 0;
        }
        else if(FD_ISSET(udp_fd,&mask)){
            FD_CLR(udp_fd, &mask_copy);
            //FD_CLR(tcp_s_fd, &mask);
            //static int i =0;
            n = recvfrom(udp_fd, (char *)buff, 100, 
                MSG_WAITALL, ( struct sockaddr *) &udp_client,
                &len);
            sendto(udp_fd, "ACK", strlen("ACK"), 
                MSG_CONFIRM, (const struct sockaddr *) &udp_client,
                    len);
            if((*buff=='F')||(*buff=='R')){
                process_FND_RSP(buff,me,succ,chord);
            }
            else if (*buff=='E'){
                process_EFND_EPRED(in_a_ring,&pred,&command_details,buff);
            }
        }    
    }
    return 0;    
}

