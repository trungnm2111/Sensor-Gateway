#include "../inc/Main_Process.h"

extern SharedQueue shared_queue; 
SensorNodeState sensorStates[MAX_NODES];
void Data_CaculationAverage(SensorData sensor_data);

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
    SensorData sensor_data;
    for (int i = 0; i < MAX_NODES; i++) {
        sensorStates[i].sensorNodeID = i;
        sensorStates[i].runningAverage = 0.0;
        sensorStates[i].sampleCount = 0;
    }
    while(1)
    {
        dequeue(&shared_queue, &sensor_data); // lấy dữ liệu có trong queue 
        printf("Data from sensor %d: %s\n", sensor_data.sensorNodeID, sensor_data.data);
        Data_CaculationAverage(sensor_data);
    }
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

// Khởi tạo hàng đợi
void init_queue(SharedQueue *q) {
    q->front = 0;
    q->rear = 0;
    q->count = 0;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->cond, NULL);
}

// Thêm phần tử vào hàng đợi
void enqueue(SharedQueue *q, SensorData *data) 
{
    pthread_mutex_lock(&q->mutex);

    // Kiểm tra hàng đợi có đầy không
    if (q->count == MAX_QUEUE_SIZE) {
        printf("Queue is full! Dropping data.\n");
        pthread_mutex_unlock(&q->mutex);
        return;
    }

    // Thêm dữ liệu vào hàng đợi
    q->queue[q->rear] = *data;
    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    q->count++;

    // Thông báo cho thread đang chờ
    pthread_cond_signal(&q->cond);

    pthread_mutex_unlock(&q->mutex);
}

// Lấy phần tử từ hàng đợi
int dequeue(SharedQueue *q, SensorData *data) 
{
    pthread_mutex_lock(&q->mutex);

    // Chờ khi hàng đợi trống
    while (q->count == 0) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }

    // Lấy dữ liệu từ hàng đợi
    *data = q->queue[q->front];
    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    q->count--;

    pthread_mutex_unlock(&q->mutex);
    return 0;
}

void Data_CaculationAverage(SensorData sensor_data)
{
    double temperature = atof(sensor_data.data);
    // Tìm trạng thái sensor tương ứng
    int nodeID = sensor_data.sensorNodeID;
    if (nodeID < 0 ) {
        printf("Invalid sensor node ID: %d\n", nodeID);
        Log_InvalidIDSensor(nodeID);
        return; // Bỏ qua dữ liệu không hợp lệ
    }
    SensorNodeState *state = &sensorStates[nodeID];
    // Cập nhật trung bình động
    state->runningAverage = (state->runningAverage * state->sampleCount + temperature) / (state->sampleCount + 1);
    state->sampleCount++;
    if (state->runningAverage > 30) {
        Log_ReportHotSensor(nodeID,state->runningAverage);
        // printf("Node %d: Too Hot (%.2f°C)\n", nodeID, state->runningAverage);
    } else if (state->runningAverage < 15) {
        Log_ReportColdSensor(nodeID,state->runningAverage);
        // printf("Node %d: Too Cold (%.2f°C)\n", nodeID, state->runningAverage);
    }
}