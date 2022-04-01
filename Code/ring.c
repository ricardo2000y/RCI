#include "header.h"
// types of comments
// !
// ?
// TODO:
// // 
// *

//! 
//TODO on udp recieved messages remove the \n by default !  use strcspn(): (basicly find the position of the \n and put a \0 there 
//*^code is used in tcp so just implement for udp aswell ! 
// implica implementar pelo menos tcp código escrito de forma independente ao ring para já
//? para bentry têm que ser feito o store do address que contactou (por causa das chords)
//* guardar num addr_bentry ou algo do genero!

//TODO1: pesquisa de chave mensagens usadas 
//*FND k n i i.IP i.port e RSP k n i i.IP i.port
//todo array/ struct to save udp clients addresses (maybe also saving their key(or associate a number to them idk))
//  max number of udp clients to my server that i need to save is the ones from outise the ring trying to connect so maybe 5 if i see we need more up to 10 or just say server is full atm try later idk? 
// todo make a list of stuff that needs a timeout 
//todo criar array com número de sequencia 
//? verificar novamente o formato do comando para garantir que no RSP o searched key é o destinatário(quem iniciou a procura)
//* onde k é o procurado// destinatario
//* n número de sequencia 
//* i i.ip i.port // info de quem iniciou a procura/resposta
//* se tcp meter \n no final da string, se UDP responder com ACK
// 1 2 3 4 5 6 
//TODO2: saída e entrada de um  nó
//! para saida verificar se EOF é lido
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

//! making quick resume of the connections so i don't lose myself on making the functions
/*
todo TCP

tcp_c_fd  //*  connects to the pred 
sends:SELF
recieves: PRED,FND,RSP ---
//////////////////////////////////
accepted_fd //* accepts a connected from a new client(aka new succ)
sends: nothing //?
recieves:SELF(then binds this to the tcp_s_fd)
//////////////////////////////////
tcp_s_fd//* connected to the succ
sends:PRED, FND, RSP
recieves:nothing //?  

todo UDP
udp_c_fd(//?chord_fd)  
//* connects to a chord if one exists
sends:FND,RSP,"ACK" //* ACK IS ONLY WHEN IT RECIEVES A EPRED
recieves:"ACK",EPRED

udp_s_fd
sends:"ACK", EPRED
recieves:"ACK",FND,RSP, EFND//* when EFND we need to store the addres (maybe store up to 5 addresses(?))

*/
//! making quick resume of the connections so i don't lose myself on making the functions



// gets line from stdin given by user
void get_info_from_client( char* fcommand){
    size_t len = 1000; 
    getline(&fcommand,&len,stdin); // * gets a full line until '\n' is met
    fcommand[strlen(fcommand)-1]= '\0';//* removes the \n 
}

