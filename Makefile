CC=gcc
THREAD_POOL_SIZE=5
CFLAGS=-Wall -I.

server: tcp_server
	$(CC) $(CFLAGS) -o server.o -c ./cmd/server_cli.c
	$(CC) $(CFLAGS) -o server server.o tcp_server.o connection.o \
		-D THREAD_POOL_SIZE=$(THREAD_POOL_SIZE)

queue_test: connection
	$(CC) $(CFLAGS) -o queue_test.o -c ./test/queue_test.c
	gcc -o queue_test queue_test.o connection.o

tcp_server: connection
	$(CC) $(CFLAGS) -o tcp_server.o -c ./lib/tcp_server.c -D THREAD_POOL_SIZE=$(THREAD_POOL_SIZE)

connection: 
	$(CC) $(CFLAGS) -o connection.o -c ./lib/connection_queue.c

doc:
	doxygen doc-config

clean:
	rm *.o
	rm server
	rm -rf docs
