/**
 * connection_queue.h
 * 
 * Connection queue implementation
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "./connection_queue.h"

/**
 * Create a new connection object
 * 
 * \param connection file descriptor
 */
connection_t *connection_new(int fd)
{
    connection_t *connection;

    connection = malloc(sizeof(connection_t));
    connection->connection_fd = fd;
    connection->next = NULL;

    return connection;
}

/**
 * Free up the connection instance
 * 
 * \param connection instance
 */
void connection_free(connection_t *connection)
{
    if (connection)
        free(connection);
}

/**
 * Returns the top of the queue and removes it from the list
 * 
 * \param[in] Connection queue
 * 
 * \return Connection instance on the top of the queue
 */
connection_t *next(connection_queue_t *queue)
{
    // If invalid queue instance is provided or the 
    // head is invalid return
    if(!queue || !queue->connection_head)
    {
        return NULL;
    }

    connection_t *head = queue->connection_head;
    queue->connection_head = head->next;
    queue->connection_count -= 1;

    return head;
}

int queue_empty(connection_queue_t *queue)
{
    if (queue == NULL) {
        return 1;
    }

    return queue->connection_count == 0;
}

/**
 * Inserts a new element in the queue.
 * 
 * \param queue Connection queue instance
 * \param connection Connection node to add
 * 
 * \return 0 on success or the appropriate error code.
 */
int add_last(connection_queue_t *queue, connection_t *connection)
{
    fprintf(stdout, "Adding %d to the queue\n", connection->connection_fd);

    if(!queue) {
        return INVALID_QUEUE;
    } else if (!connection) {
        return INVALID_CONNECTION;
    }

    // check if the queue is empty
    if(queue_empty(queue)) {
        queue->connection_head = queue->connection_tail = connection;
    } else {
        queue->connection_tail->next = connection;
        queue->connection_tail = connection;
    }

    queue->connection_count++;
    return 0;
}

/**
 * Prints the elements of the connection queue.
 * 
 * \param queue connection queue instance
 */
void print_queue(connection_queue_t *queue)
{
    connection_t *connection = queue->connection_head;
    while(connection != NULL) {
        fprintf(stdout, "%d, ", connection->connection_fd);
        connection = connection->next;
    }

    fprintf(stdout, "\n");
}

/******** public functions ***************/

connection_queue_t *create_connection_queue()
{
    connection_queue_t *connection_queue;

    // Allocate memory
    connection_queue = malloc(sizeof(connection_queue_t));

    // Initialize the variables
    connection_queue->connection_count = 0;
    connection_queue->connection_head = NULL;
    connection_queue->connection_tail = NULL;

    return connection_queue;
}

void free_connection_queue(connection_queue_t *connection_queue)
{
    // Iterate through the existing connections and close the open ones
    if (!connection_queue) {
        fprintf(stdout, "Invalid connection queue provided\n");
        return;
    }

    connection_t *connection = next(connection_queue);

    while((connection = next(connection_queue)) != NULL) {
        close(connection->connection_fd);
        connection_free(connection);
    }

    free(connection_queue);
}

int enqueue(connection_queue_t *connection_queue, int connection_fd)
{
    if(!connection_queue) {
        return INVALID_QUEUE;
    } else if (connection_fd < 0) {
        return INVALID_FD;
    }

    connection_t *connection;
    // Create a new connection instance
    connection = connection_new(connection_fd);
    if (!connection) {
        fprintf(stdout, "Unable to create new connection\n");
        return CONNECTION_CREATION_ERROR;
    }

    add_last(connection_queue, connection);
    print_queue(connection_queue);
    return 0;
}

int deque(connection_queue_t *connection_queue)
{
    if(!connection_queue) {
        return INVALID_QUEUE;
    }

    connection_t *connection;
    int ret = -1;

    connection = next(connection_queue);

    if (connection) {
        ret = connection->connection_fd;

        // free(connection);
        print_queue(connection_queue);

    }
    return ret;
}