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

/**
 * @brief Structure to store sensor data
 * 
 * This structure is used to store sensor data.
 * It contains the data from the sensor node and the ID of the sensor node.
 */
typedef struct {
    char data[256];         // Array contains data sensor node 
    int sensorNodeID;       // ID of the sensor node
} SensorData;


/**
 * @brief Structure to store the shared queue
 * 
 * This structure is used to store the shared queue.
 * It contains the queue, the front and rear of the queue, the count of elements in the queue,
 * a mutex to protect the queue, and a condition variable to signal when there is new data.
 */
typedef struct {
    SensorData queue[MAX_QUEUE_SIZE];   // Queue to store sensor data 
    int front, rear, count;             // Front, rear, count of elements in the queue      
    pthread_mutex_t mutex;              // Mutex to protect the queue
    pthread_cond_t cond;                // Condition variable to signal when there is new data
} SharedQueue;

/**
 * @brief Structure to store the state of a sensor node
 * 
 * This structure is used to store the state of a sensor node.
 * It contains the ID of the sensor node, the running average of the temperature data,
 * and the number of samples taken.
 */
typedef struct {
    int sensorNodeID;                   // ID of the sensor node
    double runningAverage;              // Running average of the temperature data
    int sampleCount;                    // Number of samples taken
} SensorNodeState;

extern SharedQueue shared_queue; 
extern SensorNodeState sensorStates[MAX_NODES];
/**
 * @brief Initializes the shared queue.
 * 
 * @param q A pointer to the shared queue.
 * 
 * This function initializes the shared queue.
 * It sets the front, rear, and count of elements in the queue to 0.
 * It initializes the mutex and condition variable.
 */
void init_queue(SharedQueue *q);
/**
 * @brief Enqueues a sensor data into the shared queue.
 * 
 * @param q A pointer to the shared queue.
 * @param data A pointer to the sensor data to enqueue.
 * 
 * This function enqueues a sensor data into the shared queue.
 * It locks the mutex, enqueues the data, increments the count of elements in the queue,
 * signals the condition variable, and unlocks the mutex.
 */
void enqueue(SharedQueue *q, SensorData *data);
/**
 * @brief Dequeues a sensor data from the shared queue.
 * 
 * @param q A pointer to the shared queue.
 * @param data A pointer to store the dequeued sensor data.
 * 
 * This function dequeues a sensor data from the shared queue.
 * It locks the mutex, dequeues the data, decrements the count of elements in the queue,
 * signals the condition variable, and unlocks the mutex.
 */
int dequeue(SharedQueue *q, SensorData *data);


#endif

