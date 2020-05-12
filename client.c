/***********
 * Authors : Mokshagna Doddala, Manikrishna sai Ravi
 * Team Name : Team M&M
 * Compile: gcc client.c -o server
 * Execute Example : ./client 127.0.0.1 8080
 *******/

#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <signal.h>
#include <wait.h>
#include <arpa/inet.h>
#include<time.h>


int main(int argc, char *argv[]){
    char message[255];
    int server_fd, port_number, pid, n;
    socklen_t len;
    struct sockaddr_in serv_add;
    if(argc != 3){
        printf("Call model: %s <IP> <Port #>\n", argv[0]);
        exit(0);
    }

    if((server_fd=socket(AF_INET, SOCK_STREAM, 0))<0){
        fprintf(stderr, "Cannot create socket\n");
        exit(1);
    }

    serv_add.sin_family = AF_INET;
    sscanf(argv[2], "%d", &port_number);
    serv_add.sin_port = htons((uint16_t)port_number);

    if(inet_pton(AF_INET, argv[1],&serv_add.sin_addr)<0){
        fprintf(stderr, " inet_pton() has failed\n");
        exit(2);
    }

    if(connect(server_fd, (struct sockaddr *) &serv_add,sizeof(serv_add))<0){
        fprintf(stderr, "connect() has failed, exiting\n");
        exit(3);
    }

    printf("Connected to server... Waiting for other player...\n");
    char name[255];
    read(server_fd, name, 255);
    fprintf(stderr, "Referee: %s\n", name);

    srand(time(NULL));
    long int ss =0;
    
    while(1)
    {
        
        read(server_fd, message, 255); 
        if(!strcmp(message,"You can now play"))
        {
            printf("\nReferee: %s\n",message);
            printf("%s: playing my dice and",name);
            int dice = rand()%10+1;
           // int dice =(int) time(&ss)%10 + 1;
            printf(" got %d points\n", dice);
            int c = htonl(dice);
            write(server_fd,&c,sizeof(dice));
        }

        else  if(!strcmp(message,"You won the game"))
        {
            printf("\nReferee: %s\n",message);
            printf("\n%s: I won the game\n",name);
            close(server_fd);
            exit(0);
        }
        else if (!strcmp(message,"You lost the game"))
        {
            printf("\nReferee: %s\n",message);
            printf("%s: I lost the game\n",name);
            close(server_fd);
            exit(0);
        }
   
    }
}