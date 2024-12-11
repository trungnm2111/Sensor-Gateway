#include "../inc/Queue_Share.h"

SharedQueue shared_queue; 
SensorNodeState sensorStates[MAX_NODES];
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
    if (q->count == MAX_QUEUE_SIZE) {
        printf("Queue is full! Dropping data.\n");
        pthread_mutex_unlock(&q->mutex);
        return;
    }

    q->queue[q->rear] = *data;
    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    q->count++;

    pthread_cond_signal(&q->cond);

    pthread_mutex_unlock(&q->mutex);
}

// Lấy phần tử từ hàng đợi
int dequeue(SharedQueue *q, SensorData *data) 
{
    pthread_mutex_lock(&q->mutex);
    while (q->count == 0) {
        pthread_cond_wait(&q->cond, &q->mutex);
    }
    *data = q->queue[q->front];
    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    q->count--;

    pthread_mutex_unlock(&q->mutex);
    return 0;
}