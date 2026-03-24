#include "buffer.h"
#include <assert.h>
#include <stdio.h>

#define INITIAL_CAPACITY 10

#define SMALL_CAPACITY 2
#define GROWTH_FACTOR 2
#define MIN_CAPACITY 1024

#define TEST_CHAR_1 'A'
#define TEST_CHAR_2 'B'
#define TEST_CHAR_3 'C'
#define TEST_CHAR_4 'D'
#define TEST_CHAR_5 'E'
#define TEST_CHAR_6 'F'

void test_buffer_init(void) {
    printf("TESTING: buffer_init\n");

    EditorBuffer eb;

    BufferStatus buffer_init_result = buffer_init(&eb, INITIAL_CAPACITY);
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
    buffer_insert_char(&eb, TEST_CHAR_1);
    buffer_insert_char(&eb, TEST_CHAR_2);
    buffer_insert_char(&eb, TEST_CHAR_3);

    size_t expected_capacity = MIN_CAPACITY;
    size_t items_inserted = 3;

    assert(eb.capacity == expected_capacity);
    assert(eb.gap_start == items_inserted); 
    assert(eb.gap_end == expected_capacity);
    assert(eb.data != NULL);

    buffer_free(&eb);
    printf("PASSED: buffer_grow\n");
}

void test_buffer_insert_char(void) {
    printf("TESTING: buffer_insert_char\n");

    EditorBuffer eb;
    buffer_init(&eb, INITIAL_CAPACITY);

    BufferStatus buffer_insert_result = buffer_insert_char(&eb, TEST_CHAR_1);
    assert(buffer_insert_result == BUFFER_SUCCESS);

    size_t expected_gap_start = 1;

    assert(eb.capacity == INITIAL_CAPACITY);
    assert(eb.gap_start == expected_gap_start);
    assert(eb.gap_end == INITIAL_CAPACITY);
    assert(eb.data != NULL);

    buffer_free(&eb);
    printf("PASSED: buffer_insert_char\n");
}


void test_buffer_delete_before_cursor(void) {
    printf("TESTING: buffer_delete_before_cursor\n");

    EditorBuffer eb;
    buffer_init(&eb, INITIAL_CAPACITY);
    buffer_insert_char(&eb, TEST_CHAR_1);

    BufferStatus buffer_delete_result = buffer_delete_before_cursor(&eb);
    assert(buffer_delete_result == BUFFER_SUCCESS);

    assert(eb.capacity == INITIAL_CAPACITY);
    assert(eb.gap_start == 0);
    assert(eb.gap_end == INITIAL_CAPACITY);
    assert(eb.data != NULL);

    buffer_free(&eb);
    printf("PASSED: buffer_delete_before_cursor\n");
}

void test_buffer_get_cursor_column(void) {
    printf("TESTING: buffer_get_cursor_column\n");

    EditorBuffer eb;
    buffer_init(&eb, INITIAL_CAPACITY);

    size_t col = buffer_get_cursor_column(&eb);
    assert(col == 0);

    buffer_insert_char(&eb, TEST_CHAR_1);
    col = buffer_get_cursor_column(&eb);
    assert(col == 1);

    buffer_insert_char(&eb, TEST_CHAR_2);
    col = buffer_get_cursor_column(&eb);
    assert(col == 2);

    buffer_insert_char(&eb, '\n');
    col = buffer_get_cursor_column(&eb);
    assert(col == 0);

    buffer_insert_char(&eb, TEST_CHAR_3);
    col = buffer_get_cursor_column(&eb);
    assert(col == 1);

    buffer_free(&eb);
    printf("PASSED: buffer_get_cursor_column\n");
}

