#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#include <errno.h>

#include "lib/tcp_server.h"

#define BUFFER_SIZE 50

/**
 * Function to handle the client request
 * 
 * \todo This will be added as a callback in the server, to implement various capabilities
 * 
 * \param pclient_fd Client file descriptor
 */
void *handle_tcp_request(void* pclient_fd, server_t *server)
{
    int client_fd = *(int *)pclient_fd;
    free(pclient_fd);

    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    
    pthread_mutex_lock(&server->ctx_lock);
    void *handler_ctx = NULL;
    handler_ctx = calloc(1, sizeof(server->cb_ctx));
    memcpy(handler_ctx, server->cb_ctx, sizeof(server->cb_ctx));

    pthread_mutex_unlock(&server->ctx_lock);

    // TODO fix buffer errors and issues with ending the stream.
    // TODO add timeouts
    while((bytes_read = read(client_fd, buffer, BUFFER_SIZE)) > 0) {
        fprintf(stdout, "Read : %zu\n", bytes_read);

        if(bytes_read == 0) {
            break;
        }

        callbackstatus status = server->cb_fn(client_fd, (uint8_t *)buffer, bytes_read, handler_ctx);
        bytes_read = 0;
        memset(buffer, 0, BUFFER_SIZE);

        // if(!keep_processing) break;
        if (status == CONN_END || status == CONN_ERROR) {
            fprintf(stdout, "Terminating connection\n");
            break;
        } else if (status == CONN_TERMINATE) {
            fprintf(stdout, "Connection terminated\n");
        } else {
            fprintf(stdout, "Guess I am going to conitnue\n");
        }
    }

    // buffer[msg_size-1] = 0;
    server->cb_fn(client_fd, NULL, 0, handler_ctx);

    server->destroy_fn(handler_ctx);
    handler_ctx = NULL;

    close(client_fd);
    client_fd = -1;
    return NULL;
}

/**
 * Thread function that will listen for new connections 
 * in the connection queue.
 * 
 * \param args server instance
 */
void* thread_function(void *args)
{
    server_t *server = (server_t *) args;
    while(1) {
        int client_fd;
        pthread_mutex_lock(&(server->mutex));
        if((client_fd = deque(server->queue)) < 0) {
            pthread_cond_wait(&(server->condition_var), &(server->mutex));
        }
        pthread_mutex_unlock(&(server->mutex));

        if (client_fd > 0) {
            fprintf(stdout, "Handling %d\n", client_fd);
            int *pclient_fd = malloc(sizeof(int));
            *pclient_fd = client_fd;
            handle_tcp_request(pclient_fd, server);
        }
        fprintf(stdout, "Fetching connections in queue\n");
    }
}

/**
 * Initialize the server thread pool
 * 
 * \param server server instance
 */
void initialize_thread_pool(server_t *server)
{
    fprintf(stdout, "Creating %d threads\n", THREAD_POOL_SIZE);
    int i = 0;
    for (i = 0; i < THREAD_POOL_SIZE; i++) {
        pthread_create(&(server->thread_pool[i]), NULL, thread_function, server);
    }

    server->mutex = (pthread_mutex_t) PTHREAD_MUTEX_INITIALIZER;
    server->condition_var = (pthread_cond_t) PTHREAD_COND_INITIALIZER;
}

// server_t *create_server(int port)
server_t 
*create_server(int port, callback cb_fn, 
                destroy_ctx destroy_fn, void *cb_ctx)
{
    server_t *server;
    server = malloc(sizeof(server_t));
    server->queue = create_connection_queue();

    if (!server->queue) {
        free(server);
        return NULL;
    }

    server->socket_fd = 0;
    server->port = port;

    server->cb_fn = cb_fn;
    server->destroy_fn = destroy_fn;
    server->cb_ctx = cb_ctx;

    return server;
}

int start_server(server_t *server)
{
    struct sockaddr_in address, client_address;
    int address_len = sizeof(address);
    int address_size;

    if(!server) {
        return -1;
    }

    // Start the thread pool
    initialize_thread_pool(server);

    server->socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server->socket_fd < 0) {
        fprintf(stdout, "Unable to create socket\n");
        return -2;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(server->port);

    int socket_option = 1;
    if(setsockopt(server->socket_fd, SOL_SOCKET, SO_REUSEADDR, &socket_option, sizeof(socket_option)) < 0) {
        fprintf(stdout, "Uable to set socket option : %s\n", strerror(errno));
        return -3;
    }

    // bind the socket
    if (bind(server->socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        fprintf(stdout, "Bind error\n");
        free_server(server);
        exit(-1);
    }

    if(listen(server->socket_fd, 10) < 0) {
        fprintf(stderr, "Unable to bind address to server\n");
        free_server(server);
        exit(-1);
    }

    int client_fd;

    // Start accepting connection
    while(1) {
        fprintf(stdout, "Waiting for connections....\n");
        address_size = sizeof(address_len);
        client_fd = accept(server->socket_fd, (struct sockaddr *)&client_address, (socklen_t *)&address_size);
        if (client_fd < 0) {
            fprintf(stdout, "Failed to accept connection\n");
            free_server(server);
            exit(-1);
        }

        pthread_mutex_lock(&(server->mutex));
        enqueue(server->queue, client_fd);
        pthread_cond_signal(&(server->condition_var));
        pthread_mutex_unlock(&(server->mutex));
    }

    return 0;
}

void free_server(server_t *server)
{
    if(!server) {
        return;
    }

    close(server->socket_fd);
    server->socket_fd = -1;

    free_connection_queue(server->queue);
    server->queue = NULL;

    free(server);
}
