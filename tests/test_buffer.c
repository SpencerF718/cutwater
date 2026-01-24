#include "buffer.h"
#include <assert.h>
#include <stdio.h>

void test_buffer_init(void) {
  printf("TESTING: buffer_init\n");

  EditorBuffer eb;

  assert(buffer_init(&eb, 10) == 0);

  assert(eb.capacity == 10);
  assert(eb.gap_start == 0);
  assert(eb.gap_end == 10);
  assert(eb.data != NULL);

  buffer_free(&eb);
  printf("PASSED: buffer_init\n");
}

void test_buffer_insert(void) {
  printf("TESTING: buffer_insert\n");

  EditorBuffer eb;
  buffer_init(&eb, 10);

  assert(buffer_insert(&eb, 'A') == 0);

  assert(eb.capacity == 10);
  assert(eb.gap_start == 1);
  assert(eb.gap_end == 10);
  assert(eb.data != NULL);

  buffer_free(&eb);
  printf("PASSED: buffer_insert\n");
}

void test_buffer_move_left(void) {
  printf("TESTING: buffer_move_left\n");

  EditorBuffer eb;
  buffer_init(&eb, 10);
  buffer_insert(&eb, 'A');
  buffer_insert(&eb, 'B');

  assert(buffer_move_left(&eb) == 0);

  assert(eb.capacity == 10);
  assert(eb.gap_start == 1);
  assert(eb.gap_end == 9);
  assert(eb.data[0] == 'A');
  assert(eb.data[9] == 'B');
  assert(eb.data != NULL);

  buffer_free(&eb);
  printf("PASSED: buffer_move_left\n");
}

int main(void) {
  test_buffer_init();
  test_buffer_insert();
  test_buffer_move_left();
  return 0;
}
