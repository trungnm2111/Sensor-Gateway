#include "../inc/Sql_DB.h"

// Hàm kết nối đến SQL
MYSQL* SQL_Connect() {
    MYSQL *conn = mysql_init(NULL);
    if (!conn) {
        fprintf(stderr, "MySQL initialization failed\n");
        return NULL;
    }
    if (!mysql_real_connect(conn, "localhost", "root", "1", "sensor_db", 0, NULL, 0)) {
        fprintf(stderr, "MySQL connection error: %s\n", mysql_error(conn));
        mysql_close(conn);
        return NULL;
    }
    return conn;
}

// Hàm lưu trữ dữ liệu vào SQL
bool SQL_Store(MYSQL *conn, SensorData *data) {
    char query[1024];
    snprintf(query, sizeof(query),
         "INSERT INTO sensor_data (sensor_id, value, timestamp) VALUES (%d, '%.*s', NOW())",
         data->sensorNodeID, (int) (sizeof(query) - 100), data->data);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Failed to insert data: %s\n", mysql_error(conn));
        return false;
    }
    return true;
}

int HandleDatabaseStorage(SensorData *sensor_data)
{
    int retry_count = 0;
    MYSQL *db_conn = NULL;
    while (1) {
        dequeue(&shared_queue, sensor_data);
        if (db_conn == NULL) {
            db_conn = SQL_Connect();
            if (db_conn == NULL) {
                retry_count++;
                if (retry_count >= MAX_RETRIES) {
                    fprintf(stderr, "Failed to connect to database after %d attempts. Closing gateway.\n", MAX_RETRIES);
                    Log_SqlFailedConnection();
                    exit(EXIT_FAILURE); 
                }
                sleep(RETRY_DELAY);
                continue; 
            }
        }
        // Lưu trữ dữ liệu vào cơ sở dữ liệu
        Log_SqlEstablishedConnection();
        if (!SQL_Store(db_conn, sensor_data)) {
            fprintf(stderr, "Failed to store data. Retrying...\n");
            Log_SqlFailedConnection();
            retry_count++;
            if (retry_count >= MAX_RETRIES) {
                fprintf(stderr, "Failed to store data after %d attempts. Closing gateway.\n", MAX_RETRIES);
                Log_SqlLostConnection();
                mysql_close(db_conn);
                return -1;
            }
            sleep(RETRY_DELAY);
        } else {
            retry_count = 0;
            Log_SqlTableCreated();
        }
    }

    if (db_conn) {
        mysql_close(db_conn); 
    }
    return 0;
}