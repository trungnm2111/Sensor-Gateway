#include "../inc/Log_Process.h"

/**
 * @brief Log process
 * 
 */
void logProcess()
{
    // Mở FIFO để đọc
    int fifo_fd = open(FIFO_NAME, O_RDONLY);
    if (fifo_fd < 0) {
        perror("Unable to open FIFO for reading");
        exit(EXIT_FAILURE);
    }
    
    // mở file log để ghi
    FILE *log_file = fopen("gateway.log", "a"); 
    if (!log_file) {
        perror("Unable to open log file");
        close(fifo_fd);
        exit(EXIT_FAILURE);
    }
    int sequence_number = 0;
    char buf[256];
    char timestamp[64];
    char *event;
    while(1)
    {
        ssize_t byte_read = read(fifo_fd, buf, sizeof(buf) - 1);
        if (byte_read > 0)
        {
            buf[byte_read] = '\0';          // Kết thúc chuỗi
            event = strtok(buf, "\n");
            while (event != NULL)
            {
                get_timeStamp(timestamp, sizeof(timestamp));
                fprintf(log_file, "<%d> <%s>: %s\n", sequence_number ,timestamp, event);
                fflush(log_file);           // Đảm bảo dữ liệu được ghi ngay
                event = strtok(NULL, "\n");
                sequence_number ++;
            }
        }
    }
    fclose(log_file);
    close(fifo_fd);
}

/**
 * @brief Lấy thời gian hiện tại
 * 
 * @param buffer Chuỗi lưu thời gian
 * @param size Kích thước buffer
 */
void get_timeStamp(char *buffer, size_t size) 
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}