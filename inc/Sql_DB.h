#ifndef __SQL_DB_H__
#define __SQL_DB_H__

#include <mysql/mysql.h>
#include "Queue_Share.h"
#include "Log_Process.h"

#define MAX_RETRIES 3 // Số lần thử lại trước khi thoát
#define RETRY_DELAY 5 // Thời gian chờ giữa các lần thử lại (giây)

MYSQL* SQL_Connect();
bool SQL_Store(MYSQL *conn, SensorData *data);
int HandleDatabaseStorage(SensorData *sensor_data);

#endif