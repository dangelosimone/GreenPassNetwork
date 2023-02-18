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
#include "function_centVacc.h"
#include "../wrapper.h"


#define MAX_SIZE 1024
#define MESS_CVACC 64
#define ID_SIZE 21

//funzione per stampa messaggio all'arrivo del segnale SIGINT
void handler(int sign){
    if(sign == SIGINT){
        printf("\nExit...\n");
        sleep(2);
        printf("***Thank you for using our service***\n");
        exit(0);
    }
}

//funzione per definire da scadenza GreenPass
void create_end_date(DATE *end_date){
    //per la gestione della data estrapolando il time del computer
    time_t time_tick;
    time_tick = time(NULL);

    //function di localtime che permette di convertire formato data a intero
    struct tm *t_date = localtime(&time_tick);

    t_date -> tm_mon += 10; //il conteggio dei mesi va da 0 ad 11
    t_date -> tm_year += 1901; //gli anni partono da 122 quindi per raggiungere 2023 sommo 1900

    //effettuo il controllo nel caso in cui il vaccino sia stato fatto nel mese di

    //aprile
    if (t_date -> tm_mon == 13){
        t_date -> tm_mon = 1;
        t_date -> tm_year++;
    }

    //maggio
    if (t_date -> tm_mon == 14){
        t_date -> tm_mon = 2;
        t_date -> tm_year++;
    }

    //giugno
    if (t_date -> tm_mon == 15){
        t_date -> tm_mon = 3;
        t_date -> tm_year++;
    }

    //luglio
    if (t_date -> tm_mon == 16){
        t_date -> tm_mon = 4;
        t_date -> tm_year++;
    }

    //agosto
    if (t_date -> tm_mon == 17){
        t_date -> tm_mon = 5;
        t_date -> tm_year++;
    }

    //settembre
    if (t_date -> tm_mon == 18){
        t_date -> tm_mon = 6;
        t_date -> tm_year++;
    }

    //ottobre
    if (t_date -> tm_mon == 19){
        t_date -> tm_mon = 7;
        t_date -> tm_year++;
    }

    //novembre
    if (t_date -> tm_mon == 20){
        t_date -> tm_mon = 8;
        t_date -> tm_year++;
    }

    //dicembre
    if (t_date -> tm_mon == 21){
        t_date -> tm_mon = 9;
        t_date -> tm_year++;
    }

    printf("La Data di Scadenza del Green Pass é : %02d/%02d/%02d\n",t_date->tm_mday,t_date->tm_mon,t_date->tm_year);

    //assegniamo i valori definiti da tal funzione con i vari if nella struct
    end_date -> day = t_date -> tm_mday;
    end_date -> month = t_date -> tm_mon;
    end_date -> year = t_date -> tm_year;
}

//calcola la data di inizio validitá del GreenPass (data emissione del certificato)
void create_start_date(DATE *start_date){
    time_t time_tick;
    time_tick = time(NULL);

    //stessa inizializzazione della precedente
    struct tm *a_date = localtime(&time_tick);
    a_date -> tm_mon += 1; //poché i mesi partono da (0 a 11)
    a_date -> tm_year += 1901; //per raggiungere 2022

    printf("La Data di Inizio Validitá del Green Pass é : %02d/%02d/%02d\n",a_date->tm_mday,a_date->tm_mon,a_date->tm_year);

    start_date->day = a_date->tm_mday;
    start_date->month = a_date -> tm_mon;
    start_date->year = a_date -> tm_year;
}

//funzione per inviare Green_Pass a ServerVaccinale
void send_GreenPass(GreenPass_Date gp){
    //stabilisco connessione con il serverVaccinale
    int socket_fd;
    int client_fd;
    struct sockaddr_in serv_addr;
    char start_bit;
    char buffer[MAX_SIZE];

    //inizializziamo il bit a 1 da inviare al ServerVaccinale
    start_bit = '1';

    socket_fd = Socket(AF_INET,SOCK_STREAM,0);

    //definisco il protocollo e la porta di accesso tra centro_vaccinale e serverVaccinale
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(1025);

    //conversione
    if(inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr)<=0){
        perror("inet_pton() error");
        exit(1);
    }

    //tentativo connessione con il server
    client_fd = Connect(socket_fd,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

    //inviamo al server vaccinale il bit 1 per informarlo che la comunicazione é con il centro_vaccinale 
    if(Full_write(socket_fd,&start_bit,sizeof(char))<0){
        perror("full_write() error");
        exit(1);
    }

    //invio effettivo del green pass
    if(Full_write(socket_fd, &gp, sizeof(gp))<0){
        perror("full_write() error");
        exit(1);
    }

    close(socket_fd);

}

//funzione di generazione green pass dopo connessione con utente
void answer_user(int connect_fd){
    //centri_vaccinali random
    char *hub_name[] = {"Casoria", "Napoli", "Afragola", "Frattamaggiore", "Roma", "Palermo", "Varese", "Milano", "Bologna", "Udine"};
    char buffer[MAX_SIZE];
    int index, message_welcome_size, package_size;
    vax_request package;
    GreenPass_Date gp;

    //selezioniamo a caso un centro vaccinale tra quelli sopra definiti
    srand(time(NULL));
    index = rand() % 10;

    //stampiamo nel buffer il messaggio di benvenuto
    snprintf(buffer,MAX_SIZE,"***Benvenuto nel centro vaccinale di %s\nInserire Nome, Cognome e Identificativo della tessera sanitaria\n***",hub_name[index]);
    //definiamo la lunghezza del messaggio di benvenuto
    message_welcome_size = sizeof(buffer);

    //inviamo i byte di scrittura del buffer
    if(Full_write(connect_fd,&message_welcome_size,sizeof(int))<0){
        perror("full_write() error\n");
        exit(1);
    }

    //successivamente passiamo al buffer il messaggio di benvenuto
    if(Full_write(connect_fd,buffer,message_welcome_size)<0){
        perror("full_write() error\n");
        exit(1);
    }

     //Riceviamo le informazioni per il GreenPass dall'Utente
    if(Full_read(connect_fd, &package, sizeof(vax_request)) < 0) {
        perror("full_read() error");
        exit(1);
    }
    printf("\nDati ricevuti\n");
    printf("Nome: %s\n", package.name);
    printf("Cognome: %s\n", package.surname);
    printf("Numero Tessera Sanitaria: %s\n\n", package.ID);

    //copio identificativo della tessera inviato dall'utente nel green_pass che sará mandato al serverVaccinale
    strcpy(gp.ID,package.ID);
    //definisco data di inizio e data di fine validitá del Green Pass
    create_start_date(&gp.start_date);
    create_end_date(&gp.end_date);

    close(connect_fd);
    send_GreenPass(gp);
}