//prints the current status of the client with the relevant node/ring information 
void show_status(bool in_a_ring, node me, node succ, node pred, node* chord,time_t uptime){
    system("clear");
    printf("%-15s\t%-15s\t%-15s\n\n"," "," ","STATUS");
    printf("%-15s\t%-15s\t%-15s\t%-15s\n","     ", "KEY","IP","PORT");
    printf("mine:       \t%-15s\t%-15s\t%-15s\n",me.key , me.IP,me.PORT); 
   if(in_a_ring){
        printf("sucessor:   \t%-15s\t%-15s\t%-15s\n",succ.key , succ.IP,succ.PORT);
        printf("predecessor:\t%-15s\t%-15s\t%-15s\n",pred.key , pred.IP,pred.PORT);
        if(chord->IP == 0) printf("atalho:\t%-15s\t%-15s\t%-15s\n",chord->key , chord->IP,chord->PORT);
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
    char ipvalidation[20];
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
    strcpy(copy->IP,origin->IP);
    strcpy(copy->key,origin->key);
    strcpy(copy->PORT,origin->PORT);
}

//routine for creating a new node, has to initialize the successor and predecessor nodes
// TODO: add more stuff need to check what is missing for new (pbbly had stuff to the select mask (the new sockets from the udp/tcp(listen_sd) servers))
// * most likely start a tcp/udp server listening to possible connections 
void new(node *me, node *pred, node *succ,node* temp_node){
    copy_node_info(me,pred);// * initialize predecessor = me
    copy_node_info(me,succ);// * initialize successor = me
    copy_node_info(me,temp_node);
}

// split the given string and stores it on a new string
void split_str_nd_copy_to_new_location(char** str_to_split,char* str_to_save){
    if(*str_to_split!=NULL){
        char*splitted = strsep(str_to_split, " "); 
        if (splitted == NULL){// do routine for bad command 
            printf("%s given is incomplete program will terminate\n" ,"command");
            exit(0);
        }
        strcpy(str_to_save ,splitted);
    }
}

// splits the command given by user saving it in a structure while checking if the command is valid
void split_command(char * fcommand, command_details_t* command_details){
    //? malloc of the strings inside command is needed or not? 
    memset(command_details,0,sizeof(command_details_t));
    char* str_to_free ,*str_to_split;
    str_to_free=str_to_split=strdup(fcommand);
    split_str_nd_copy_to_new_location(&str_to_split,command_details->command);
    split_str_nd_copy_to_new_location(&str_to_split,command_details->key);
    split_str_nd_copy_to_new_location(&str_to_split,command_details->IP);
    split_str_nd_copy_to_new_location(&str_to_split,command_details->PORT);
    printf("%s\n%s\n%s\n%s\n",command_details->command, command_details->IP, command_details->key, command_details->PORT);
    if(str_to_split != NULL){// * do routine for bad command 
        printf("%s given is too big program will ignore", fcommand);
        //free(str_to_free);
        //! do something here
    }
    free(str_to_free);
}

// determines if a given object is mine
bool find_key(char * fcommand,char* me, char* succ, command_details_t* command_details){

    char* str_to_free ,*str_to_split;
    str_to_free=str_to_split=strdup(fcommand);
    split_str_nd_copy_to_new_location(&str_to_split,command_details->command);
    split_str_nd_copy_to_new_location(&str_to_split,command_details->key);
    if (str_to_split != NULL){ // * do routine for bad command 
        printf("%s given is incomplete program will terminate", fcommand);
        //! do something here
        //free(str_to_free);
    }
    free(str_to_free);
    int dist_me_k = check_distance(me, command_details->key);// * determines distance between me and object
    int dist_succ_k= check_distance(succ,command_details->key);// * determines distance between me and object
     if (dist_me_k <= dist_succ_k)// * if i am closer then object is mine
         return true;
    return false;
}

//starting routine gets the argv and checks it's validity storing it in me
void start_routine(node* me, node*pred, node* succ,node* temp_node,node* chord,command_details_t* command_details,
                    char** argv,char* fcommand,char* buff,fd_set* mask,fd_set* mask_copy){
    memset(me,0,sizeof(node));
    memset(pred, 0,sizeof(node));
    memset(succ, 0,sizeof(node));
    memset(temp_node,0,sizeof(node));
    memset(chord,0,sizeof(node));
    memset(command_details, 0,sizeof(command_details_t));
    memset(fcommand,0,sizeof(char)*100);
    memset(buff,0,sizeof(char)*100);
    memset(mask,0,sizeof(fd_set));
    memset(mask_copy,0,sizeof(fd_set));   
    strcpy(me->key,argv[1]);
    strcpy(me->IP,argv[2]);
    strcpy(me->PORT,argv[3]);
    if (valid_IP_nd_port(me->IP, me->PORT)){
        printf("%s\t%s\t%s\n",me->key , me->IP,me->PORT);   
    }
    else printf("IP or Port provided is not valid");
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
    printf("%s\n",command_details->command);
    if(strcmp(command_details->command,compare)==0){
        split_str_nd_copy_to_new_location(&str_to_split,node->key);
        split_str_nd_copy_to_new_location(&str_to_split,node->IP);
        split_str_nd_copy_to_new_location(&str_to_split,node->PORT);
    } 
    free(str_to_free); 
}

//todo put tcp searching working 
//* i can answer for me ( if my distance to searched is < than my_succ to searched key  then the key is mine)
//* can awser for pred incase of : a EFND!! this node starts a find k
//* and decide where to send based on distance

//! working late so need to recheck + comment from here to
int process_FND_RSP(char* buff,node me,node succ,node pred,node* chord){//? maybe pass command as arg so i can have access to it outside this func(most likely + easy change so for now i'll keep like is)
    command_details_t command;
    char str[100] ;
    strcpy(str,buff);
    split_FND_RSP(&command,str);
    int succ_distance =check_distance(command.searched_key,succ.key);
    int me_distance= check_distance(command.searched_key,me.key);
    if (strcmp(command.command,"FND")==0){
        //TODO FND ROUTINE(CASE FOR ME, SUCC OR CHORD)
        if(me_distance<= succ_distance){
            sprintf(buff,"%s %s %s %s %s %s","RSP",command.searched_key, command.n,me.key ,me.IP, me.PORT);
            //sprintf(output, "%s %s %s %s %s %s\n", "FND",command.searched_key, command.n, command.key,command.IP, command.PORT);
        }else if(check_distance(command.searched_key,pred.key)<= me_distance){
            //! sitio errado mas ideia certa (basicamente testar quando recebe um EFND k)
            sprintf(buff,"%s %s %s %s %s %s","RSP",command.searched_key, command.n,pred.key, pred.IP, pred.PORT);
        }
        if(chord==NULL){
            return 1;
        }
        else if(succ_distance<=check_distance(command.searched_key,chord->key)){
            return 1;//! for now 1=tcp
        }
        else{
            //strcat(buff,"\n");
            return 2;//! send search via_udp
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
        /*if(have_a_chord){
            return 1;
            //! way to check if chord is empty without extra variable if possible
        }
        else*/ if(succ_distance<=check_distance(command.searched_key,chord->key)){
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

void process_EFND_EPRED(bool in_a_ring,node *node, command_details_t *command_details,char* buff){
//todo all this func is where i stopped for now 
    char* str_to_free ,*str_to_split;
    str_to_free=str_to_split=strdup(buff);
    split_str_nd_copy_to_new_location(&str_to_split,command_details->command);
    if (in_a_ring){
        if (strcmp("EFND",command_details->command)==0){
           split_str_nd_copy_to_new_location(&str_to_split,command_details->key);
           //! check if mine or pred and then check best way to continue search by distance between succ/chord to searched key
           //! start a search for the this key  
        }    
    }else if(strcmp("EPRED",command_details->command)==0){
        split_str_nd_copy_to_new_location(&str_to_split,node->key);
        split_str_nd_copy_to_new_location(&str_to_split,node->IP);
        split_str_nd_copy_to_new_location(&str_to_split,node->PORT);
    }
    free(str_to_free);
}

//initializes the mask with the req fd's
void mask_init(fd_set *mask_copy,int*maxfd,int udp_fd,int listen_fd,int tcp_s_fd,int chord_fd,int accepted_socket,int tcp_c_fd){
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
    if(tcp_s_fd){
        FD_SET(tcp_s_fd, mask_copy);
        *maxfd=max(*maxfd,tcp_s_fd);
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
	tcp_servaddr.sin_port = htons(PORT);//! mandar por argumento

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

void  init_udp_server(int *udp_fd, int PORT){
	SA_in tcp_servaddr;
// Creating socket file descriptor
    if ( (*udp_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }
       
    // Filling server information
    tcp_servaddr.sin_family    = AF_INET; // IPv4
    tcp_servaddr.sin_addr.s_addr = INADDR_ANY;
    tcp_servaddr.sin_port = htons(PORT);//! mandar por argumento
       
    // Bind the socket with the server address
    if ( bind(*udp_fd, (const struct sockaddr *)&tcp_servaddr, sizeof(tcp_servaddr)) < 0 ){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
}

//TODO: apenas faz um connect durante toda a execução(e quand há um leave ou pred)
void tcp_client(int *tcp_c_fd,SA_in*tcp_servaddr, char * port_, char*addr, char* message){
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
	if (connect(*tcp_c_fd, (SA*)tcp_servaddr, sizeof(*tcp_servaddr)) != 0) {
		printf("Connection with the server failed.\n");
		exit(0);
	}
	else
		printf("Connected to the server.\n");
    write(*tcp_c_fd,message, sizeof(char)*100);

}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        printf("to run client supply ring key, IP and port, separated by a single space \n");
        exit(0);
    } 
    // VAR INIT
    fd_set mask, mask_copy;
    int udp_fd=0, listen_fd=0,tcp_s_fd=0,maxfd=0,chord_fd=0,port,temp_tcp_c_fd=0,tcp_c_fd=0, n,accepted_socket=0;
    time_t entered_ring= time(NULL);
    node me, pred,succ, temp_node,  chord;
    command_details_t command_details;
    bool in_a_ring = false;
    char fcommand[100];
    SA_in tcp_client_addr, udp_client_addr,tcp_servaddr;
    //todo mudar variaveis para definitivas no tpc e udp
    socklen_t len = sizeof(SA_in);
    char buff[100];    
    // VAR INIT
    
    start_routine(&me,&pred,&succ,&temp_node,&chord,&command_details,argv,fcommand,buff,&mask,&mask_copy);
    port = strtol(me.PORT, NULL, 10);
    init_tcp_server(&listen_fd, port);
    init_udp_server(&udp_fd, port);
    
    for(;;){
        memset(buff,0,sizeof(char)*100);
        mask_init(&mask_copy,&maxfd,udp_fd,listen_fd,tcp_s_fd,chord_fd,accepted_socket,tcp_c_fd);
        mask = mask_copy;
        select(maxfd+1, &mask, (fd_set*)NULL, (fd_set*)NULL, (struct timeval *)NULL);
        if(FD_ISSET(0,&mask)){
            FD_CLR(0, &mask_copy);
            get_info_from_client( fcommand);
            if (*fcommand == 'n'){/* creates a new, empty ring where the node will ocupy the given key position per default */
                if(!in_a_ring){
                    in_a_ring =true; 
                    entered_ring = time(NULL);
                    new(&me, &succ,&pred,&temp_node);
                }
                else printf("Already in a ring u dumbass\n");
            }
                
            else if (*fcommand =='b' ){/*enters the ring knowing only one random node*/
                split_command(fcommand,&command_details);
            }
                
            else if (*fcommand == 'p' ){/*enters the ring know it's pred_key*/
                if (in_a_ring== false){
                    split_command(fcommand,&command_details);   
                    strcpy(pred.IP,command_details.IP);
                    strcpy(pred.key,command_details.key);
                    strcpy(pred.PORT,command_details.PORT);
                    sprintf(buff,"%s %s %s %s\n","SELF", me.key,me.IP,me.PORT);
                    tcp_client(&tcp_c_fd,&tcp_servaddr,pred.PORT,pred.IP,buff);
                }
            }
                
            else if (*fcommand =='c' ){/*creates a udp shortcut to a given key, key_ip, key_port*/                
                split_command(fcommand,&command_details);/*need to add a checker for valid command so it can exit nicely*/
                memset(&chord, 0,sizeof(node));
                //todo bind a udp client to the socket

            }
                
            else if(*fcommand == 'd' /*'e'*/){//echord 
              //!test code for client 
                if(chord.IP!=0){
                    close(chord_fd);
                    memset(&chord, 0,sizeof(node));
                }
            }/*problema porque e é short form para exit && echord prof tem que mudar*/
                
            else if(*fcommand =='s' ){// show current status
                    time_t uptime =time(NULL)- entered_ring;
                    show_status(in_a_ring, me,succ,pred,&chord,uptime);
            }
                
            else if(*fcommand =='f' ){
                if(find_key(fcommand, me.key, succ.key,&command_details)){// verifies if it is mine or not
                    //send_message()
                }
            }
                
            else if(*fcommand =='l'){
               if(in_a_ring){
                    if(strcmp(me.key, succ.key)!=0){
                        sprintf(buff,"%s %s %s %s\n","PRED", pred.key,pred.IP,pred.PORT);
                        write(tcp_s_fd,buff,sizeof(char)*100);
                    }
                    memset(&pred, 0,sizeof(node));
                    memset(&succ, 0,sizeof(node));
                    //todo put close all sockets inside a func
                    if (chord_fd){
                        close(chord_fd);
                        chord_fd=0;
                        FD_CLR(chord_fd, &mask_copy);
                    } 
                    if (tcp_c_fd){
                        close(tcp_c_fd);
                        tcp_c_fd=0;
                        FD_CLR(tcp_c_fd, &mask_copy);
                    } 
                    if (accepted_socket){
                        close(accepted_socket);
                        accepted_socket=0;
                        FD_CLR(accepted_socket, &mask_copy);
                    } 
                    if(tcp_s_fd){
                        close(tcp_s_fd);
                        tcp_s_fd =0;
                        FD_CLR(tcp_s_fd, &mask_copy);
                    }
                    in_a_ring= false;
               }
            }

            else if(*fcommand =='e' ){// exit
                if (chord_fd){
                    close(chord_fd);
                    chord_fd=0;
                } 
                if (tcp_c_fd){
                    close(tcp_c_fd);
                    tcp_c_fd=0;
                } 
                if (udp_fd){
                    close(udp_fd);
                    udp_fd=0;
                } 
                if (accepted_socket){
                    close(accepted_socket);
                    accepted_socket=0;
                } 
                close(listen_fd);
                close(udp_fd);
                exit(0);
                //exit => close all sockets and return 0
            }/*problema porque e é short form para exit && echord prof tem que mudar*/

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
        /*
            FD_CLR(tcp_s_fd, &mask_copy);
		if((n = read(tcp_s_fd, buff, 1000)) != 0){
			if(n == -1){
				printf("Reading error.\n");
				exit(1);
			}
			buff[strcspn(buff, "\n")] = 0;
			printf("%s\n", buff);
			if(*buff=='P'){//* might be on the wrong place
				split_self_pred_m(buff,&pred,"PRED",&command_details);
				sprintf(buff,"%s %s %s %s\n","SELF", me.key,me.IP,me.PORT);
				close(tcp_c_fd);
				close(tcp_s_fd); 
				tcp_client(&tcp_c_fd,&tcp_servaddr,pred.PORT, pred.IP,buff);
				
			}
			else if((*buff=='F')||(*buff=='R')){
				process_FND_RSP(buff,me,succ,pred,&chord);
			} 
		}
        */ 
        else if(udp_fd && FD_ISSET(udp_fd,&mask)){
            FD_CLR(udp_fd, &mask_copy);
            n = recvfrom(udp_fd, (char *)buff, 1000, 
                MSG_WAITALL, ( struct sockaddr *) &udp_client_addr,
                &len);
            sendto(udp_fd, "ACK\0", strlen("ACK\0"), 
                MSG_CONFIRM, (const struct sockaddr *) &udp_client_addr,
                    len);
            buff[strcspn(buff, "\n")] = 0;
            if((*buff=='F')||(*buff=='R')){
                process_FND_RSP(buff,me,succ,pred,&chord);
            }
            else if (*buff=='E'){
                process_EFND_EPRED(in_a_ring,&pred,&command_details,buff);
            }
        }
        else if(accepted_socket && FD_ISSET(accepted_socket, &mask)){
            FD_CLR(accepted_socket, &mask_copy);
            if((n = read(accepted_socket, buff, sizeof(char)*100)) != 0){
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
                        if(strcmp(me.key, succ.key)==0){
                            sprintf(buff,"%s %s %s %s\n","SELF", me.key,me.IP,me.PORT);
                            copy_node_info(&temp_node,&pred);
                            tcp_client(&tcp_c_fd,&tcp_servaddr,temp_node.PORT,temp_node.IP,buff);
                            
                        }
                        else {
                            sprintf(buff,"%s %s %s %s\n","PRED", temp_node.key,temp_node.IP,temp_node.PORT);
                            write(tcp_s_fd,buff,sizeof(char)*100);
                            close(tcp_s_fd);                            
                        }
                        
                    }
                    if(!in_a_ring){
                        in_a_ring=1;
                        entered_ring = time(NULL);
                    } 
                    copy_node_info(&temp_node,&succ);
                    tcp_s_fd = accepted_socket;
                    accepted_socket=0;   
                }
            }

        }
        else if(tcp_c_fd && FD_ISSET(tcp_c_fd, &mask)){
            FD_CLR(tcp_c_fd, &mask_copy);
            if((n = read(tcp_c_fd, buff, sizeof(char)*100)) != 0){
                if(n == -1){
                    printf("Reading error.\n");
                    exit(1);
                }
                buff[strcspn(buff, "\n")] = 0;
                printf("%s\n", buff);
                if(*buff=='P'){//* might be on the wrong place
                    split_self_pred_m(buff,&pred,"PRED",&command_details);
                    sprintf(buff,"%s %s %s %s\n","SELF", me.key,me.IP,me.PORT);
                    close(tcp_c_fd);
                    tcp_client(&temp_tcp_c_fd,&tcp_servaddr,pred.PORT, pred.IP,buff);
                    tcp_c_fd=temp_tcp_c_fd;
                    temp_tcp_c_fd=0;
                    
                }
                else if((*buff=='F')||(*buff=='R')){
                    process_FND_RSP(buff,me,succ,pred,&chord);
                } 
            }
        }
    }
    return 0;    
}

//! pentry is buggy not sure why 
