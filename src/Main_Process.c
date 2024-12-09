#include "../inc/Main_Process.h"

int port_no = 0;
/**
 * @brief Tạo FIFO
 * 
 */
void createFifo() 
{
    // Kiểm tra xem file có tồn tại không
    if(access(FIFO_NAME, F_OK) == -1) 
    {
        while (mkfifo(FIFO_NAME, 0666) != 0) 
            handle_error("Create FiFO()");
        printf("Create FiFo successfully\n");
    }
}

/**
 * @brief Thread quản lý kết nối
 * 
 * @param arg Tham số truyền vào
 * @return void* 
 */
void *Thread_ConnectionManger(void *arg)
{
    int server_fd, new_socket_fd;
    int client_fd;
    struct sockaddr_in serv_addr, client_addr;
    socklen_t len = sizeof(client_addr);

    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    /* Tạo socket */
    if (CreateSocket(port_no, &server_fd, &serv_addr) == -1)
    {
        handle_error("CreateSocket()");
        pthread_exit(NULL);
    }
    printf("Server is listening at port : %d \n....\n",port_no);
    Client_Handler(server_fd); 
    close(server_fd);
    sleep(10);
    
    return NULL;
}


/**
 * @brief Thread quản lý dữ liệu
 * 
 * @param arg Tham số truyền vào
 * @return void* 
 */
void *Thread_DataManager(void *arg)
{
    // Sensor_ColdReport(1, 25.5);
    // Sensor_HotReport(1, 30.5);
    // Sensor_IdInvalid(999);
    sleep(10);
    return NULL;
}

/**
 * @brief Thread control storage
 * 
 * @param arg Tham số truyền vào
 * 
 * @return void*
 */
void *Thread_StorageManager(void *arg)
{
    // Sql_EstablishedConnection();
    // Sql_CreateTable("temperature_data");
    // Sql_LostConnection();
    // Sql_FailedConnection();
    sleep(10);
    return NULL;
}

void mainProcess()
{
    pthread_mutex_init(&resource.mutex, NULL);

    pthread_t connection_thread, data_thread, storage_thread;
    pthread_create(&connection_thread, NULL, Thread_ConnectionManger, NULL);
    pthread_create(&data_thread, NULL, Thread_DataManager, NULL);
    pthread_create(&storage_thread, NULL, Thread_StorageManager, NULL);

    pthread_join(connection_thread, NULL);
    pthread_join(data_thread, NULL);
    pthread_join(storage_thread, NULL);

    pthread_mutex_destroy(&resource.mutex);
}



