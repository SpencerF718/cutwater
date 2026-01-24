#include "buffer.h"
#include <assert.h>
#include <stdio.h>

void test_buffer_init(void) {
    printf("TESTING: buffer_init\n");

    EditorBuffer eb;

    int buffer_init_result = buffer_init(&eb, 10);
    assert(buffer_init_result == 0);

    assert(eb.capacity == 10);
    assert(eb.gap_start == 0);
    assert(eb.gap_end == 10);
    assert(eb.data != NULL);

    buffer_free(&eb);
    printf("PASSED: buffer_init\n");
}

void test_buffer_grow(void) {
    printf("TESTING: buffer_grow\n");

    EditorBuffer eb;
    buffer_init(&eb, 2);
    buffer_insert(&eb, 'A');
    buffer_insert(&eb, 'B');
    buffer_insert(&eb, 'C');

    assert(eb.capacity == 4);
    assert(eb.gap_start == 3);
    assert(eb.gap_end == 4);
    assert(eb.data != NULL);

    buffer_free(&eb);
    printf("PASSED: buffer_grow\n");
}

void test_buffer_insert(void) {
    printf("TESTING: buffer_insert\n");

    EditorBuffer eb;
    buffer_init(&eb, 10);

    int buffer_insert_result = buffer_insert(&eb, 'A');
    assert(buffer_insert_result == 0);

    assert(eb.capacity == 10);
    assert(eb.gap_start == 1);
    assert(eb.gap_end == 10);
    assert(eb.data != NULL);

    buffer_free(&eb);
    printf("PASSED: buffer_insert\n");
}


void test_buffer_delete(void) {
    printf("TESTING: buffer_delete\n");

    EditorBuffer eb;
    buffer_init(&eb, 10);
    buffer_insert(&eb, 'A');

    int buffer_delete_result = buffer_delete(&eb);
    assert(buffer_delete_result == 0);

    assert(eb.capacity == 10);
    assert(eb.gap_start == 0);
    assert(eb.gap_end == 10);
    assert(eb.data != NULL);

    buffer_free(&eb);
    printf("PASSED: buffer_delete\n");
}

void test_buffer_move_left(void) {
    printf("TESTING: buffer_move_left\n");

    EditorBuffer eb;
    buffer_init(&eb, 10);
    buffer_insert(&eb, 'A');
    buffer_insert(&eb, 'B');

    int buffer_move_left_result = buffer_move_left(&eb);
    assert(buffer_move_left_result == 0);

    assert(eb.capacity == 10);
    assert(eb.gap_start == 1);
    assert(eb.gap_end == 9);
    assert(eb.data[0] == 'A');
    assert(eb.data[9] == 'B');
    assert(eb.data != NULL);

    buffer_free(&eb);
    printf("PASSED: buffer_move_left\n");
}

void test_buffer_move_right(void) {
    printf("TESTING: buffer_move_right\n");

    EditorBuffer eb;
    buffer_init(&eb, 10);
    buffer_insert(&eb, 'A');
    buffer_insert(&eb, 'B');
    buffer_move_left(&eb);

    int buffer_move_right_result = buffer_move_right(&eb);
    assert(buffer_move_right_result == 0);

    assert(eb.capacity == 10);
    assert(eb.gap_start == 2);
    assert(eb.gap_end == 10);
    assert(eb.data[0] == 'A');
    assert(eb.data[1] == 'B');
    assert(eb.data != NULL);

    buffer_free(&eb);
    printf("PASSED: buffer_move_right\n");
}

int main(void) {
    test_buffer_init();
    test_buffer_grow();
    test_buffer_insert();
    test_buffer_delete();
    test_buffer_move_left();
    test_buffer_move_right();

    printf("--- ALL TESTS PASSED ---\n");

    return 0;
}
