#include "../inc/Main_Process.h"
void createFifo() 
{
    // Kiểm tra xem file có tồn tại không
    if(access(FIFO_NAME, F_OK) == -1) {
        while (mkfifo(FIFO_NAME, 0666) != 0) {
            perror("Unable to create fifo");
            exit(1);
        }
        printf("Create FiFo successfully\n");
    }
}


// luong quan ly ket noi
void *Thread_ConnectionManger(void *arg)
{
    connection_opened(1);
    connection_closed(1);
    sleep(1);
    // char log_event[256];
    // for (int i = 1; i <= 5; i++) {
    //     snprintf(log_event, sizeof(log_event), "Sensor node %d connected\n", i);
    //     write_logEvent(log_event);
    //     sleep(1);
    // }
    return NULL;
}

// luong quan ly du lieu
void *Thread_DataManager(void *arg)
{
    Sensor_ColdReport(1, 25.5);
    Sensor_HotReport(1, 30.5);
    Sensor_IdInvalid(999);
    sleep(1);
    // for (int i = 1; i <= 5; i++) {
    //     char log_event[256];
    //     snprintf(log_event, sizeof(log_event), "Sensor node %d: temperature too high\n", i);
    //     write_logEvent(log_event);
    //     sleep(1);
    // }
    return NULL;
}

// luong quan ly luu tru 
void *Thread_StorageManager(void *arg)
{
    Sql_EstablishedConnection();
    Sql_CreateTable("temperature_data");
    Sql_LostConnection();
    Sql_FailedConnection();
    return NULL;
    // for (int i = 1; i <= 5; i++) {
    //     char log_event[256];
    //     snprintf(log_event, sizeof(log_event), "Storage manager: Storing data...\n");
    //     write_logEvent(log_event);
    //     sleep(1);
    // }
}

void mainProcess()
{
    // Khởi tạo mutex
    pthread_mutex_init(&resource.mutex, NULL);

    // Khởi tạo các luồng
    pthread_t connection_thread, data_thread, storage_thread;
    pthread_create(&connection_thread, NULL, Thread_ConnectionManger, NULL);
    pthread_create(&data_thread, NULL, Thread_DataManager, NULL);
    pthread_create(&storage_thread, NULL, Thread_StorageManager, NULL);

    // Chờ các luồng kết thúc
    pthread_join(connection_thread, NULL);
    pthread_join(data_thread, NULL);
    pthread_join(storage_thread, NULL);

    // Hủy mutex
    pthread_mutex_destroy(&resource.mutex);
}

// // Hàm ghi log-event vào FIFO
void write_logEvent(const char *log_event)
{
    pthread_mutex_lock(&resource.mutex);

    int fifo_fd = open(FIFO_NAME, O_WRONLY);
    if (fifo_fd < 0) {
        perror("Unable to open FIFO for writing");
    } else {
        write(fifo_fd, log_event, strlen(log_event));
        close(fifo_fd);
    }
    pthread_mutex_unlock(&resource.mutex);
}


void format_log_event(LogEvent event, char *buffer, size_t buffer_size) 
{
    switch (event.type) {
        case CONNECTION_OPENED:
            snprintf(buffer, buffer_size, "Sensor node with ID <%d> has opened a new connection\n", event.sensorNodeID);
            break;
        case CONNECTION_CLOSED:
            snprintf(buffer, buffer_size, "Sensor node with ID <%d> has closed the connection\n", event.sensorNodeID);
            break;
        case TOO_COLD:
            snprintf(buffer, buffer_size, "The sensor node with ID <%d> reports it's too cold (running avg temperature = %.2f)\n",
                     event.sensorNodeID, event.temperatureValue);
            break;
        case TOO_HOT:
            snprintf(buffer, buffer_size, "The sensor node with ID <%d> reports it's too hot (running avg temperature = %.2f)\n",
                     event.sensorNodeID, event.temperatureValue);
            break;
        case INVALID_SENSOR_NODE_ID:
            snprintf(buffer, buffer_size, "Received sensor data with invalid sensor node ID <%d>\n", event.sensorNodeID);
            break;
        case SQL_CONNECTION_ESTABLISHED:
            snprintf(buffer, buffer_size, "Connection to SQL server established.\n");
            break;
        case TABLE_CREATED:
            snprintf(buffer, buffer_size, "New table <%s> created.\n", event.extraInfo);
            break;
        case SQL_CONNECTION_LOST:
            snprintf(buffer, buffer_size, "Connection to SQL server lost.\n");
            break;
        case SQL_CONNECTION_FAILED:
            snprintf(buffer, buffer_size, "Unable to connect to SQL server.\n");
            break;
        default:
            snprintf(buffer, buffer_size, "Unknown event type.\n");
            break;
    }
}


/**
 * @brief Logs an event to the FIFO log system.
 *
 * This function formats a given `LogEvent` structure into a human-readable
 * log message and writes it into a FIFO for further processing or storage.
 * It ensures that the event is recorded in a structured and consistent format.
 *
 * @param event The event to log, containing details such as the type of event,
 *              sensorNodeID, temperature value, or other additional information.
 *
 * @note The `LogEvent` structure must be properly initialized before calling this function.
 *       The FIFO must be created and available for writing to ensure successful operation.
 *
 * Example usage:
 * @code
 * LogEvent event = { .type = TOO_HOT, .sensorNodeID = 1, .temperatureValue = 35.5 };
 * log_event(event);
 * @endcode
 */
void log_event(LogEvent event) 
{
    char log_message[512];
    format_log_event(event, log_message, sizeof(log_message));
    write_logEvent(log_message); // Ghi log-event vào FIFO
}

void connection_opened(int sensorNodeID) {
    LogEvent event = { .type = CONNECTION_OPENED, .sensorNodeID = sensorNodeID };
    log_event(event);
}

void connection_closed(int sensorNodeID) {
    LogEvent event = { .type = CONNECTION_CLOSED, .sensorNodeID = sensorNodeID };
    log_event(event);
}

void Sensor_ColdReport(int sensorNodeID, double avg_temp) {
    LogEvent event = { .type = TOO_COLD, .sensorNodeID = sensorNodeID, .temperatureValue = avg_temp };
    log_event(event);
}

void Sensor_HotReport(int sensorNodeID, double avg_temp) {
    LogEvent event = { .type = TOO_HOT, .sensorNodeID = sensorNodeID, .temperatureValue = avg_temp };
    log_event(event);
}

void Sensor_IdInvalid(int nodeID) {
    LogEvent event = { .type = INVALID_SENSOR_NODE_ID, .sensorNodeID = nodeID };
    log_event(event);
}

void Sql_EstablishedConnection() {
    LogEvent event = { .type = SQL_CONNECTION_ESTABLISHED };
    log_event(event);
}

void Sql_CreateTable(const char *table_name) {
    LogEvent event = { .type = TABLE_CREATED };
    snprintf(event.extraInfo, sizeof(event.extraInfo), "%s", table_name);
    log_event(event);
}

void Sql_LostConnection() {
    LogEvent event = { .type = SQL_CONNECTION_LOST };
    log_event(event);
}

void Sql_FailedConnection() {
    LogEvent event = { .type = SQL_CONNECTION_FAILED };
    log_event(event);
}
