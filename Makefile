# Safety fork()

CC := gcc
CFLAGS := -pthread -lmysqlclient

all:
	touch gateway.log

	$(CC) -c ./src/log_manager.c  $(CFLAGS)
	$(CC) -c ./src/queue_share.c  $(CFLAGS)
	$(CC) -c ./src/socket_connection.c  $(CFLAGS)
	$(CC) -c ./src/sql_db.c  $(CFLAGS)
	$(CC) -c ./src/sensor_data_manager.c  $(CFLAGS)
	$(CC) -g -o process ./src/main.c sensor_data_manager.o log_manager.o socket_connection.o queue_share.o sql_db.o $(CFLAGS)

clean:
	rm process
	rm gateway.log 
	rm *.o
	rm logFifo

