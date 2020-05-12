/***********
 * Authors : Mokshagna Doddala, Manikrishna sai Ravi
 * Team Name : Team M&M
 * Compile: gcc server.c -o server
 * Execute Example : ./server 8080
 *******/

#include <unistd.h> 
#include<time.h>
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <signal.h>
#include <sys/shm.h>
#include <wait.h>

void servicePlayers (int,int);
void displayMessage();

int main(int argc, char *argv[])
{
    int socket_fd, client1_fd=0, client2_fd=0, port_number, status;
    socklen_t len;
    struct sockaddr_in serv_add;

    if(argc != 2){
        printf("ERROR Calling!!!\n");
        printf("Call model: %s <Port #>\n", argv[0]);
        exit(0);
    }

    //creating socket
    if((socket_fd = socket(AF_INET, SOCK_STREAM, 0))<0){
        printf("ERROR: Creating socket\n");
        exit(1);
    }

    // building socket address
    serv_add.sin_family = AF_INET;
    serv_add.sin_addr.s_addr = htonl(INADDR_ANY);
    sscanf(argv[1], "%d", &port_number);
    serv_add.sin_port = htons((uint16_t)port_number);

    // assigning name specified by serv_add to socket created
    bind(socket_fd,(struct sockaddr*)&serv_add,sizeof(serv_add));
    //prepare to accept connections 
    listen(socket_fd, 5);

    printf("Socket created with file descriptor %d\n", socket_fd);
    printf("Assigning name specified by serv_add to socket referenced by %d\n",socket_fd);

    signal(SIGUSR1,displayMessage);
    while(1){
        displayMessage();
        client1_fd = accept(socket_fd,(struct sockaddr *)NULL,NULL);
            if(client1_fd<0)
                {
                    fprintf(stderr,"ERROR Conncenting to client exiting\n");
                    exit(0);
                }
        fprintf(stderr,"Client 1 joined\n");
        client2_fd = accept(socket_fd,(struct sockaddr *)NULL,NULL);
        fprintf(stderr,"Client 2 joined\n");
            if(client2_fd<0)
                {
                    fprintf(stderr,"ERROR Conncenting to client exiting\n");
                    exit(0);
                }         
        fprintf(stderr,"Got two players. starting the game\n\n");
        if(!fork()){
            fprintf(stderr,"Process with pid %d handling the game \n\n",getpid());
            servicePlayers(client1_fd,client2_fd);
        }
   
    }
}

void displayMessage()
{
    fprintf(stderr,"Waiting for clients\n");
}

void servicePlayers(int sd1, int sd2){
    char message[255];
    int n, pid, score1=0,score2=0;
    int score[2] = {0, 0};

    write(sd1, "TOTO", 5);
    write(sd2, "TITI", 5);

    while(1)
    {
        int sc = 0 ;

        //send message to client1 and wait for its response. 
        write(sd1, "You can now play", 17);
        read(sd1,&sc,sizeof(sc));
        score1 += ntohl(sc);
        sleep(1);

        //send message to client2 and wait for its response. 
        write(sd2, "You can now play", 17);
        read(sd2,&sc,sizeof(sc));
        score2 += ntohl(sc);

        if(score1>= 100 || score2>= 100)
        {
        
            printf("Game Handled by process  %d is Over\n", getpid());
            printf("Score obtained by TOTO : %d\n", score1);
            printf("Score obtained by TITI : %d\n", score2);

            if(score1 >= 100)
        {
             write(sd1,"You won the game",17);
             if(score2<100)
                write(sd2,"You lost the game",18);
        }
          if(score2 >= 100)
        {
             write(sd2,"You won the game",17);
             if(score1<100)
                write(sd1,"You lost the game",18);
        }
        close(sd1);
        close(sd2);
        kill(getppid(),SIGUSR1); // send signal to server indicating game over.
        kill(getpid(),SIGTERM);
        }        
    }    
}
