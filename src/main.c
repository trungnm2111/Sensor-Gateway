#include "../inc/log_process.h"
#include "../inc/sensor_data_manager.h"

int main( int argc, char *argv[] ) 
{

    // tao fifo
    createFifo();
    // Tạo tiến trình con

    // create FIFO
    createFifo();
    // Create a child process

    pid_t pid = fork();
    if (pid < 0)
        handle_error("Fork failed"); 
    if (argc < 2) {
        handle_error("No port provided\ncommand: ./server <port number>\n");
    } else
        port_no = atoi(argv[1]);
    if (pid == 0) {
        logProcess();
    } else {
        mainProcess();
        printf("Main process running (PID: %d)\n", getpid());
        printf("Log process started (PID: %d)\n", pid);
    }
    return 0;
}

