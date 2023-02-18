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

#define MAX_SIZE 1024   //dim max del buffer
#define ID_SIZE 21         //dim del codice di tessera sanitaria (10 byte +1 del terminatore)
#define ACK_SIZE 61        //dim dell'ack ricevuto dal serverVerifica
#define ASL_ACK 39 //size per gli ACK inviati all'ASL

typedef struct{
    char ID[ID_SIZE];
    char report;
}Report_Asl;

int main(int argc, char *argv[]){
    char bit_communication;
    char buffer[MAX_SIZE];
    int socket_fd;
    struct sockaddr_in serv_addr;
    Report_Asl package;

    bit_communication = '1'; //inizializzazione del bit da inviare a server_verifica
    
    //struttura socket-client
    socket_fd = Socket(AF_INET,SOCK_STREAM,0);
    //struttura serv_addr
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(1026); //1026: porta per il collegamento con server_verifica
    
    //da stringa ad address network dell'indirizzo localhost [127.0.0.1]
    if(inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr)<=0){
        perror("inet_pton() error\n");
        exit(1);
    }

    //connesione al server
    Connect(socket_fd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

    //invia il bit al server_verifica per comunicazione con asl a quest'ultimo
    if(Full_write(socket_fd, &bit_communication, sizeof(char))<0){
        perror("full_write() error");
        exit(1);
    }

    printf("***ASL***\n");
    printf("Inserire identificativo tessera sanitaria ed il referto del tampone per sospensione o ripristinare del green_pass\n\n");
    //inserimento identificativo tessera sanitaria
    while(1){
        printf("Inserisci identificativo tessera sanitaria [20 caratteri]\n");
        if(fgets(package.ID, MAX_SIZE, stdin) == NULL){
            perror("fgets() error");
            exit(1);
        }
        //controllo input ricevuto
        if(strlen(package.ID)!=ID_SIZE){printf("Identificativo tessera sanitaria inserina NON corretto, riprovare\n\n");
        }else{
            //eliminiamo il carattere di invio/terminazione inserito precedentemente nel fgets
            package.ID[ID_SIZE-1] = 0;
            break;
        }
    }
    //dopo aver controllaro l'identificativo della tessera sanitaria, controlliamo l'esito del tampone
    while(1){
        printf("Inserire 0 [Green Pass NON valido]\nInserire 1 [Green Pass valido]\nINPUT: ");
        scanf("%c",&package.report);
        //controllo del valore inserito [deve essere 0 o 1]
        if(package.report == '1' || package.report == '0'){break;}
        printf("Error: inserire input corretto\n");
    }

    //controllo del valore
    if(package.report == '1'){
        printf("Invio richiesta di ripristino del Green Pass\n");
    }else if(package.report == '0'){
        printf("Invio richiesta di sospensione del Green Pass\n");
    }

    //invio del pacchetto report al server_verifica
    if(Full_write(socket_fd,&package,sizeof(Report_Asl))<0){
        perror("full_write() error");
        exit(1);
    }
    //lettura messaggio di report dal server_verifica
    Full_read(socket_fd,buffer,ASL_ACK);
    sleep(2); //simulazione tempo di attesa
    printf("%s\n",buffer); //stampa messaggio del report
    exit(0);

}