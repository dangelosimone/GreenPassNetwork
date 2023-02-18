#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>      // libreria standard del C che contiene definizioni di macro per la gestione delle situazioni di errore.
#include <string.h>
#include <fcntl.h>      // contiene opzioni di controllo dei file
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/socket.h> //contiene le definizioni dei socket.
#include <arpa/inet.h>  // contiene le definizioni per le operazioni Internet.
#include <time.h>
#include <signal.h> 

#define MAX_SIZE 2048
#define ID_SIZE 21

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
void send_gp(int connect_fd);
void modify_report(int connect_fd);
void server_verifica_communication(int connect_fd);
void centro_vaccinale_communication(int connect_fd);