void test_buffer_move_left(void) {
    printf("TESTING: buffer_move_left\n");

    EditorBuffer eb;
    buffer_init(&eb, INITIAL_CAPACITY);

    BufferStatus bounds_result = buffer_move_left(&eb);
    assert(bounds_result == BUFFER_ERR_START_OF_BUFFER);

    buffer_insert_char(&eb, TEST_CHAR_1);
    buffer_insert_char(&eb, TEST_CHAR_2);

    BufferStatus buffer_move_left_result = buffer_move_left(&eb);
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
    buffer_init(&eb, INITIAL_CAPACITY);
    buffer_insert_char(&eb, TEST_CHAR_1);
    buffer_insert_char(&eb, '\n');
    buffer_insert_char(&eb, TEST_CHAR_2);

    BufferStatus move_over_char_result = buffer_move_left(&eb);
    assert(move_over_char_result == BUFFER_SUCCESS);

    BufferStatus move_over_newline_result = buffer_move_left(&eb);
    assert(move_over_newline_result == BUFFER_ERR_START_OF_LINE);

    buffer_free(&eb);
    printf("PASSED: buffer_move_left\n");
}

void test_buffer_move_right(void) {
    printf("TESTING: buffer_move_right\n");

    EditorBuffer eb;
    buffer_init(&eb, INITIAL_CAPACITY);

    BufferStatus bounds_result = buffer_move_right(&eb);
    assert(bounds_result == BUFFER_ERR_END_OF_BUFFER);

    buffer_insert_char(&eb, TEST_CHAR_1);
    buffer_insert_char(&eb, TEST_CHAR_2);
    buffer_move_left(&eb);

    BufferStatus buffer_move_right_result = buffer_move_right(&eb);
    assert(buffer_move_right_result == BUFFER_SUCCESS);

    size_t expected_gap_start = 2;

    assert(eb.capacity == INITIAL_CAPACITY);
    assert(eb.gap_start == expected_gap_start);
    assert(eb.gap_end == INITIAL_CAPACITY);
    assert(eb.data[0] == TEST_CHAR_1);
    assert(eb.data[1] == TEST_CHAR_2);
    assert(eb.data != NULL);

    buffer_free(&eb);
    buffer_init(&eb, INITIAL_CAPACITY);
    buffer_insert_char(&eb, TEST_CHAR_1);
    buffer_insert_char(&eb, '\n');
    buffer_insert_char(&eb, TEST_CHAR_2);
    buffer_move_left(&eb);
    buffer_move_up(&eb, 1);

    BufferStatus move_over_newline_result = buffer_move_right(&eb);
    assert(move_over_newline_result == BUFFER_ERR_END_OF_LINE);

    buffer_free(&eb);
    printf("PASSED: buffer_move_right\n");
}

void test_buffer_move_up(void) {
    printf("TESTING: buffer_move_up\n");

    EditorBuffer eb;
    buffer_init(&eb, INITIAL_CAPACITY);

    buffer_insert_char(&eb, TEST_CHAR_1);
    buffer_insert_char(&eb, TEST_CHAR_2);
    buffer_insert_char(&eb, '\n');
    buffer_insert_char(&eb, TEST_CHAR_3);
    buffer_insert_char(&eb, 'D');

    size_t col = buffer_get_cursor_column(&eb);
    assert(col == 2);

    BufferStatus result = buffer_move_up(&eb, 1);
    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 1);

    col = buffer_get_cursor_column(&eb);
    assert(col == 1);

    buffer_move_down(&eb, 5);
    while (buffer_move_right(&eb) == BUFFER_SUCCESS);

    result = buffer_move_up(&eb, 5);
    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 2); 

    col = buffer_get_cursor_column(&eb);
    assert(col == 2);

    buffer_move_up(&eb, 0);
    while (buffer_move_left(&eb) == BUFFER_SUCCESS);

    result = buffer_move_up(&eb, 0);
    assert(result != BUFFER_SUCCESS);

    buffer_free(&eb);
    printf("PASSED: buffer_move_up\n");
}

void test_buffer_move_down(void) {
    printf("TESTING: buffer_move_down\n");

    EditorBuffer eb;
    buffer_init(&eb, INITIAL_CAPACITY);

    buffer_insert_char(&eb, TEST_CHAR_1);
    buffer_insert_char(&eb, '\n');
    buffer_insert_char(&eb, TEST_CHAR_2);
    buffer_insert_char(&eb, TEST_CHAR_3);

    buffer_move_up(&eb, 0);
    while (buffer_move_left(&eb) == BUFFER_SUCCESS);

    BufferStatus result = buffer_move_down(&eb, 1);
    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 3);

    size_t col = buffer_get_cursor_column(&eb);
    assert(col == 1);

    result = buffer_move_down(&eb, 0);
    assert(result != BUFFER_SUCCESS);

    buffer_free(&eb);
    printf("PASSED: buffer_move_down\n");
}

