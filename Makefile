# Safety fork()

CC := gcc
CFLAGS := -pthread

all:
	touch gateway.log
	$(CC) -c ./src/Log_Process.c  $(CFLAGS)
	$(CC) -c ./src/Socket_Connection.c  $(CFLAGS)
	$(CC) -c ./src/Main_Process.c  $(CFLAGS)
	$(CC) -o process ./src/main.c Main_Process.o Log_Process.o Socket_Connection.o $(CFLAGS)

clean:
	rm process
	rm gateway.log 
	rm *.o
	rm logFifo

