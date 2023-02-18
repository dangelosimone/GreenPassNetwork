#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>      // libreria standard del C che contiene definizioni di macro per la gestione delle situazioni di errore.
#include <string.h>
#include <netdb.h>      // contiene le definizioni per le operazioni del database di rete.
#include <sys/types.h>
#include <sys/socket.h> //contiene le definizioni dei socket.
#include <arpa/inet.h> 

int Socket(int family, int type, int protocol);
void Bind(int sockfd,struct sockaddr* address,socklen_t length);
void Listen(int sockfd, int n_queue);
int Accept(int sockfd, struct sockaddr* address, socklen_t* length);
int Connect(int sockfd,struct sockaddr* address, unsigned int lenght);
ssize_t Full_write(int fd, const void *buffer, size_t count);
ssize_t Full_read(int fd, void *buffer, size_t count);