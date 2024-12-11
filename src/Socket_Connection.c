#include "../inc/Socket_Connection.h"

SharedQueue shared_queue;
int numReadings;
#define RUNNING_AVG_WINDOW 10   // Kích thước cửa sổ cho trung bình động
#define TEMP_THRESHOLD_HOT 30  // Ngưỡng nhiệt độ "quá nóng"
#define TEMP_THRESHOLD_COLD 10 // Ngưỡng nhiệt độ "quá lạnh"

void Client_Handler(int server_fd)
{
    SensorData sensor_data;
    fd_set read_fds, temp_fds;
    int client_sockets[MAX_CLIENTS] = {0}; // Danh sách socket của các client
    int max_fd = server_fd;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);
    char buffer[BUFF_SIZE];
    int activity, new_socket, i;

    // Khởi tạo tập file descriptor
    FD_ZERO(&read_fds);
    FD_SET(server_fd, &read_fds);
    printf("Server is ready to accept clients...\n");
    
    while (1) {
        temp_fds = read_fds; // Sao chép tập file descriptor
        // Chờ sự kiện trên các file descriptor
        activity = select(max_fd + 1, &temp_fds, NULL, NULL, NULL);
        if (activity < 0 && errno != EINTR) {
            perror("select()");
            break;
        }
        // Kiểm tra nếu có kết nối mới
        if (FD_ISSET(server_fd, &temp_fds)) {
            new_socket = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);
            if (new_socket < 0) {
                perror("accept()");
                continue;
            }
            printf("New connection: [%s:%d]\n",
                   inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            Log_OpenConnection(ntohs(client_addr.sin_port));
            // Thêm socket mới vào danh sách client
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = new_socket;
                    FD_SET(new_socket, &read_fds);
                    if (new_socket > max_fd)
                        max_fd = new_socket;
                    printf("Added to list of sockets as %d\n", i);
                    break;
                }
            }
            // Nếu danh sách đầy, từ chối kết nối
            if (i == MAX_CLIENTS) {
                printf("Maximum clients reached. Closing connection.\n");
                close(new_socket);
            }
        }
        // Kiểm tra dữ liệu từ các client hiện có
        for (i = 0; i < MAX_CLIENTS; i++) {
            int sock = client_sockets[i];
            if (FD_ISSET(sock, &temp_fds)) {
                int valread = read(sock, buffer, BUFF_SIZE - 1);
                if (valread <= 0) {
                    // Client ngắt kết nối
                    printf("Client at socket %d disconnected.\n", i);
                    Log_CloseConnection(ntohs(client_addr.sin_port));
                    close(sock);
                    FD_CLR(sock, &read_fds);
                    client_sockets[i] = 0;
                } else {
                    // Xử lý dữ liệu nhận được
                    buffer[valread] = '\0';
                    printf("Message from client %d: %s\n", i, buffer);        
                    if (strncmp(buffer, "exit", 4) == 0) {
                        printf("Client at socket %d requested disconnection.\n", i);
                        Log_CloseConnection(ntohs(client_addr.sin_port));
                        close(sock);
                        FD_CLR(sock, &read_fds);
                        client_sockets[i] = 0;
                    }
                    else 
                    {
                        // Chuẩn bị dữ liệu để ghi vào hàng đợi
                        strncpy(sensor_data.data, buffer, sizeof(sensor_data.data) - 1);
                        sensor_data.sensorNodeID = ntohs(client_addr.sin_port);
                        // Ghi dữ liệu vào hàng đợi
                        enqueue(&shared_queue, &sensor_data);
                    }
                    
                }
            }
        }
    }
}

int CreateSocket(int port_no, int *server_fd, struct sockaddr_in *server_addr)
{
    /* Tạo socket */
    *server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (*server_fd == -1)
        handle_error("socket()");
    /* Ngăn lỗi : “address already in use” */
    int opt = 1;
    if (setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
        handle_error("setsockopt()");  
    /* Khởi tạo địa chỉ cho server */
    server_addr->sin_family = AF_INET;
    server_addr->sin_port = htons(port_no);
    server_addr->sin_addr.s_addr =  INADDR_ANY;
    /* Gắn socket với địa chỉ server */
    if (bind(*server_fd, (struct sockaddr*)server_addr, sizeof(*server_addr)) == -1)
        handle_error("bind()");
    /* Nghe tối đa 5 kết nối trong hàng đợi */
    if (listen(*server_fd, LISTEN_BACKLOG) == -1)
        handle_error("listen()");
    return 0;
}