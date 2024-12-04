#include "../inc/Log_Process.h"
#include "../inc/Main_Process.h"


int main( int argc, char *argv[] ) 
{
    // tao fifo
    createFifo();
    // Tạo tiến trình con
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        logProcess();
    } else {
        // Code của tiến trình chính
        mainProcess();
        printf("Main process running (PID: %d)\n", getpid());
        printf("Log process started (PID: %d)\n", pid);
    }
    return 0;
}


