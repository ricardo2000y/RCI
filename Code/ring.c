
#include "header.h"
// types of comments
// !
// ?
// TODO:
// // 
// *
//! ver aqui info relevante
//* para todos os comandos fazer uma função que recebe uma string
//* qqr e a manda corretamente no canal tcp/udp necessário
//* chamada da func será algo tipo
//* communication(mode, message, (node)destinatario, socket) em que mode indica tcp ou UDP
//* 
//* dentro há de ter um static counter//
//* que vai de 0 A 99
//! ^^^^
//TODO1: pesquisa de chave mensagens usadas 
//*FND k n i i.IP i.port e RSP k n i i.IP i.port
//* onde k é o procurado// destinatario
//* n número de sequencia 
//* i i.ip i.port // info de quem iniciou a procura/resposta
//* se tcp meter \n no final da string, se UDP responder com ACK

//TODO2: saída e entrada de um  nó
//* PRED pred pred.IP pred.port\n ///de me para succ
//* SELF i i.ip i.port\n ///de me para pred

//TODO3:descoberta de posição no anel
//*EFND (udp) Realizado aquando da bentry
//* Epred pred pred.ip pred.port
//* ACK



// creates the socket to with the corect type (true = SOCK_DGRAM= UDP false = SOCK_STREAM=TCP) and checks if the creation was sucessful 
void criar_socket(int *sock_fd, bool mode){
    if(mode)// *true = SOCK_DGRAM= UDP false = SOCK_STREAM=TCP
        *sock_fd = socket(AF_INET, SOCK_DGRAM, 0);   // *creates the socket
    else  *sock_fd = socket(AF_INET, SOCK_STREAM, 0); // *creates the socket
    if (*sock_fd == -1){                         // *checks if the creation was sucessful
	    perror("socket{} ");   
	    exit(-1);
    }

}
// gets line from stdin given by user
void get_info_from_client( char* fcommand){
    size_t len =100;
    getline(&fcommand,&len,stdin); // * gets a full line until '\n' is met
    fcommand[strlen(fcommand)-1] ='\0';// * replaces the '\n' by a '\0' 
}

// checks the distance between the node and another node, returns the distance value
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
    void * ipvalidation;

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
// TODO: add more stuff need to check what is missing for new
// * most likely start a tcp/udp server listening to possible connections 
void new(node *me, node *pred, node *succ){
    copy_node_info(me,pred);// * initialize predecessor = me
    copy_node_info(me,succ);// * initialize successor = me
    // ?start tcp client and server(connect one to the other is unecessary(?)(have a variable knowing node is only one node)) + udp server
}

// split the given string and stores it on a new string
split_str_nd_copy_to_new_location(char* str_to_split,char *str_to_save){
    char* splitted;
    splitted = strsep(&str_to_split, " "); 
    if (splitted == NULL){// do routine for bad command 
        printf("%s given is incomplete program will terminate" ,"command");
        exit(0);
    }
    str_to_save=strdup (splitted);
}

//!fucking up on the ip_nd_port_check 
// splits the command given by user saving it in a structure while checking if the command is valid
void split_command(char * fcommand, command_details_t* command_details){
    char* splitted=NULL;
    char* str=NULL;
    bool valid;
    //? malloc of the strings inside command is needed or not? 
    str=(char*) malloc(sizeof(char)*100 );
    str = strdup(fcommand);
    //! still untested from here to:
    split_str_nd_copy_to_new_location(str,command_details->command);
    split_str_nd_copy_to_new_location(str,command_details->key);
    split_str_nd_copy_to_new_location(str,command_details->IP);
    split_str_nd_copy_to_new_location(str,command_details->PORT);
    //! till here
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



int main(int argc, char *argv[])
{
    if (argc != 4) {
        printf("to run client supply ring key, IP and port, separated by a single space \n");
        exit(0);
    } 
    node me, pred,succ;
    node * chord = NULL;
    command_details_t command_details;
    bool in_a_ring =false;
    memset(&me,0,sizeof(node));
    memset(&pred, 0,sizeof(node));
    memset(&succ, 0,sizeof(node));
    memset(&command_details, 0,sizeof(command_details_t));   
    me.key= strdup(argv[1]);
    me.IP= strdup(argv[2]);
    me.PORT= strdup(argv[3]);
    if (valid_IP_nd_port(me.IP, me.PORT)){
        printf("%s\t%s\t%s\n",me.key , me.IP,me.PORT);   
    }
    else printf("IP or Port provided is not valid");
    char* fcommand=(char*) calloc(1,sizeof(char) *100);
   
    do{//TODO: when implemmenting the select this part will be on the mask part for stdin
        get_info_from_client( fcommand);
        if (*fcommand == 'n'){/* creates a new, empty ring where the node will ocupy the given key position per default */
            if(!in_a_ring){
                in_a_ring =true; 
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
            /*use strtok to split str into key/ip/port*/ 
            split_command(fcommand,&command_details);/*need to add a checker for valid command so it can exit nicely*/
            //chord = (node*)malloc(sizeof(node));
            // memset(&chord, 0,sizeof(node));
            // 

        }
            
        else if(*fcommand == 'd' /*'e'*/){
            //close(chord_fd);
            // free(chord);
            // chord = NULL;
        }/*problema porque e é short form para exit && echord prof tem que mudar*/
            
        else if(*fcommand =='s' ){// show current status
                printf("%-15s\t%-15s\t%-15s\n\n"," "," ","STATUS");
                printf("%-15s\t%-15s\t%-15s\t%-15s\n","     ", "KEY","IP","PORT");
                printf("mine:       \t%-15s\t%-15s\t%-15s\n",me.key , me.IP,me.PORT); 
                printf("sucessor:   \t%-15s\t%-15s\t%-15s\n",succ.key , succ.IP,succ.PORT);
                printf("predecessor:\t%-15s\t%-15s\t%-15s\n",pred.key , pred.IP,pred.PORT);
                if(chord !=NULL) printf("atalho:\t%-15s\t%-15s\t%-15s\n",chord->key , chord->IP,chord->PORT);
                // adicionar info extra p.ex se está sozinho se há 2 pessoas no ring ou se há pelo menos 3(basta comparar os IP's dos node's me, succ, pred)
        }
            
        else if(*fcommand =='f' ){
            if(find_key(fcommand, me.key, succ.key)){// verifies if it is mine or not

            }
        }
            
        else if(*fcommand =='l'){
            memset(&pred, 0,sizeof(node));
            memset(&succ, 0,sizeof(node));
            free(chord); 
            in_a_ring= false;
        }

        /*else if(command =='e' ){
            exit => close all sockets and return 0
        }problema porque e é short form para exit && echord prof tem que mudar*/

        /*else{
            //insert some stuff saying invalid command
        }*/
    } while(*fcommand !='e');
    return 0;    
}
