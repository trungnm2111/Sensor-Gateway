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
#include <sys/socket.h>     //  Chứa cấu trúc cần thiết cho socket. 
#include <netinet/in.h>     
#include <errno.h>

#include "Log_Process.h"
#include "Queue_Share.h"

#define MAX_CLIENTS 5       // Số lượng client tối đa
#define MAX_CONNECTIONS 5   // Số lượng kết nối tối đa
#define BUFFER_SIZE 256     // Kích thước buffer cho dữ liệu

#define LISTEN_BACKLOG 50
#define BUFF_SIZE 256
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int CreateSocket(int port_no, int *server_fd, struct sockaddr_in *server_addr);
void Client_Handler(int server_fd);

#endif