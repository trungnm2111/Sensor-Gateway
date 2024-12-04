#ifndef __MAIN_PROCESS_H__
#define __MAIN_PROCESS_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h> 
#include <string.h>
#include <pthread.h>

#include "Log_Process.h"
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

void createFifo();
void *Thread_ConnectionManger(void *arg);
void *Thread_DataManager(void *arg);
void *Thread_StorageManager(void *arg);
void mainProcess();
void write_logEvent(const char *log_event);
void log_event(LogEvent event);
void format_log_event(LogEvent event, char *buffer, size_t buffer_size);
void connection_opened(int sensorNodeID);
void connection_closed(int sensorNodeID);
void Sensor_ColdReport(int sensorNodeID, double avg_temp);
void Sensor_HotReport(int sensorNodeID, double avg_temp);
void Sensor_IdInvalid(int nodeID);
void Sql_EstablishedConnection();
void Sql_CreateTable(const char *table_name);
void Sql_LostConnection();
void Sql_FailedConnection();


#endif