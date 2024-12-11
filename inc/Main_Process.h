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
#include "Queue_Share.h"
#include "Sql_DB.h"


extern int port_no;

void createFifo();
void *Thread_ConnectionManger(void *arg);
void *Thread_DataManager(void *arg);
void *Thread_StorageManager(void *arg);
void mainProcess();

#endif