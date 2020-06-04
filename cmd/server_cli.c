/** \mainpage TCP Server
 * 
 * \section introduction Introduction
 * TCP server is a basic concurrent TCP server written in C
 * 
 * \section building Building the server
 * The server can be built using the make file.
 * To build the makefile run the following command
 * \code
 * 
 * make all 
 * 
 * \endcode
 * 
 * Alternatively the following command can be used:
 * \code
 * 
 * make
 * 
 * \endcode
 *
 * The build command can also be supplied with the <b>THREAD_POOL_SIZE</b> parameter
 * 
 * \code
 * 
 * make THREAD_POOL_SIZE=20    # Compiles the code with 20 threads
 * 
 * \endcode
 * 
 * \section code_org Code Structure
 * The code comprises of the following directories:
 * - lib : Consists of all the library files
 * - cmd : Consists of the server code
 * 
 * \section testing Testing the server
 * The current implementation of the server only is an echo server. 
 * 
 * To test this telnet can be used once the server is running
*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>

#include "lib/tcp_server.h"

#define PORT 8989

/** Server instance **/
static server_t *server;

typedef enum server_state_t {
  new = 0,
  in_progress = 1
} server_state;

typedef struct echo_server_ctx_t
{
  uint8_t *data;
  size_t received_size;
  time_t start_time;
  time_t end_time;

  server_state state;
} echo_server_ctx;


/**
 * Create a new echo server context
 */
echo_server_ctx 
*create_echo_server_ctx()
{
  echo_server_ctx *ctx;
  ctx = malloc(sizeof(echo_server_ctx));
  if(ctx == NULL) {
    fprintf(stdout, "Unable to create echo context\n");
    return NULL;
  }

  // We will initialize the buffer only when
  // handling the request  
  ctx->data = NULL;
  ctx->received_size = 0;
  ctx->start_time = 0;
  ctx->end_time = 0;

  ctx->state = new;
  return ctx;
}

/**
 * Free up the space allocated to the 
 * echo server context
 */
int free_echo_server_ctx(void *ctx)
{
  int status = 0;
  if(ctx == NULL) {
    return status;
  }

  echo_server_ctx *server_ctx = (echo_server_ctx *) ctx;
  if(server_ctx->data != NULL) {
    free(server_ctx->data);
    server_ctx->data = NULL;
  }

  free(server_ctx);
  server_ctx = NULL;
  return status;
}

/**
 * Signal handler
 */
void handle_signal(int sig)
{
    // Close the server
    fprintf(stdout, "Stopping server\n");

    free_server(server);
    exit(0);
}

/**
 * Registers interrupt signal to gracefully shutdown
 * the server
 */
void register_signal()
{
    signal(SIGINT, handle_signal);
    signal(SIGHUP, handle_signal);
    signal(SIGQUIT, handle_signal);
}

/**
 * Echo server callback to handle requests
 */
int echo_server_callback(int fd, uint8_t *data, size_t len, void *arg)
{
  fprintf(stdout, "Handling in the echo server\n");
  int status = 0;

  echo_server_ctx *ctx = (echo_server_ctx *)arg;
  fprintf(stdout, "The state is : %d\n", ctx->state);
  switch(ctx->state) {
    case new:
      ctx->data = malloc(4096);
      ctx->start_time = time(NULL);
      ctx->state = in_progress;
    case in_progress:
      if(data != NULL) {
        int available = 4096 - ctx->received_size;
        if(len > available) {
          status = -2;
        } else {
          memcpy((ctx->data + ctx->received_size), data, len);
          fprintf(stdout, "The buffer is : %s\n", ctx->data);
          ctx->received_size += len;
        }
      } else {
        fprintf(stdout, "This is what we have : %s\n", (uint8_t *)ctx->data);
        sleep(1);
        ctx->end_time = time(NULL);
        // Cleanup
        write(fd, ctx->data, ctx->received_size);

        time_t tl = ctx->end_time - ctx->start_time;
        fprintf(stdout, "Time : (%ld - %ld) = %ld ms", ctx->end_time, ctx->start_time, tl);
        // close(fd);
      }
  }
  return status; 
}

int main()
{
    // Create instance of server
    echo_server_ctx *ctx = NULL;
    ctx = create_echo_server_ctx();

    if(ctx == NULL) {
      perror("Unable to initialize the server context\n");
    }

    server = create_server(8989, &echo_server_callback, &free_echo_server_ctx, ctx);

    // server = create_server(PORT);
    if (!server) {
        perror("Unable to create server");
    }

    register_signal();

    // Initialize the socket
    start_server(server);
    free_server(server);
}
