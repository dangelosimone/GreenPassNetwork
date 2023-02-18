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
#include <signal.h>
#include "../wrapper.h"
#include "function_centVacc.h"

#define MAX_SIZE 1024
#define MESS_CVACC 64
#define ID_SIZE 21

int main(int argc, char const *argv[]){
    int listen_fd; //socket per il l'attesa del collegamento
    int connect_fd; //socket dell'effettivo collegamento
    struct sockaddr_in serv_addr;
    pid_t pid;
    //cattura segnale di CNTR+C
    signal(SIGINT,handler);

    //definisco fd della socket di collegamento
    listen_fd = Socket(AF_INET,SOCK_STREAM,0);
    //struttura
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //poiché lavoriamo in locale
    serv_addr.sin_port = htons(1024); //collegamento con l'utente

    //leghiamo tale porta al server
    Bind(listen_fd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    //mettimo il server in ascolto fino a 1024 connessioni
    Listen(listen_fd,1024);

    for(;;){
        printf("In attesa di nuove richieste di vaccinazione ...");
        connect_fd = Accept(listen_fd,(struct sockaddr*)NULL,NULL);

        //creo figli che permettono la gestione delle varie richieste degli utenti
        if((pid=fork())<0){
            perror("fork() error\n");
            exit(1);
        }
        if(pid==0){
            //chiudi il server in ascolto in modo da gestire un utente alla volta
            close(listen_fd);
            //riceve informazioni dell'utente [Definisce GreenPass_Date]
            //invia al serverVaccinazione le informazioni ricevute
            answer_user(connect_fd);
            //chiude connessione con l'utente dopo aver ottenuto le sue informazioni
            close(connect_fd);
            exit(0);
        }else{close(connect_fd);}
    }
    exit(0);
}


