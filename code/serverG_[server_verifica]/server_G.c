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
#include "server_G.h"
#include "../wrapper.h"
#define MAX_SIZE 1024  //dim massima del buffer
#define WELCOME_SIZE 108
#define ID_SIZE 21    //dim della tessera sanitaria
#define ACK_SIZE 64
#define ASL_ACK 39


void handler(int sign){
    if(sign == SIGINT){
        printf("\nExit...\n");
        sleep(2);
        printf("***Thank you for using our service***\n");
        exit(0);
    }
}

//funzione per estrazione data corrente che verrá utilizzata con GP
void create_data_corrente(DATE *start_date){
    time_t time_current;
    time_current = time(NULL);

    //utilizziamo struttura tm per convertire la data da stringa a intero
    struct tm *c_date = localtime(&time_current);
    //partendo i mesi di tm da 0
    c_date -> tm_mday += 1;
    //partendo gli anni da 122 per raggiungere 2022
    c_date -> tm_year += 1901;

    //carichiamo i valori della data corrente in start_date
    start_date -> day = c_date -> tm_mday;
    start_date -> month = c_date -> tm_mon;
    start_date -> year = c_date -> tm_year;
}

//funzione di comunicazione con app_verifica che invierá il numero di tessera sanitaria
//server_verifica richiede il report al server_vaccinale e dopo le procedure di verifica comunica il risultato a app_verifica
char verifica_ID(char ID[]){
    struct sockaddr_in serv_addr;
    int socket_fd;
    char bit_communication,report;
    char buffer[MAX_SIZE];
    GreenPass_Request gp;
    DATE data_corrente;

    bit_communication = '0'; //impostiamo a 0 per far capire al server_vaccinale che sta comunicando con server_verifica

    //struttura socketClient
    socket_fd = Socket(AF_INET,SOCK_STREAM,0);
    //struttura
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(1025);
    //conversione di indirizzo ip da stringa a dotted (address_network)
    if(inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr)<=0){
        perror("perror() error\n");
        exit(1);
    }
    //connessione con il server
    Connect(socket_fd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

    //inviamo bit 0 per dire al server_vaccinale di collegarsi con server_verifica
    if(Full_write(socket_fd,&bit_communication,sizeof(char))<0){
        perror("full_write() error\n");
        exit(1);
    }

    bit_communication = '1';

    //inviamo bit 1 per dire al server_vaccinale che deve controllare la validitá del green pass
    if(Full_write(socket_fd,&bit_communication,sizeof(char))<0){
        perror("full_write() error\n");
        exit(1);
    }

    //invio numero di tessera sanitaria ricevuto da app_verifica al server_vaccinale
    if(Full_write(socket_fd,ID,ID_SIZE)<0){
        perror("full_write() error\n");
        exit(1);
    }
    
    //lettura report di server_vaccinale
    if(Full_read(socket_fd,&report,sizeof(char))<0){
        perror("full_read() error\n");
        exit(1);
    }

    
    if(report == '1'){
        //estraiamo tutto il greenpass associato alla tessera sanitaria inviata da app_verifica
        if(Full_read(socket_fd,&gp,sizeof(GreenPass_Request))<0){
            perror("full_read() error\n");
            exit(1);
        }

        //chiudiamo la connessione dopo aver ottenuto il gp associato
        close(socket_fd);

        create_data_corrente(&data_corrente);
        //vediamo se il gp é realmente valido confrontandolo con la data corrente
        //controllo effettuato mediante la data di scandenza del gp
        if(data_corrente.year > gp.end_date.year) report = '0';
        if(report == '1' && data_corrente.year > gp.end_date.year && data_corrente.month > gp.end_date.month) report = '0';
        if(report == '1' && data_corrente.year > gp.end_date.year && data_corrente.month > gp.end_date.month && data_corrente.day > gp.end_date.day) report = '0';
        //controllo effettuato se presente gp di utente con tampone positivo [gp.report = 0]
        if(report == '1' && gp.report == '0') report = '0';
    }
    return report;
}