void test_buffer_move_to_line_start(void) {
    printf("TESTING: buffer_move_to_line_start\n");

    EditorBuffer eb;
    buffer_init(&eb, INITIAL_CAPACITY);

    buffer_insert_char(&eb, TEST_CHAR_1);
    buffer_insert_char(&eb, TEST_CHAR_2);
    buffer_insert_char(&eb, TEST_CHAR_3);
    buffer_insert_char(&eb, '\n');
    buffer_insert_char(&eb, TEST_CHAR_4);
    buffer_insert_char(&eb, TEST_CHAR_5);
    buffer_insert_char(&eb, TEST_CHAR_6);

    BufferStatus result = buffer_move_to_line_start(&eb);

    assert(result == 0);
    assert(eb.gap_start == 4);
    assert(eb.data[eb.gap_end] == TEST_CHAR_4);

    buffer_free(&eb);
    printf("PASSED: buffer_move_to_line_start\n");
}

void test_buffer_move_to_first_non_blank(void) {
    printf("TESTING: buffer_move_to_first_non_blank\n");

    EditorBuffer eb;
    buffer_init(&eb, INITIAL_CAPACITY);

    BufferStatus result = buffer_move_to_first_non_blank(&eb);
    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 0);

    buffer_free(&eb);
    buffer_init(&eb, INITIAL_CAPACITY);

    buffer_insert_char(&eb, TEST_CHAR_1);
    buffer_insert_char(&eb, TEST_CHAR_2);
    buffer_insert_char(&eb, TEST_CHAR_3);
    buffer_insert_char(&eb, '\n');
    buffer_insert_char(&eb, ' ');
    buffer_insert_char(&eb, ' ');
    buffer_insert_char(&eb, TEST_CHAR_4);
    buffer_insert_char(&eb, TEST_CHAR_5);
    buffer_insert_char(&eb, TEST_CHAR_6);

    result = buffer_move_to_first_non_blank(&eb);
    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 6);
    assert(eb.data[eb.gap_end] == TEST_CHAR_4);

    buffer_free(&eb);
    buffer_init(&eb, INITIAL_CAPACITY);

    buffer_insert_char(&eb, ' ');
    buffer_insert_char(&eb, ' ');

    result = buffer_move_to_first_non_blank(&eb);
    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 0);
    assert(eb.data[eb.gap_end] == ' ');

    buffer_free(&eb);
    printf("PASSED: buffer_move_to_first_non_blank\n");
}

void test_buffer_move_to_line_end(void) {
    printf("TESTING: buffer_move_to_line_end\n");

    EditorBuffer eb;
    buffer_init(&eb, INITIAL_CAPACITY);

    buffer_insert_char(&eb, TEST_CHAR_1);
    buffer_insert_char(&eb, TEST_CHAR_2);
    buffer_insert_char(&eb, TEST_CHAR_3);
    buffer_insert_char(&eb, '\n');
    buffer_insert_char(&eb, TEST_CHAR_4);
    buffer_insert_char(&eb, TEST_CHAR_5);
    buffer_insert_char(&eb, TEST_CHAR_6);

    buffer_move_to_line_start(&eb);

    BufferStatus result = buffer_move_to_line_end(&eb);

    assert(result == 0);
    assert(eb.gap_start == 7);

    buffer_free(&eb);
    printf("PASSED: buffer_move_to_line_end\n");
}

