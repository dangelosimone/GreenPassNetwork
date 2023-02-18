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
#include "server_V.h"
#include "../wrapper.h"


void handler(int sign){
    if(sign == SIGINT){
        printf("\nExit...\n");
        sleep(2);
        printf("***Thank you for using our service***\n");
        exit(0);
    }
}

//funzione che invia un GP rihiesto dal server_verifica (ServerG)
void send_gp(int connect_fd){
    char ID[ID_SIZE];
    char report;
    int fd;
    GreenPass_Request gp;

    //scrive in ID l'identificativo della tessera sanitaria ricevuta da server_verifica da connect_fd
    if(Full_read(connect_fd,ID,ID_SIZE)<0){
        perror("full_read() error");
        exit(1);
    }

    //apriamo il file rinominato ID, cioé l'identificativo inviato da server_verifica
    fd = open(ID,O_RDONLY,0777);

    if(errno == 2){ //errno = 2 se la open é fallita non avendo trovato alcun numero di tessera sanitaria ("No such file or directory")
        printf("Numero tessera sanitaria non esistente, riprovare...\n");
        report = '2';

        //invio il report al ServerVerifica
        if(Full_write(connect_fd,&report,sizeof(char))<0){
            perror("full_read() error");
            exit(1);
        }
    }else{
        if(flock(fd,LOCK_EX)<0){ //macro LOCK_EX definisce che solo un processo alla volta può accedere al blocco
            perror("flock() error");
            exit(1);
        }
        //lettura del GreenPass del file aperto precedentemente
        if(read(fd,&gp,sizeof(GreenPass_Request))<0){
            perror("read() error");
            exit(1);
        }
        if(flock(fd,LOCK_UN)<0){ //macro LOCK_UN rimuove la function flock con macro LOCK_EX definita precedentemente
            perror("flock() error");
            exit(1);
        }

        close(fd);
        report= '1';

        //inviamo il report al server_verifica
        if(Full_write(connect_fd,&report,sizeof(char))<0){
            perror("full_write() error");
            exit(1);
        }

        //mandiamo il GreenPass richiesto al server_verifica
        if(Full_write(connect_fd,&gp,sizeof(GreenPass_Request))<0){
            perror("full_write() error");
            exit(1);
        }
    }
}

//funzione che modifica il report secondo volere dell'asl
void modify_report(int connect_fd){
    report_client package;
    GreenPass_Request gp;
    int fd;
    char report;
    //riceve report_client da server_vaccinale da asl con identificativo tessera sanitaria e referto
    if(Full_read(connect_fd,&package,sizeof(report_client))<0){
        perror("full_read() error");
        exit(1);
    }

    fd = open(package.ID,O_RDWR,0777);

    if(errno == 2){
        printf("Identificativo tessera sanitaria inesistente\n");
        report = '1';
    }else{
        //LOCK_NB ritorno immediato
        if(flock(fd,LOCK_EX | LOCK_NB)<0){
            perror("flock() error");
            exit(1);
        }

        if(read(fd,&gp,sizeof(GreenPass_Request))<0){
            perror("read() error");
            exit(1);
        }


        
        //assegnamo al report del GP associato al ID dell'asl il report che quest'ultimi voglion modificare
        gp.report = package.report;
        lseek(fd,0,SEEK_SET); //ritorniamo a inizio dello stream del file

        if(write(fd,&gp,sizeof(GreenPass_Request))<0){
            perror("write() error");
            exit(1);
        }

        if(flock(fd,LOCK_UN)<0){
            perror("read() error");
            exit(1);
        }
        report = '0';
    }
    //invio report al server di verifica
    if(Full_write(connect_fd,&report,sizeof(char))<0){
        perror("full_write() error");
        exit(1);
    }
}

//funzione di comunicazione con ServerVerifica
//ricava il green pass dal filesystem dalla tessera sanitaria ricevuta e lo invia al server_verifica

void server_verifica_communication(int connect_fd){
    char bit_communication;

    //dato che utilizza 2 funzioni diverse
    // se riceve bit 0 server_vaccinale modificherá report
    // se riceve bit 1 server_vaccinale invierá green pass
    if(Full_read(connect_fd,&bit_communication,sizeof(char))<0){
        perror("full_read() error");
        exit(1);
    }
    if(bit_communication == '0'){modify_report(connect_fd);
    }else if(bit_communication == '1'){send_gp(connect_fd);
    }else{printf("bit_communication NOT FOUND\n\n");}
}

//funzione per comunicazione con centro_vaccinale
void centro_vaccinale_communication(int connect_fd){
    int fd;
    GreenPass_Request gp;

    //ricavo il green pass dal centro vaccinale
    if(Full_read(connect_fd,&gp,sizeof(GreenPass_Request))<0){
        perror("full_read() error");
        exit(1);
    }
    gp.report = '1'; //quando generato un nuovo GP risulterà ovviamente valido
    //per ogni tessera sanitaria crea un file contente i dati ricevuti
    printf("Nome file creato: %s \n", gp.ID);
    if ((fd = open(gp.ID, O_WRONLY| O_CREAT | O_TRUNC, 0777)) < 0) {
        perror("open() error");
        exit(1);
    }
    //scriviamo nel file creato i campi di GP associati a quella tessera sanitaria
    if(write(fd,&gp,sizeof(GreenPass_Request))<0){
        perror("write() error");
        exit(1);
    }
    close(fd); //chiudiamo il file creato    
}


