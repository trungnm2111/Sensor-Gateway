#include "../inc/log_manager.h"

/**
 * @brief Initializes the FIFO log system.
 */
void logProcess()
{
    // Create FIFO if not exists
    int fifo_fd = open(FIFO_NAME, O_RDONLY);
    if (fifo_fd < 0) {
        perror("Unable to open FIFO for reading");
        exit(EXIT_FAILURE);
    }
    
    // Open log file
    FILE *log_file = fopen("gateway.log", "a"); 
    if (!log_file) {
        perror("Unable to open log file");
        close(fifo_fd);
        exit(EXIT_FAILURE);
    }

    int sequence_number = 0;
    char buf[256];
    char timestamp[64];
    char *event;
    while(1)
    {
        ssize_t byte_read = read(fifo_fd, buf, sizeof(buf) - 1);
        if (byte_read > 0)
        {
            buf[byte_read] = '\0';          
            event = strtok(buf, "\n");
            while (event != NULL)
            {
                get_timeStamp(timestamp, sizeof(timestamp));
                fprintf(log_file, "<%d> <%s>: %s\n", sequence_number ,timestamp, event);
                fflush(log_file);           
                event = strtok(NULL, "\n");
                sequence_number ++;
            }
        }
        sleep(1);
    }
    fclose(log_file);
    close(fifo_fd);
}

/**
 * @brief Gets the current timestamp.
 */
void get_timeStamp(char *buffer, size_t size) 
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

/**
 * @brief Writes a log event to the FIFO log system.
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
 */
void format_log_event(LogEvent event, char *buffer, size_t buffer_size) 
{
    switch (event.type) 
    {
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
 */
void log_event(LogEvent event) 
{
    char log_message[512];
    format_log_event(event, log_message, sizeof(log_message));
    write_logEvent(log_message); // Ghi log-event vào FIFO
}

/**
 * @brief Logs a connection opened event.
 */
void Log_OpenConnection(int sensorNodeID) 
{
    LogEvent event = { .type = CONNECTION_OPENED, .sensorNodeID = sensorNodeID };
    log_event(event);
}

/**
 * @brief Logs a connection closed event.
 */
void Log_CloseConnection(int sensorNodeID) 
{
    LogEvent event = { .type = CONNECTION_CLOSED, .sensorNodeID = sensorNodeID };
    log_event(event);
}

/**
 * @brief Logs a sensor node reporting too cold event.
 */
void Log_ReportColdSensor(int sensorNodeID, double avg_temp) 
{
    LogEvent event = { .type = TOO_COLD, .sensorNodeID = sensorNodeID, .temperatureValue = avg_temp };
    log_event(event);
}


void Log_ReportHotSensor(int sensorNodeID, double avg_temp) 
{
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
void Log_InvalidIDSensor(int nodeID) 
{
    LogEvent event = { .type = INVALID_SENSOR_NODE_ID, .sensorNodeID = nodeID };
    log_event(event);
}

/**
 * @brief Logs a SQL connection established event.
 * 
 * This function logs a SQL connection established event to the FIFO log system.
 */
void Log_SqlEstablishedConnection() 
{
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
void Log_SqlTableCreated(const char *table_name) 
{
    LogEvent event = { .type = TABLE_CREATED };
    snprintf(event.extraInfo, sizeof(event.extraInfo), "%s", table_name);
    log_event(event);
}

/**
 * @brief Logs a SQL connection lost event.
 * 
 * This function logs a SQL connection lost event to the FIFO log system.
 */
void Log_SqlLostConnection() 
{
    LogEvent event = { .type = SQL_CONNECTION_LOST };
    log_event(event);
}

/**
 * @brief Logs a SQL connection failed event.
 * 
 * This function logs a SQL connection failed event to the FIFO log system.
 */
void Log_SqlFailedConnection() 
{
    LogEvent event = { .type = SQL_CONNECTION_FAILED };
    log_event(event);
}