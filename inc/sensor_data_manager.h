#ifndef __SENSOR_DATA_MANAGER_H__
#define __SENSOR_DATA_MANAGER_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h> 
#include <string.h>
#include <pthread.h>

#include "log_manager.h"
#include "socket_connection.h"
#include "queue_share.h"
#include "sql_db.h"


extern int port_no; // Port number to connect to

/**
 * @brief Creates a FIFO log system.
 */
void createFifo();

/**
 * @brief Initializes the FIFO log system.
 */
void *Thread_ConnectionManger(void *arg);
/*
    * @brief Function to handle the data manager thread
    * 
    * This function handles the data manager thread.
    * 
    * @param arg The argument to the thread.
    * 
    * @return NULL.
*/
void *Thread_DataManager(void *arg);
/*
    * @brief Function to handle the storage manager thread
    * 
    * This function handles the storage manager thread.
    * 
    * @param arg The argument to the thread.
    * 
    * @return NULL.
*/
void *Thread_StorageManager(void *arg);
/*
    * @brief Function to initialize the system threads
    * 
    * This function initializes the system threads.
*/
void system_threads_init();

#endif