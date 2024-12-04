# Safety fork()

CC := gcc
CFLAGS := -pthread

all:
	touch gateway.log
	$(CC) -c ./src/Log_Process.c  $(CFLAGS)
	$(CC) -c ./src/Main_Process.c  $(CFLAGS)
	$(CC) -o process ./src/main.c Main_Process.o Log_Process.o $(CFLAGS)

clean:
	rm process
	rm gateway.log 
	rm logFifo
	rm Log_Process.o

