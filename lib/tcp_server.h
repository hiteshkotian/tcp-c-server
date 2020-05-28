/**
 * \file tcp_server.h
 * 
 * Contains implementation for TCP server
 */
#ifndef SERVER_H_
#define SERVER_H_

#include <pthread.h>

#include "connection_queue.h"

/// Thread pool size. Defaults to 10 but can be
/// configured at compile time
#ifndef THREAD_POOL_SIZE
#define THREAD_POOL_SIZE 10
#endif

/**
 * Server structure
 */
typedef struct server_t_
{
    /// Port the server will listen for requests
    int port;

    /// Connection queue
    connection_queue_t *queue;

    /// Server socket fd
    int socket_fd;

    /// Thread pool instance
    pthread_t thread_pool[THREAD_POOL_SIZE];

    pthread_mutex_t mutex;
    pthread_cond_t condition_var;
} server_t;

/**
 * Create a new instance of the server
 * 
 * \param port Port the server will accept requests
 * 
 * \return server instance on success otherwise NULL
 */
server_t *create_server(int port);

/**
 * Deallocates the resources allocated to the server
 * 
 * \param server server instnace
 */
void free_server(server_t *server);

/**
 * Starts the tcp server
 * 
 * \param server server instance
 */
int start_server(server_t *server);

#endif