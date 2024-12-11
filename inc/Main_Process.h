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
#include "Socket_Connection.h"

/*****************************************************New***********************************/

#define MAX_NODES 5
#define MAX_BUFFER_SIZE_SHARE_DATA 256
#define MAX_QUEUE_SIZE 100

typedef struct {
    char data[256]; // Gói dữ liệu từ sensor node
    int sensorNodeID; // ID của sensor node
} SensorData;

typedef struct {
    SensorData queue[MAX_QUEUE_SIZE];
    int front, rear, count; // Biến đếm số
    pthread_mutex_t mutex; // Để bảo vệ hàng đợi
    pthread_cond_t cond;   // Để thông báo khi có dữ liệu mới
} SharedQueue;

typedef struct {
    int sensorNodeID;
    double runningAverage;
    int sampleCount;
} SensorNodeState;

/******************************************************************************************/
extern int port_no;

void createFifo();
void *Thread_ConnectionManger(void *arg);
void *Thread_DataManager(void *arg);
void *Thread_StorageManager(void *arg);
void mainProcess();
void enqueue(SharedQueue *q, SensorData *data);
int dequeue(SharedQueue *q, SensorData *data);

#endif