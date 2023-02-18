#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>      // libreria standard del C che contiene definizioni di macro per la gestione delle situazioni di errore.
#include <string.h>
#include <netdb.h>      // contiene le definizioni per le operazioni del database di rete.
#include <sys/types.h>
#include <sys/socket.h> //contiene le definizioni dei socket.
#include <arpa/inet.h>  // contiene le definizioni per le operazioni Internet.

#include "../wrapper.h"

#define MAX_SIZE 1024   //dim massima del buffer
#define ACK_SIZE 64     //dim dell'ack ricevuto dal ServerVerifica
#define WELCOME_SIZE 108 //dim del messaggio di benvenuto dal ServerVerifica
#define APP_ACK 39       //dim dell'ack ricevuto dall'AppVerifica
#define ID_SIZE 21 //20 byte per identificativo la tessera sanitaria più un byte per il terminatore

int main(){
    int socket_fd;
    struct sockaddr_in serv_addr;
    char bit_communication;
    char buffer[MAX_SIZE],ID[ID_SIZE];

    bit_communication = '0'; //per inviarlo al server_verifica
    socket_fd = Socket(AF_INET,SOCK_STREAM,0);
    //struttura
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(1026); //connessione al server_verifica

    //conversione da stringa a address network
    if(inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr)<=0){
        perror("inet_pton() error");
        exit(1);
    }

    //connessione al server
    Connect(socket_fd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    //inviamo bit 0 a server_verifica per indicare che quest'ultimo che si sta connettendo con app_verifica
    if (Full_write(socket_fd,&bit_communication,sizeof(char)) < 0) {
        perror("full_write() error");
        exit(1);
    }
    //riceviamo il benevenuto dal server_verifica
    if (Full_read(socket_fd,buffer,WELCOME_SIZE) < 0) {
        perror("full_read() error");
        exit(1);
    }
    //stampiamo il messaggio di benvenuto ricevuto
    printf("%s\n\n",buffer);

    //inserimento tessera sanitaria
    while(1){
        printf("Inserire identificativo tessera sanitaria [20 caratteri, codice presente dietro la tessera]\n");
        fgets(ID,MAX_SIZE,stdin);
        //controllo Identificativo ricevuto
        if(strlen(ID)!=ID_SIZE){printf("Identificativo inserito non corrisponde al numero corretto di caratteri, riprovare\n");
        }else{
            //se inserito correttamente l'ID allora eliminiamo l'ultimo carattere presente rappresentato dall'invio
            ID[ID_SIZE - 1] = 0;
            break; //usciamo dal ciclo infinito se abbiamo ottenuto un indentificativo di tessera sanitaria valido
        }
    }

    //invio dell'identificativo di tessera sanitaria
    if(Full_write(socket_fd,ID,ID_SIZE)< 0){
        perror("full_write() error");
        exit(1);
    }
    //recezione della corretta recezione dell'identificativo della ts
    if(Full_read(socket_fd,buffer,ACK_SIZE) < 0){
        perror("full_read() error");
        exit(1);
    }
    //stampiamo il messaggio di benvenuto ricevuto
    printf("%s\n\n",buffer);

    printf("Convalida in corso,attendere.....\n\n");
    sleep(3); //3 secondi di attesa

    //recezione dell'esito del green_pass da server_verifica
    if(Full_read(socket_fd,buffer,APP_ACK)<0){
        perror("full_read() error\n");
        exit(1);
    }
    //stampiamo l'esito
    printf("%s\n",buffer);
    close(socket_fd);
    exit(0);
}