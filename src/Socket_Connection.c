#include "../inc/socket_connection.h"

SharedQueue shared_queue;
int numReadings;
#define RUNNING_AVG_WINDOW 10   

/**
 * @brief Create a socket connection
 */
void Close_Client_Connection(int *client_sockets, int index, fd_set *read_fds) {
    printf("Client at socket %d disconnected.\n", index);
    Log_CloseConnection(index);
    close(client_sockets[index]);
    FD_CLR(client_sockets[index], read_fds);
    client_sockets[index] = 0;
}

/**
 * @brief Process received data from client
 */
void Process_Received_Data(int sock, char *buffer, int index, int *client_sockets, fd_set *read_fds) {
    SensorData sensor_data;

    buffer[strcspn(buffer, "\n")] = 0;  // Remove newline character
    printf("Message from client %d: %s\n", index, buffer);

    if (strncmp(buffer, "exit", 4) == 0) {
        printf("Client at socket %d requested disconnection.\n", index);
        Close_Client_Connection(client_sockets, index, read_fds);
    } else {
        strncpy(sensor_data.data, buffer, sizeof(sensor_data.data) - 1);
        sensor_data.sensorNodeID = index;
        enqueue(&shared_queue, &sensor_data);
    }
}


/**
 * @brief Handle messages from clients
 */
void Handle_Client_Message(int *client_sockets, fd_set *temp_fds, fd_set *read_fds) {
    char buffer[BUFF_SIZE];
    int valread;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        int sock = client_sockets[i];

        if (FD_ISSET(sock, temp_fds)) {
            valread = read(sock, buffer, BUFF_SIZE - 1);

            if (valread <= 0) {
                Close_Client_Connection(client_sockets, i, read_fds);
            } else {
                buffer[valread] = '\0';
                Process_Received_Data(sock, buffer, i, client_sockets, read_fds);
            }
        }
    }
}

/**
 * @brief Accept new client connection
 * 
 */
void Accept_New_Client(int server_fd, int *client_sockets, fd_set *read_fds, int *max_fd) {
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    int new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);

    if (new_socket < 0) {
        perror("accept()");
        return;
    }

    printf("New connection: [%s:%d]\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    Log_OpenConnection(ntohs(client_addr.sin_port));

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_sockets[i] == 0) {
            client_sockets[i] = new_socket;
            FD_SET(new_socket, read_fds);
            if (new_socket > *max_fd)
                *max_fd = new_socket;
            printf("Added to list of sockets as %d\n", i);
            return;
        }
    }

    printf("Maximum clients reached. Closing connection.\n");
    close(new_socket);
}

/**
 * @brief Handle client connections
 * 
 * @param server_fd File descriptor of the server
 */
void Client_Handler(int server_fd) {
    fd_set read_fds, temp_fds;
    int client_sockets[MAX_CLIENTS] = {0};
    int max_fd = server_fd;
    int activity;

    FD_ZERO(&read_fds);
    FD_SET(server_fd, &read_fds);
    printf("Server is ready to accept clients...\n");

    while (1) {
        temp_fds = read_fds;

        activity = select(max_fd + 1, &temp_fds, NULL, NULL, NULL);
        if (activity < 0 && errno != EINTR) {
            perror("select()");
            break;
        }

        if (FD_ISSET(server_fd, &temp_fds)) {
            Accept_New_Client(server_fd, client_sockets, &read_fds, &max_fd);
        }

        Handle_Client_Message(client_sockets, &temp_fds, &read_fds);
    }
}

/**
 * @brief Create a socket connection
 */
int CreateSocket(int port_no, int *server_fd, struct sockaddr_in *server_addr)
{
    /* Tạo socket */
    *server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*server_fd == -1)
        handle_error("socket()");

    /* Set socket options */
    int opt = 1;
    if (setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        handle_error("setsockopt()");  

    /* Bind socket */
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(port_no);
    server_addr->sin_addr.s_addr =  INADDR_ANY;
    if (bind(*server_fd, (struct sockaddr*)server_addr, sizeof(*server_addr)) == -1)
        handle_error("bind()");
    if (listen(*server_fd, LISTEN_BACKLOG) == -1)
        handle_error("listen()");
    
        return 0;
}