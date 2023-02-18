#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include "wrapper.h"

int Socket(int family, int type, int protocol){
    int fd;
    if((fd = socket(family,type,protocol))<0){
        perror("Error Socket Server\n");
        exit(EXIT_FAILURE);
    }
    return(fd);
}

void Bind(int sockfd,struct sockaddr* address,socklen_t length){
    if((bind(sockfd,address,length))<0){
        perror("Error Bind Server\n");
        exit(EXIT_FAILURE);
    }
}

void Listen(int sockfd, int n_queue){
    if((listen(sockfd,n_queue))<0){
        perror("Error Listen Server\n");
        exit(EXIT_FAILURE);
    }
}

int Accept(int sockfd, struct sockaddr* address, socklen_t* length){
    int new_socket;
    if((new_socket = accept(sockfd,address,length))<0){
        perror("Error Accept Server\n");
        exit(EXIT_FAILURE);
    }
    return(new_socket);
}

int Connect(int sockfd,struct sockaddr* address, unsigned int lenght){
    int client_fd;
    if((client_fd = connect(sockfd,address,lenght))<0){
        perror("Errore Connect Client");
        exit(EXIT_FAILURE);
    }
    return(client_fd);
}

ssize_t Full_read(int fd, void *buffer, size_t count) {
    size_t n_left;
    ssize_t n_read;
    n_left = count;
    while (n_left > 0) {  
        // repeat finchè non ci sono left
        if ((n_read = read(fd, buffer, n_left)) < 0) {
            if (errno == EINTR) continue; 
            // Se si verifica una System Call che interrompe ripeti il ciclo
            else exit(n_read);
        } else if (n_read == 0) break; 
        // Se sono finiti, esci
        n_left -= n_read;
        buffer += n_read;
    }
    buffer = 0;
    return n_left;
}

ssize_t Full_write(int fd, const void *buffer, size_t count) {
    size_t n_left;
    ssize_t n_written;
    n_left = count;
    while (n_left > 0) {          
        //repeat finchè non ci sono left
        if ((n_written = write(fd, buffer, n_left)) < 0) {
            if (errno == EINTR) continue; 
            //Se si verifica una System Call che interrompe ripeti il ciclo
            else exit(n_written); 
            //Se non è una System Call, esci con un errore
        }
        n_left -= n_written;
        buffer += n_written;
    }
    buffer = 0;
    return n_left;
}

