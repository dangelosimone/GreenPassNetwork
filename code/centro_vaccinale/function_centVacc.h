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

#define MAX_SIZE 1024
#define MESS_CVACC 64
#define ID_SIZE 21

typedef struct{
    char name[MAX_SIZE];
    char surname[MAX_SIZE];
    char ID[ID_SIZE];
} vax_request;

typedef struct{
    int day;
    int month;
    int year;
}DATE;

//Struct del pacchetto inviato con id utente, data inizio green_pass, data fine green_pass
typedef struct{
    char ID[ID_SIZE];
    DATE start_date;
    DATE end_date;
}GreenPass_Date;

void handler(int sign);
void create_end_date(DATE *end_date);
void create_start_date(DATE *start_date);
void send_GreenPass(GreenPass_Date gp);
void answer_user(int connect_fd);