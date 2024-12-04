#include "../inc/Main_Process.h"

/**
 * @brief Tạo FIFO
 * 
 */
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

/**
 * @brief Thread quản lý kết nối
 * 
 * @param arg Tham số truyền vào
 * @return void* 
 */
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

/**
 * @brief Thread quản lý dữ liệu
 * 
 * @param arg Tham số truyền vào
 * @return void* 
 */
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

/**
 * @brief Thread control storage
 * 
 * @param arg Tham số truyền vào
 * 
 * @return void*
 */
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

/**
 * @brief Writes a log event to the FIFO log system.
 * 
 * @param log_event The log event to write.
 * 
 * This function writes a given log event to the FIFO log system.
 * It ensures that the log event is written atomically and consistently.
 */
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

/**
 * @brief Formats a log event into a human-readable message.
 * 
 * @param event The event to format.
 * @param buffer The buffer to store the formatted message.
 * @param buffer_size The size of the buffer.
 * 
 * This function formats a given `LogEvent` structure into a human-readable
 * log message and stores it in the provided buffer. It ensures that the event
 * is recorded in a structured and consistent format.
 */
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
 */
void log_event(LogEvent event) 
{
    char log_message[512];
    format_log_event(event, log_message, sizeof(log_message));
    write_logEvent(log_message); // Ghi log-event vào FIFO
}

/**
 * @brief Logs a connection opened event.
 * 
 * @param sensorNodeID The ID of the sensor node that opened the connection.
 * 
 * This function logs a connection opened event to the FIFO log system.
 * It records the sensor node ID that opened the connection.
 */
void connection_opened(int sensorNodeID) {
    LogEvent event = { .type = CONNECTION_OPENED, .sensorNodeID = sensorNodeID };
    log_event(event);
}

/**
 * @brief Logs a connection closed event.
 * 
 * @param sensorNodeID The ID of the sensor node that closed the connection.
 * 
 * This function logs a connection closed event to the FIFO log system.
 * It records the sensor node ID that closed the connection.
 */
void connection_closed(int sensorNodeID) {
    LogEvent event = { .type = CONNECTION_CLOSED, .sensorNodeID = sensorNodeID };
    log_event(event);
}

/**
 * @brief Logs a sensor node reporting too cold event.
 * 
 * @param sensorNodeID The ID of the sensor node that reported the event.
 * @param avg_temp The average temperature value reported by the sensor node.
 * 
 * This function logs a sensor node reporting too cold event to the FIFO log system.
 * It records the sensor node ID and the average temperature value.
 */
void Sensor_ColdReport(int sensorNodeID, double avg_temp) {
    LogEvent event = { .type = TOO_COLD, .sensorNodeID = sensorNodeID, .temperatureValue = avg_temp };
    log_event(event);
}

/**
 * @brief Logs a sensor node reporting too hot event.
 * 
 * @param sensorNodeID The ID of the sensor node that reported the event.
 * @param avg_temp The average temperature value reported by the sensor node.
 * 
 * This function logs a sensor node reporting too hot event to the FIFO log system.
 * It records the sensor node ID and the average temperature value.
 */
void Sensor_HotReport(int sensorNodeID, double avg_temp) {
    LogEvent event = { .type = TOO_HOT, .sensorNodeID = sensorNodeID, .temperatureValue = avg_temp };
    log_event(event);
}

/**
 * @brief Logs an invalid sensor node ID event.
 * 
 * @param nodeID The invalid sensor node ID.
 * 
 * This function logs an invalid sensor node ID event to the FIFO log system.
 * It records the invalid sensor node ID.
 */
void Sensor_IdInvalid(int nodeID) {
    LogEvent event = { .type = INVALID_SENSOR_NODE_ID, .sensorNodeID = nodeID };
    log_event(event);
}

/**
 * @brief Logs a SQL connection established event.
 * 
 * This function logs a SQL connection established event to the FIFO log system.
 */
void Sql_EstablishedConnection() {
    LogEvent event = { .type = SQL_CONNECTION_ESTABLISHED };
    log_event(event);
}

/**
 * @brief Logs a new table created event.
 * 
 * @param table_name The name of the new table created.
 * 
 * This function logs a new table created event to the FIFO log system.
 * It records the name of the new table created.
 */
void Sql_CreateTable(const char *table_name) {
    LogEvent event = { .type = TABLE_CREATED };
    snprintf(event.extraInfo, sizeof(event.extraInfo), "%s", table_name);
    log_event(event);
}

/**
 * @brief Logs a SQL connection lost event.
 * 
 * This function logs a SQL connection lost event to the FIFO log system.
 */
void Sql_LostConnection() {
    LogEvent event = { .type = SQL_CONNECTION_LOST };
    log_event(event);
}

/**
 * @brief Logs a SQL connection failed event.
 * 
 * This function logs a SQL connection failed event to the FIFO log system.
 */
void Sql_FailedConnection() {
    LogEvent event = { .type = SQL_CONNECTION_FAILED };
    log_event(event);
}
