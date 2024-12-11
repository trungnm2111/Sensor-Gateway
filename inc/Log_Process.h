#ifndef __LOG_PROCESS_H__
#define __LOG_PROCESS_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h> 
#include <string.h>
#include <pthread.h>
#include <time.h>

// #include "./Main_Process.h"

#define FIFO_NAME "logFifo"

typedef enum {
    CONNECTION_OPENED,
    CONNECTION_CLOSED,
    TOO_COLD,
    TOO_HOT,
    INVALID_SENSOR_NODE_ID,
    SQL_CONNECTION_ESTABLISHED,
    TABLE_CREATED,
    SQL_CONNECTION_LOST,
    SQL_CONNECTION_FAILED
} LogEventType;

typedef struct {
    LogEventType type;
    int sensorNodeID;        // ID của sensor node (nếu có)
    double temperatureValue; // Giá trị nhiệt độ trung bình (nếu có)
    char extraInfo[256];     // Thông tin bổ sung (nếu có)
} LogEvent;

//Cau truc du lieu chia se
typedef struct {
    pthread_mutex_t mutex;
} SharedResource;

SharedResource resource;

void logProcess();
void get_timeStamp(char *buffer, size_t size);
void write_logEvent(const char *log_event);
void log_event(LogEvent event);
void format_log_event(LogEvent event, char *buffer, size_t buffer_size);
void Log_OpenConnection(int sensorNodeID);
void Log_CloseConnection(int sensorNodeID);
void Log_ReportColdSensor(int sensorNodeID, double avg_temp);
void Log_ReportHotSensor(int sensorNodeID, double avg_temp);
void Log_InvalidIDSensor(int nodeID);
void Log_SqlEstablishedConnection();
void Log_SqlTableCreated();
void Log_SqlLostConnection();
void Log_SqlFailedConnection();

#endif