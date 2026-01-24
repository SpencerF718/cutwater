#include "buffer.h"
#include <assert.h>
#include <stdio.h>

#define BUFFER_SUCCESS 0
#define INITIAL_CAPACITY 10

#define SMALL_CAPACITY 2
#define GROWTH_FACTOR 2

#define TEST_CHAR_1 'A'
#define TEST_CHAR_2 'B'
#define TEST_CHAR_3 'C'

void test_buffer_init(void) {
    printf("TESTING: buffer_init\n");

    EditorBuffer eb;

    int buffer_init_result = buffer_init(&eb, INITIAL_CAPACITY);
    assert(buffer_init_result == BUFFER_SUCCESS);

    assert(eb.capacity == INITIAL_CAPACITY);
    assert(eb.gap_start == 0);
    assert(eb.gap_end == INITIAL_CAPACITY);
    assert(eb.data != NULL);

    buffer_free(&eb);
    printf("PASSED: buffer_init\n");
}

void test_buffer_grow(void) {
    printf("TESTING: buffer_grow\n");

    EditorBuffer eb;

    buffer_init(&eb, SMALL_CAPACITY);
    buffer_insert(&eb, TEST_CHAR_1);
    buffer_insert(&eb, TEST_CHAR_2);
    buffer_insert(&eb, TEST_CHAR_3);

    size_t expected_capacity = SMALL_CAPACITY * GROWTH_FACTOR;
    size_t items_inserted = 3;

    assert(eb.capacity == expected_capacity);
    assert(eb.gap_start == items_inserted); 
    assert(eb.gap_end == expected_capacity);
    assert(eb.data != NULL);

    buffer_free(&eb);
    printf("PASSED: buffer_grow\n");
}

void test_buffer_insert(void) {
    printf("TESTING: buffer_insert\n");

    EditorBuffer eb;
    buffer_init(&eb, INITIAL_CAPACITY);

    int buffer_insert_result = buffer_insert(&eb, TEST_CHAR_1);
    assert(buffer_insert_result == BUFFER_SUCCESS);

    size_t expected_gap_start = 1;

    assert(eb.capacity == INITIAL_CAPACITY);
    assert(eb.gap_start == expected_gap_start);
    assert(eb.gap_end == INITIAL_CAPACITY);
    assert(eb.data != NULL);

    buffer_free(&eb);
    printf("PASSED: buffer_insert\n");
}


void test_buffer_delete(void) {
    printf("TESTING: buffer_delete\n");

    EditorBuffer eb;
    buffer_init(&eb, INITIAL_CAPACITY);
    buffer_insert(&eb, TEST_CHAR_1);

    int buffer_delete_result = buffer_delete(&eb);
    assert(buffer_delete_result == BUFFER_SUCCESS);

    assert(eb.capacity == INITIAL_CAPACITY);
    assert(eb.gap_start == 0);
    assert(eb.gap_end == INITIAL_CAPACITY);
    assert(eb.data != NULL);

    buffer_free(&eb);
    printf("PASSED: buffer_delete\n");
}

void test_buffer_move_left(void) {
    printf("TESTING: buffer_move_left\n");

    EditorBuffer eb;
    buffer_init(&eb, INITIAL_CAPACITY);
    buffer_insert(&eb, TEST_CHAR_1);
    buffer_insert(&eb, TEST_CHAR_2);

    int buffer_move_left_result = buffer_move_left(&eb);
    assert(buffer_move_left_result == BUFFER_SUCCESS);

    size_t expected_gap_start = 1;
    size_t expected_gap_end = INITIAL_CAPACITY - 1;

    assert(eb.capacity == INITIAL_CAPACITY);
    assert(eb.gap_start == expected_gap_start);
    assert(eb.gap_end == expected_gap_end);

    assert(eb.data[0] == TEST_CHAR_1);
    assert(eb.data[expected_gap_end] == TEST_CHAR_2);
    assert(eb.data != NULL);

    buffer_free(&eb);
    printf("PASSED: buffer_move_left\n");
}

void test_buffer_move_right(void) {
    printf("TESTING: buffer_move_right\n");

    EditorBuffer eb;
    buffer_init(&eb, INITIAL_CAPACITY);
    buffer_insert(&eb, TEST_CHAR_1);
    buffer_insert(&eb, TEST_CHAR_2);
    buffer_move_left(&eb);

    int buffer_move_right_result = buffer_move_right(&eb);
    assert(buffer_move_right_result == BUFFER_SUCCESS);

    size_t expected_gap_start = 2;

    assert(eb.capacity == INITIAL_CAPACITY);
    assert(eb.gap_start == expected_gap_start);
    assert(eb.gap_end == INITIAL_CAPACITY);
    assert(eb.data[0] == TEST_CHAR_1);
    assert(eb.data[1] == TEST_CHAR_2);
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
