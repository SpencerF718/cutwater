#include "buffer.h"
#include <stdlib.h>
#include <stdio.h>

int buffer_init(EditorBuffer *eb, size_t initial_capacity) {
  if (eb == NULL) {
    fprintf(stderr, "Error: EditorBuffer pointer is NULL\n");
    return -1;
  }

  eb->data = malloc(initial_capacity * sizeof(char));

  if (eb->data == NULL) {
    perror("Failed to allocate memory for EditorBuffer");
    return -1;
  }

  eb->gap_start = 0;
  eb->gap_end = initial_capacity;
  eb->capacity = initial_capacity;

  return 0;
}

int buffer_insert(EditorBuffer *eb, char c) {
  if (eb == NULL) {
    fprintf(stderr, "Error: EditorBuffer pointer is NULL\n");
    return -1;
  }

  if (eb->gap_start == eb->gap_end) {
    // change later
    fprintf(stderr, "gap_start == gap_end\n");
    return -1;
  }

  eb->data[eb->gap_start] = c;
  eb->gap_start++;

  return 0;
}

int buffer_delete(EditorBuffer *eb) {
  if (eb == NULL) {
    fprintf(stderr, "Error: EditorBuffer pointer is NULL\n");
    return -1;
  }

  if (eb->gap_start == 0) {
    fprintf(stderr, "Error: Already at the beginning of the buffer\n");
    return -1;
  }

  eb->gap_start--;

  return 0;
}

int buffer_move_left(EditorBuffer *eb) {
  if (eb == NULL) {
    fprintf(stderr, "Error: EditorBuffer pointer is NULL\n");
    return -1;
  }

  if (eb->gap_start == 0) {
    fprintf(stderr, "Error: Already at the beginning of the buffer\n");
    return -1;
  }

  eb->data[eb->gap_end - 1] = eb->data[eb->gap_start - 1];

  eb->gap_start--;
  eb->gap_end--;

  return 0;
}

int buffer_move_right(EditorBuffer *eb) {
  if (eb == NULL) {
    fprintf(stderr, "Error: EditorBuffer pointer is NULL\n");
    return -1;
  }

  if (eb->gap_end == eb->capacity) {
    fprintf(stderr, "Error: Already at the end of the buffer\n");
    return -1;
  }

  eb->data[eb->gap_start] = eb->data[eb->gap_end];

  eb->gap_start++;
  eb->gap_end++;

  return 0;
}

int buffer_free(EditorBuffer *eb) {
  if (eb == NULL) {
    fprintf(stderr, "Error: EditorBuffer pointer is NULL\n");
    return -1;
  }

  free(eb->data);

  eb->data = NULL;
  eb->capacity = 0;

  return 0;
}
