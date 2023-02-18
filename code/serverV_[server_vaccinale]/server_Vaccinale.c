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
#include "../wrapper.h"
#include "server_V.h"

int main(){
    signal(SIGINT,handler); //intercetta comando CNTL + C
    int listen_fd;
    int connect_fd;
    struct sockaddr_in serv_addr;
    char bit_communication;
    pid_t pid;


    listen_fd = Socket(AF_INET,SOCK_STREAM,0);

    //struttura
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //accetta associazioni qualsiasi indirizzo associato al server
    serv_addr.sin_port = htons(1025);

    Bind(listen_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
    Listen(listen_fd,1024); //in attesa fino a 1024 connessioni

    while (1)
    {
        printf("***In attesa di nuove connesioni***\n\n");
        connect_fd = Accept(listen_fd,(struct sockaddr *)NULL,NULL);

        if((pid = fork())<0){
            perror("fork() error\n");
            exit(1);
        }

        if(pid==0){
            close(listen_fd);
            if((Full_read(connect_fd,&bit_communication,sizeof(char)))<0){
                perror("fork() error\n");
                exit(1);
            }
            if(bit_communication == '1'){
                centro_vaccinale_communication(connect_fd);
            }else if(bit_communication == '0'){
                server_verifica_communication(connect_fd);
            }else{
                printf("Client NOT FOUND\n\n");
            }
            close(connect_fd);
            exit(0);
        }else{
            close(connect_fd);
        }
    }
}



