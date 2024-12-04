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
#include "./Main_Process.h"

#define FIFO_NAME "logFifo"

void logProcess();
void get_timeStamp(char *buffer, size_t size);


#endif