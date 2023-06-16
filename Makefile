CC=gcc
THREAD_POOL_SIZE=5

server: tcp_server
	gcc -Wall -o server.o -c ./cmd/server_cli.c
	gcc -Wall -o server server.o tcp_server.o connection.o \
		-D THREAD_POOL_SIZE=$(THREAD_POOL_SIZE)

tcp_server: connection
	gcc -Wall -o tcp_server.o -c ./lib/tcp_server.c

connection: 
	gcc -Wall -o connection.o -c ./lib/connection_queue.c

doc:
	doxygen doc-config

# TODO UPDATE ME
test: server
	$(CC) $(CFLAGS) -c -o test/queue_test.o test/queue_test.c
	$(CC) $(CFLAGS) -o test/queue_test test/queue_test.o lib/connection_queue.o -lcmocka
	./test/queue_test

clean:
	rm *.o
	rm server
	rm -rf docs
	rm test/queue_test.o
	rm test/queue_test