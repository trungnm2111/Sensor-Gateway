#ifndef __SOCKET_CONNECTION_H__
#define __SOCKET_CONNECTION_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h> 
#include <string.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>     //  socket definitions
#include <netinet/in.h>     
#include <errno.h>

#include "log_manager.h"
#include "queue_share.h"

#define MAX_CLIENTS 5       // number of clients
#define MAX_CONNECTIONS 5   // number of connections
#define BUFFER_SIZE 256     // buffer size

#define LISTEN_BACKLOG 50
#define BUFF_SIZE 256
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

    
int CreateSocket(int port_no, int *server_fd, struct sockaddr_in *server_addr);
void Client_Handler(int server_fd);

#endif