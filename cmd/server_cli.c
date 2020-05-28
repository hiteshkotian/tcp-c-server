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

#include "../lib/tcp_server.h"

#define PORT 8989

/** Server instance **/
server_t *server;

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

int main()
{
    // Create instance of server
    server = create_server(8989);

    // server = create_server(PORT);
    if (!server) {
        perror("Unable to create server");
    }

    register_signal();

    // Initialize the socket
    start_server(server);
    free_server(server);
}