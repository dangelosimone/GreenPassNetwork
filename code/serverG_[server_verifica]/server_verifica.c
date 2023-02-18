#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>      // libreria standard del C che contiene definizioni di macro per la gestione delle situazioni di errore.
#include <string.h>
#include <netdb.h>      // contiene le definizioni per le operazioni del database di rete.
#include <sys/types.h>
#include <sys/socket.h> //contiene le definizioni dei socket.
#include <arpa/inet.h>  // contiene le definizioni per le operazioni Internet.
#include <time.h>
#include <signal.h>     //consente l'utilizzo delle funzioni per la gestione dei segnali fra processi.

#include "../wrapper.h"
#include "server_G.h"

#define MAX_SIZE 1024  //dim massima del buffer
#define WELCOME_SIZE 108
#define ID_SIZE 21    //dim della tessera sanitaria
#define ACK_SIZE 64
#define ASL_ACK 39

int main(){
    int listen_fd, connect_fd;
    struct sockaddr_in serv_addr;
    pid_t pid;
    char bit_communication;

    signal(SIGINT,handler); //intercetta segnale
    listen_fd = Socket(AF_INET,SOCK_STREAM,0);

    //struttura
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(1026);

    //assegnazione della porta al server
    Bind(listen_fd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

    Listen(listen_fd,1024);

    for(;;){
        printf("In attesa di Green Pass da scansionare\n");
        connect_fd = Accept(listen_fd,(struct sockaddr*)NULL,NULL);
        if((pid = fork())<0){
            perror("fork() error");
            exit(1);
        }

        if(pid==0){
            close(listen_fd);
            /*
                Il server_verifica riceve un bit come primo messaggio, che puo avere connessioni sia con app_verifica che con asl
                Quando riceve bit 1 il figlio gestirà la connessione con ASL
                Quando riceve bit 0 il figlio gestirà la connessione con app_verifica
            */
           if(Full_read(connect_fd,&bit_communication,sizeof(char))<0){
                perror("full_read() error");
                exit(1);
           }
           
           if(bit_communication == '1') receive_report(connect_fd); //connessione con ASL
           else if(bit_communication == '0') receive_ID(connect_fd); //connessione con app_verifica
           else printf("client non riconosciuto\n");

           //chiudiamo comunicazione con il client
           close(connect_fd);
           exit(0);
    } else close (connect_fd);
}
    exit(0);
}