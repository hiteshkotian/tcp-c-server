/** \mainpage TCP Server
 * 
 * \section introduction Introduction
 * This is the introduction
 * 
 * \section starting_server Starting the Server
 * Starting the server
 * 
 * \section testing Testing the server
 * Testing section
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