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
void get_info_from_client( char* fcommand){
    //int n=0;
    size_t len =100;
    //memset(fcommand,0,sizeof(fcommand));
    getline(&fcommand,&len,stdin);
    //while ((fcommand[n++] = getchar()) != '\n')
    fcommand[strlen(fcommand)-1] ='\0';
    //printf("\t%s\n", fcommand);
    
}

int check_distance(char* my_key, char* dest_key){
    int my_key_n =strtol(my_key, NULL, 10);
    int dest_key_n= strtol(dest_key, NULL, 10);
    int result = (dest_key_n-my_key_n)%MAX_KEY;
    if (result <0) result += MAX_KEY; 
    //printf("\n%d\t%d\t%d\n",my_key_n, dest_key_n,result);
    return result;

}

/*Checks if IP and PORT are valid*/
bool valid_IP_nd_port(char* IP, char* PORT){
    bool port_validator, ip_validator;
    int  check;
    void * ipvalidation;
    ip_validator = (bool)inet_pton( AF_INET, IP, ipvalidation );
    check = strtol(PORT, NULL, 10);
    if(check <65536 && check > 0) port_validator = true;
    else port_validator = false;
    return (ip_validator && port_validator);
}

void copy_node_info(node *origin, node *copy){
    copy->IP= origin->IP;
    copy->key= origin->key;
    copy->PORT = origin->PORT;
}

void new(node *me, node *pred, node *succ){
    copy_node_info(me,pred);
    copy_node_info(me,succ);
    //printf("%s\n %s\n %s\n",  pred->key,pred->IP,pred->PORT);
    /*start tcp client and server(connect one to the other is unecessary(?)(have a variable knowing node is only one node)) + udp server*/
}
/*fucking up on the ip_check */
void split_command(char * fcommand, command_details_t* command_details){
    char* splitted=NULL;
    char* str=NULL;
    bool valid;
    //str=(char*) malloc(sizeof(char) *100);
    str = strdup(fcommand);
    splitted = strsep(&str, " ");
    splitted = strsep(&str, " ");
    if (splitted == NULL){// do routine for bad command 
        printf("%s given is incomplete program will terminate", fcommand);
        exit(0);
    }
    command_details->key=strdup (splitted);
    splitted = strsep(&str, " ");
     if (splitted == NULL){// do routine for bad command 
        printf("%s given is incomplete program will terminate", fcommand);
        exit(0);
    }
    command_details->IP=strdup (splitted);
    splitted = strsep(&str, " ");
    if (splitted == NULL){// do routine for bad command 
        printf("%s given is incomplete program will terminate", fcommand);
        exit(0);
    }
    command_details->PORT=strdup (splitted);
    if(str != NULL){// do routine for bad command 
        printf("%s given is incomplete program will terminate", fcommand);
        exit(0);
    }
    //valid = valid_IP_nd_port(command_details->IP,command_details->PORT);
    //if(!valid){
       //  printf("%s given has worng port or ip program will terminate", fcommand);
       // exit(0);
        // do routine for bad command 
     // }
    //free(str);
}

bool find_key(char * fcommand,char* me, char* succ){
    char* splitted;
    char* str =(char*) malloc(sizeof(char) *100);
    str = strdup(fcommand);
    splitted = strsep(&str, " ");
    splitted = strsep(&str, " ");
    if (splitted == NULL){// do routine for bad command 
        printf("%s given is incomplete program will terminate", fcommand);
        exit(0);
    }
    free(str);
    int dist_me_k = check_distance(me, splitted);
    int dist_succ_k= check_distance(succ,splitted);
     if (dist_me_k <= dist_succ_k){
        // printf("mine\n distance: %d",dist_me_k);
         return true;
        /* k object is mine */
    }
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
   
    do{
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
