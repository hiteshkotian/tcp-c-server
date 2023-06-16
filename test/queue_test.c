#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdlib.h>

#include "../lib/connection_queue.h"

void connection_queue_test(void **state) {
    (void) state; // ignore
    connection_queue_t *queue;
    int enqeue_status = 0;
    int deqeue_status = 0;

    queue = create_connection_queue();
    assert_non_null(queue);

    enqeue_status = enqueue(queue, 10);
    assert_int_equal(enqeue_status, 0);

    enqeue_status = enqueue(queue, 20);
    assert_int_equal(enqeue_status, 0);

    enqeue_status = enqueue(queue, 30);
    assert_int_equal(enqeue_status, 0);

    deqeue_status = deque(queue);
    assert_int_equal(deqeue_status, 10);
    
    deqeue_status = deque(queue);
    assert_int_equal(deqeue_status, 20);

    deqeue_status = deque(queue);
    assert_int_equal(deqeue_status, 30);

    deqeue_status = deque(queue);
    assert_int_equal(deqeue_status, -1);
    
    free_connection_queue(queue);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(connection_queue_test),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}