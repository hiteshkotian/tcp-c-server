CC=gcc -g
THREAD_POOL_SIZE=5
CFLAGS=-Wall -I.
BIN = server
DEPS = lib/connection_queue.o lib/tcp_server.o cmd/server_cli.o

.PHONY: $(BIN) doc clean

default: all

all: $(BIN)

$(BIN): $(DEPS)
	$(CC) $(CFLAGS) -o $@ $^ 
	# -D THREAD_POOL_SIZE=$(THREAD_POOL_SIZE)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< -D THREAD_POOL_SIZE=$(THREAD_POOL_SIZE)

doc:
	doxygen doc-config

# TODO UPDATE ME
test: $(DEPS)
	$(CC) $(CFLAGS) -c -o test/queue_test.o test/queue_test.c
	$(CC) $(CFLAGS) -o test/queue_test test/queue_test.o lib/connection_queue.o -lcmocka
	./test/queue_test

clean:
	rm -rf $(DEPS) $(BIN) docs test/*.o queue_test || true
