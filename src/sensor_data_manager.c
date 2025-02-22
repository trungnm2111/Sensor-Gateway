#include "../inc/sensor_data_manager.h"

static void Data_CaculationAverage(SensorData sensor_data);

int port_no = 0; // Port number to connect to


void createFifo() 
{
    // Create FIFO if not exists
    if(access(FIFO_NAME, F_OK) == -1) 
    {
        while (mkfifo(FIFO_NAME, 0666) != 0) 
            handle_error("Create FiFO()");
        printf("Create FiFo successfully\n");
    }
}


void *Thread_ConnectionManger(void *arg)
{
    int server_fd, new_socket_fd;
    int client_fd;
    struct sockaddr_in serv_addr, client_addr;
    socklen_t len = sizeof(client_addr);

    memset(&serv_addr, 0, sizeof(struct sockaddr_in));
    memset(&client_addr, 0, sizeof(struct sockaddr_in));
    /* Create a socket */
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
        dequeue(&shared_queue, &sensor_data); 
        if (strlen(sensor_data.data) >= sizeof(sensor_data.data)) {
            printf("Data string overflow detected\n");
            continue; 
        }

        printf("Data from sensor %d: %s\n", sensor_data.sensorNodeID, sensor_data.data);
        Data_CaculationAverage(sensor_data);
        enqueue(&shared_queue, &sensor_data);
        sleep(10);
    }
    return NULL;
}



void *Thread_StorageManager(void *arg) {
    SensorData sensor_data;
    
    MYSQL *db_conn = NULL;
    int result = HandleDatabaseStorage(&sensor_data);
    if (result == -1) {
        pthread_exit(NULL);
    }
    
    return NULL;
}

void system_threads_init()
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

static void Data_CaculationAverage(SensorData sensor_data)
{
    double temperature = atof(sensor_data.data);
    int nodeID = sensor_data.sensorNodeID;
    if (nodeID < 0 || nodeID >= MAX_NODES) {
        printf("Invalid sensor node ID: %d\n", nodeID);
        Log_InvalidIDSensor(nodeID);
        return; 
    }

    SensorNodeState *state = &sensorStates[nodeID];
    // Update the running average
    state->runningAverage = (state->runningAverage * state->sampleCount + temperature) / (state->sampleCount + 1);
    state->sampleCount++;

    // Check for too hot or too cold
    if (state->runningAverage > 30) {
        Log_ReportHotSensor(nodeID, state->runningAverage);
    } else if (state->runningAverage < 15) {
        Log_ReportColdSensor(nodeID, state->runningAverage);
    }

}
