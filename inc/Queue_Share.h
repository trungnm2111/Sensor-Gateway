#ifndef __QUEUE_SHARE_H__
#define __QUEUE_SHARE_H__

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_NODES 100
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

extern SharedQueue shared_queue; 
extern SensorNodeState sensorStates[MAX_NODES];

void init_queue(SharedQueue *q);
void enqueue(SharedQueue *q, SensorData *data);
int dequeue(SharedQueue *q, SensorData *data);


#endif