void test_buffer_move_to_next_word(void) {
    printf("TESTING: buffer_move_to_next_word\n");

    EditorBuffer eb;
    buffer_init(&eb, INITIAL_CAPACITY);

    BufferStatus result = buffer_move_to_next_word(&eb);
    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 0);

    buffer_insert_char(&eb, TEST_CHAR_1);
    buffer_insert_char(&eb, TEST_CHAR_2);
    buffer_insert_char(&eb, TEST_CHAR_3);
    buffer_insert_char(&eb, ' ');
    buffer_insert_char(&eb, ' ');
    buffer_insert_char(&eb, TEST_CHAR_4);
    buffer_insert_char(&eb, TEST_CHAR_5);
    buffer_insert_char(&eb, TEST_CHAR_6);

    while (buffer_move_left(&eb) == BUFFER_SUCCESS);

    result = buffer_move_to_next_word(&eb);

    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 5);
    assert(eb.data[eb.gap_end] == TEST_CHAR_4);

    result = buffer_move_to_next_word(&eb);
    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 8);

    buffer_free(&eb);
    buffer_init(&eb, INITIAL_CAPACITY);

    buffer_insert_char(&eb, '+');
    buffer_insert_char(&eb, '+');
    buffer_insert_char(&eb, '+');
    buffer_insert_char(&eb, ' ');
    buffer_insert_char(&eb, TEST_CHAR_1);
    buffer_insert_char(&eb, TEST_CHAR_2);
    buffer_insert_char(&eb, TEST_CHAR_3);

    while (buffer_move_left(&eb) == BUFFER_SUCCESS);

    result = buffer_move_to_next_word(&eb);
    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 4);
    assert(eb.data[eb.gap_end] == TEST_CHAR_1);

    buffer_free(&eb);
    printf("PASSED: buffer_move_to_next_word\n");
}

void test_buffer_move_to_previous_word(void) {
    printf("TESTING: buffer_move_to_previous_word\n");

    EditorBuffer eb;
    buffer_init(&eb, INITIAL_CAPACITY);

    BufferStatus result = buffer_move_to_previous_word(&eb);
    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 0);

    buffer_insert_char(&eb, TEST_CHAR_1);
    buffer_insert_char(&eb, TEST_CHAR_2);
    buffer_insert_char(&eb, TEST_CHAR_3);
    buffer_insert_char(&eb, ' ');
    buffer_insert_char(&eb, ' ');
    buffer_insert_char(&eb, TEST_CHAR_4);
    buffer_insert_char(&eb, TEST_CHAR_5);
    buffer_insert_char(&eb, TEST_CHAR_6);

    result = buffer_move_to_previous_word(&eb);
    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 5);
    assert(eb.data[eb.gap_end] == TEST_CHAR_4);

    result = buffer_move_to_previous_word(&eb);
    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 0);
    assert(eb.data[eb.gap_end] == TEST_CHAR_1);

    buffer_free(&eb);
    buffer_init(&eb, INITIAL_CAPACITY);

    buffer_insert_char(&eb, TEST_CHAR_1);
    buffer_insert_char(&eb, TEST_CHAR_2);
    buffer_insert_char(&eb, TEST_CHAR_3);
    buffer_insert_char(&eb, ' ');
    buffer_insert_char(&eb, '+');
    buffer_insert_char(&eb, '+');
    buffer_insert_char(&eb, '+');

    result = buffer_move_to_previous_word(&eb);
    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 4);
    assert(eb.data[eb.gap_end] == '+');

    buffer_free(&eb);
    printf("PASSED: buffer_move_to_previous_word\n");
}

void test_buffer_move_to_word_end(void) {
    printf("TESTING: buffer_move_to_word_end\n");

    EditorBuffer eb;
    buffer_init(&eb, INITIAL_CAPACITY);

    BufferStatus result = buffer_move_to_word_end(&eb);
    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 0);

    buffer_free(&eb);
    buffer_init(&eb, INITIAL_CAPACITY);

    buffer_insert_char(&eb, TEST_CHAR_1);
    buffer_insert_char(&eb, TEST_CHAR_2);
    buffer_insert_char(&eb, TEST_CHAR_3);
    buffer_insert_char(&eb, ' ');
    buffer_insert_char(&eb, ' ');
    buffer_insert_char(&eb, TEST_CHAR_4);
    buffer_insert_char(&eb, TEST_CHAR_5);
    buffer_insert_char(&eb, TEST_CHAR_6);

    while (buffer_move_left(&eb) == BUFFER_SUCCESS);

    result = buffer_move_to_word_end(&eb);
    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 2);
    assert(eb.data[eb.gap_end] == TEST_CHAR_3);

    result = buffer_move_to_word_end(&eb);
    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 7);
    assert(eb.data[eb.gap_end] == TEST_CHAR_6);

    buffer_free(&eb);
    buffer_init(&eb, INITIAL_CAPACITY);

    buffer_insert_char(&eb, '+');
    buffer_insert_char(&eb, '+');
    buffer_insert_char(&eb, '+');
    buffer_insert_char(&eb, ' ');
    buffer_insert_char(&eb, TEST_CHAR_1);

    while (buffer_move_left(&eb) == BUFFER_SUCCESS);

    result = buffer_move_to_word_end(&eb);
    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 2);
    assert(eb.data[eb.gap_end] == '+');

    buffer_free(&eb);
    printf("PASSED: buffer_move_to_word_end\n");
}