//funzione per la comunicazione con l'utente
void receive_ID(int connect_fd){
    char report, buffer[MAX_SIZE], ID[ID_SIZE];
    int index,welcome_size;

    //messaggio di benvenuto da inviare all'app_verifica dopo la connessione con server_verifica
    snprintf(buffer,WELCOME_SIZE,"**Benvenuto nel server di verifica**\nInserire identificativo tessera sanitaria per verificare il green pass");
    buffer[WELCOME_SIZE - 1] = 0;
    //mandiamo il messaggio sopra scritto
    if(Full_write(connect_fd,buffer,WELCOME_SIZE)<0){
        perror("full_write() error\n");
        exit(1);
    }

    //legge il codice fiscale inviato da app_verifica
    if(Full_read(connect_fd,ID,ID_SIZE)<0){
        perror("full_read() error");
        exit(1);
    }

    //notifica all'utente della corretta recezione dei dati
    snprintf(buffer,ACK_SIZE,"Il numero di tessera sanitaria è stato correttamente ricevuto");
    buffer[ACK_SIZE - 1] = 0;
    if(Full_write(connect_fd,buffer,ACK_SIZE)<0){
        perror("full_write() error\n");
        exit(1);
    }

    //funzione che invia il numero di tessera sanitaria e riceve il report dal server_verifica
    report = verifica_ID(ID);

    /*
    Verifichiamo in base al report del server_verifica validitá del green pass e l'esistenza di quest'ultimo
    */
    if (report == '1') {
        strcpy(buffer, "Green Pass valido, operazione conclusa");
        if(Full_write(connect_fd, buffer, ASL_ACK) < 0) {
            perror("full_write() error");
            exit(1);
        }
        printf("%s\n\n",buffer);
    } else if (report == '0') {
        strcpy(buffer, "Green Pass non valido, uscita in corso");
        if(Full_write(connect_fd, buffer, ASL_ACK) < 0) {
            perror("full_write() error");
            exit(1);
        }
        printf("%s\n\n",buffer);
    } else {
        strcpy(buffer, "Numero tessera sanitaria non esistente");
        if(Full_write(connect_fd, buffer, ASL_ACK) < 0) {
            perror("full_write() error");
            exit(1);
        }
        printf("%s\n\n",buffer);
    }
    close(connect_fd);
}

char send_report(report_client package){
    int socket_fd;
    struct sockaddr_in serv_addr;
    int bit_communication;
    char buffer[MAX_SIZE],report;
    bit_communication = '0';

    //struttura socket-client
    socket_fd = Socket(AF_INET,SOCK_STREAM,0);
    //struttura
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(1025);
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("inet_pton() error");
        exit(1);
    }

    Connect(socket_fd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
    
    //invia un bit di valore 0 al server_vaccinale per dirlo che sta comunicando con server_verifica
    if(Full_write(socket_fd,&bit_communication,sizeof(char))<0){
        perror("full_write() error");
        exit(1);
    }

    //invia un bit di valore 0 al server_vaccinale dopo la comunicazione con quest'ultimo per modificare il report del green pass
    if(Full_write(socket_fd,&bit_communication,sizeof(char))<0){
        perror("full_write() error");
        exit(1);
    }

    //inviamo il package dell'asl al server_vaccinale
    if(Full_write(socket_fd,&package,sizeof(report_client))<0){
        perror("full_write() error");
        exit(1);
    }

    //prende il report da server_verifica
    if(Full_read(socket_fd,&report,sizeof(report))<0){
        perror("full_write() error");
        exit(1);
    }
    close(socket_fd);
    return report;
}

void receive_report(int connect_fd){
    report_client package;
    char report, buffer[MAX_SIZE];

    //Legge i dati del pacchetto report_client inviato dall'ASL
    if(Full_read(connect_fd, &package, sizeof(report_client)) < 0){
        perror("full_read() error");
        exit(1);
    }

    report = send_report(package);
    if (report == '1') {
        strcpy(buffer, "Numero tessera sanitaria non esistente");
        if(Full_write(connect_fd, buffer, ASL_ACK) < 0) {
            perror("full_write() error");
            exit(1);
        }
        printf("%s\n",buffer);
    } else {
        strcpy(buffer, "***Operazione avvenuta con successo***");
        if(Full_write(connect_fd, buffer, ASL_ACK) < 0) {
            perror("full_write() error");
            exit(1);
        }
        printf("%s\n",buffer);
    }
}
