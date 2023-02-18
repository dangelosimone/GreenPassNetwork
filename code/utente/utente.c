#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>      // libreria standard del C che contiene definizioni di macro per la gestione delle situazioni di errore.
#include <string.h>
#include <netdb.h>      // contiene le definizioni per le operazioni del database di rete.
#include <sys/types.h>
#include <sys/socket.h> // contiene le definizioni dei socket.
#include <arpa/inet.h>  // contiene le definizioni per le operazioni Internet.
#include "../wrapper.h"

#define MAX_SIZE 1024
#define MESS_CVACC 64
#define ID_SIZE 21


// Definizione pacchetto da inviare al centro vaccinale
typedef struct{
    char name[MAX_SIZE];
    char surname[MAX_SIZE];
    char ID[ID_SIZE];
} vax_request;

vax_request create_package(){
    char buffer[MAX_SIZE];
    vax_request create_pack;

    //Inserimento "name"
    printf("Inserisci Nome: ");
    if(fgets(create_pack.name, MAX_SIZE, stdin) == NULL){
        perror("fgets() Aerror\n");
    }
    create_pack.name[strlen(create_pack.name) - 1] = 0;

    //Inserimento "surname"
    printf("Inserisci Cognome: ");
    if(fgets(create_pack.surname, MAX_SIZE, stdin) == NULL){
        perror("fgets() error\n");
    }
    create_pack.surname[strlen(create_pack.surname) - 1] = 0;
    
    while(1){
        //Inserimento "ID"
        printf("Inserisci numero di identificazione della tessera sanitaria: ");
        if(fgets(create_pack.ID,MAX_SIZE,stdin)==NULL){
            perror("fgets() error\n");
            exit(1);
        }
        if(strlen(create_pack.ID)!=(ID_SIZE)){
            printf("Numero di identificazione tessera sanitaria non corretto, devono essere esattamente 20! Riprovare\n\n");
        }else{
            create_pack.ID[ID_SIZE - 1] = 0;
            break;
        }
    }
    return create_pack;
}

int main(int argc, char const *argv[]){
    int socket_fd, client_fd;
    int message_welcome;
    struct sockaddr_in server_addr;
    vax_request package;
    char buffer [MAX_SIZE];
    char *addr;

    if((socket_fd = Socket(AF_INET,SOCK_STREAM,0)) < 0){
        perror("socket() error");
        exit(1);
    }

    //Valorizzazione Struttura
    //htons da host a network (conversione)
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1024);

    if(inet_pton(AF_INET,"127.0.0.1", &server_addr.sin_addr)<= 0){
        perror("inet_pton() error");
        exit(1);
    }
    client_fd = Connect(socket_fd,(struct sockaddr*)&server_addr,sizeof(server_addr));
    
    //Legge quanti byte invia al centro vaccinale
    if(Full_read(socket_fd, &message_welcome, sizeof(int))<0){
        perror("full_read() error\n");
        exit(1);
    }

    //Riceve il benvenuto dal centro vaccinale
    if(Full_read(socket_fd,buffer,message_welcome)<0){
        perror("full_read() error\n");
        exit(1);
    }
    printf("%s\n",buffer);

    package = create_package();

    //Invio del pacchetto
    if(Full_write(socket_fd,&package,sizeof(package))<0){
        perror("full_write() error\n");
        exit(1);
    }

    //Recezione del centro vaccinale
    /*
       if(Full_read(socket_fd, buffer, MESS_CVACC)<0){
        perror("full_read() error\n");
        exit(1);
    }
    printf("%s\n\n",buffer);
    
    */
 
    exit(0);
}
