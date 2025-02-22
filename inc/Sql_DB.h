#ifndef __SQL_DB_H__
#define __SQL_DB_H__

#include <mysql/mysql.h>
#include "queue_share.h"
#include "log_manager.h"

#define MAX_RETRIES 3 // the maximum number of retries
#define RETRY_DELAY 5 // The delay between retries

/**
 *  @brief Function to connect to the SQL database
 */
MYSQL* SQL_Connect();
/**
 *  @brief Function to disconnect from the SQL database
 * 
 *  @param conn The connection to the database
 *  @brief database connection
 */
 *  @return 0 if successful, -1 if not
 */
bool SQL_Store(MYSQL *conn, SensorData *data);

/**
 *  @brief Function to handle the database storage
 * 
 *  @param sensor_data The sensor data to store
 */
int HandleDatabaseStorage(SensorData *sensor_data);

#endif