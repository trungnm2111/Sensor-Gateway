# Safety fork()

CC := gcc
CFLAGS := -pthread -lmysqlclient

all:
	touch gateway.log
	$(CC) -c ./src/Log_Process.c  $(CFLAGS)
	$(CC) -c ./src/Queue_Share.c  $(CFLAGS)
	$(CC) -c ./src/Socket_Connection.c  $(CFLAGS)
	$(CC) -c ./src/Sql_DB.c  $(CFLAGS)
	$(CC) -c ./src/Main_Process.c  $(CFLAGS)
	$(CC) -g -o process ./src/main.c Main_Process.o Log_Process.o Socket_Connection.o Queue_Share.o Sql_DB.o $(CFLAGS)

clean:
	rm process
	rm gateway.log 
	rm *.o
	rm logFifo