void test_buffer_move_to_file_start(void) {
    printf("TESTING: buffer_move_to_file_start\n");

    EditorBuffer eb;
    buffer_init(&eb, INITIAL_CAPACITY);

    BufferStatus result = buffer_move_to_file_start(&eb);
    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 0);
    assert(eb.gap_end == INITIAL_CAPACITY);

    buffer_free(&eb);
    buffer_init(&eb, INITIAL_CAPACITY);

    buffer_insert_char(&eb, TEST_CHAR_1);
    buffer_insert_char(&eb, TEST_CHAR_2);
    buffer_insert_char(&eb, '\n');
    buffer_insert_char(&eb, TEST_CHAR_3);

    result = buffer_move_left(&eb);
    assert(result == BUFFER_SUCCESS);
    result = buffer_move_left(&eb);
    assert(result == BUFFER_ERR_START_OF_LINE);

    result = buffer_move_to_file_start(&eb);
    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 0);
    assert(eb.gap_end == INITIAL_CAPACITY - 4);
    assert(eb.data[eb.gap_end] == TEST_CHAR_1);

    buffer_free(&eb);
    printf("PASSED: buffer_move_to_file_start\n");
}

void test_buffer_move_to_file_end(void) {
    printf("TESTING: buffer_move_to_file_end\n");

    EditorBuffer eb;
    buffer_init(&eb, INITIAL_CAPACITY);

    BufferStatus result = buffer_move_to_file_end(&eb);
    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 0);
    assert(eb.gap_end == INITIAL_CAPACITY);

    buffer_free(&eb);
    buffer_init(&eb, INITIAL_CAPACITY);

    buffer_insert_char(&eb, TEST_CHAR_1);
    buffer_insert_char(&eb, TEST_CHAR_2);
    buffer_insert_char(&eb, '\n');
    buffer_insert_char(&eb, TEST_CHAR_3);

    result = buffer_move_to_file_start(&eb);
    assert(result == BUFFER_SUCCESS);

    result = buffer_move_to_file_end(&eb);
    assert(result == BUFFER_SUCCESS);
    assert(eb.gap_start == 4);
    assert(eb.gap_end == INITIAL_CAPACITY);
    assert(eb.data[0] == TEST_CHAR_1);
    assert(eb.data[1] == TEST_CHAR_2);
    assert(eb.data[2] == '\n');
    assert(eb.data[3] == TEST_CHAR_3);

    buffer_free(&eb);
    printf("PASSED: buffer_move_to_file_end\n");
}

int main(void) {
    test_buffer_init();
    test_buffer_grow();
    test_buffer_insert_char();
    test_buffer_delete_before_cursor();
    test_buffer_get_cursor_column();
    test_buffer_move_left();
    test_buffer_move_right();
    test_buffer_move_up();
    test_buffer_move_down();
    test_buffer_move_to_line_start();
    test_buffer_move_to_first_non_blank();
    test_buffer_move_to_line_end();
    test_buffer_move_to_next_word();
    test_buffer_move_to_previous_word();
    test_buffer_move_to_word_end();
    test_buffer_move_to_file_start();
    test_buffer_move_to_file_end();

    printf("--- ALL TESTS PASSED ---\n");

    return 0;
}
