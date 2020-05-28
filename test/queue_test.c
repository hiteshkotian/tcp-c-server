#include <stdio.h>

#include "../lib/connection_queue.h"

int main()
{
    connection_queue_t *queue;

    queue = create_connection_queue();
    fprintf(stdout, "Connection queue created\n");

    enqueue(queue, 10);
    enqueue(queue, 20);
    enqueue(queue, 30);

    int val;

    val = deque(queue);
    fprintf(stdout, "DEQUEUED : %d\n", val);
    val = deque(queue);
    fprintf(stdout, "DEQUEUED : %d\n", val);
    val = deque(queue);
    fprintf(stdout, "DEQUEUED : %d\n", val);
    val = deque(queue);
    fprintf(stdout, "DEQUEUED : %d\n", val);
    
    free_connection_queue(queue);
    fprintf(stdout, "Connection queue freed\n");
}