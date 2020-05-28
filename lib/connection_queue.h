/**
 * \file connection_queue.h
 * 
 * Connection queue contains an implementation for
 * for queueing the incoming client connections.
 * 
 * \note The implementation is not thread safe, 
 * so the callers are expected to handle thread-safety
 */
#ifndef CONNECTION_QUEUE_H_
#define CONNECTION_QUEUE_H_

/// Invalid queue provided
#define INVALID_QUEUE 0x01
/// Invalid file descriptor provided
#define INVALID_FD 0x02
/// Connection error
#define CONNECTION_CREATION_ERROR 0x04
/// Invalid connection object provided
#define INVALID_CONNECTION 0x08

typedef struct connection_t_ connection_t;

/**
 * basic building block of the connection queue.
 */
struct connection_t_ {
    /// File descriptor of the client connection
    int connection_fd;
    /// Pointer to the next connection structure
    connection_t *next;
};

/**
 * Connection queue.
 */
typedef struct connection_queue_t_ {
    /// Head of the connection
    connection_t *connection_head;
    /// Tail of the connection
    connection_t *connection_tail;
    /// Number of connections
    size_t connection_count;
} connection_queue_t;

/**
 * Create a new connection queue
 * 
 * \return instance of the connection queue
 */
connection_queue_t *create_connection_queue();

/**
 * Free up the connection queue.
 * 
 * \param connection_queue Instance of the connection queue
 */
void free_connection_queue(connection_queue_t *connection_queue);

/**
 * Enqueues the connection
 * 
 * \param connection_queue Instance of the connection queue
 * \param connection_fd File descriptor of the connection to queue
 * 
 * \return 0 if the connection was queued successfully. Otherwise
 * see the list of error codes.
 */
int enqueue(connection_queue_t *connection_queue, int connection_fd);

/**
 * Get the connection from the top of the queue
 * 
 * \param connection_queue Connection queue instance
 * 
 * \return the file descriptor of the available connection else -1.
 */
int deque(connection_queue_t *connection_queue);

#endif