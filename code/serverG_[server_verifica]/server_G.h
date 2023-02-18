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

#define MAX_SIZE 1024  //dim massima del buffer
#define WELCOME_SIZE 108
#define ID_SIZE 21    //dim della tessera sanitaria
#define ACK_SIZE 64
#define ASL_ACK 39

//Struct del pacchetto dell'ASL contenente ID e referto di validitá
typedef struct{
    char ID[ID_SIZE];
    char report;
} report_client;

typedef struct{
    int day;
    int month;
    int year;
}DATE;

//Struct del pacchetto inviato con id utente, data inizio green_pass, data fine green_pass
typedef struct{
    char ID[ID_SIZE];
    char report; //0 GreenPass non valido, 1 GreenPass valido
    DATE start_date;
    DATE end_date;
}GreenPass_Request;

void handler(int sign);
void create_data_corrente(DATE *start_date);
char verifica_ID(char ID[]);
void receive_ID(int connect_fd);
char send_report(report_client package);
void receive_report(int connect_fd);

