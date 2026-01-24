#include "buffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int buffer_init(EditorBuffer *eb, size_t initial_capacity) {
  if (eb == NULL) {
    return -1;
  }

  eb->data = malloc(initial_capacity * sizeof(char));

  if (eb->data == NULL) {
    return -2;
  }

  eb->gap_start = 0;
  eb->gap_end = initial_capacity;
  eb->capacity = initial_capacity;

  return 0;
}

int buffer_grow(EditorBuffer *eb) {
  if (eb == NULL) {
    return -1;
  }

  size_t new_capacity = eb->capacity * 2;
  char *new_data = realloc(eb->data, new_capacity * sizeof(char));

  if (new_data == NULL) {
    perror("Failed to realloc memory");
    return -2;
  }

  eb->data = new_data;

  size_t new_gap_end = eb->gap_end + (new_capacity - eb->capacity);
  memmove(&eb->data[new_gap_end], &eb->data[eb->gap_end], eb->capacity - eb->gap_end);

  eb->gap_end = new_gap_end;
  eb->capacity = new_capacity;

  return 0;
}

int buffer_insert(EditorBuffer *eb, char c) {
  if (eb == NULL) {
    return -1;
  }

  if (eb->gap_start >= eb->gap_end) {
    int grow_result = buffer_grow(eb);
    if (grow_result == -1) {
      return -2;
    }
  }

  eb->data[eb->gap_start] = c;
  eb->gap_start++;

  return 0;
}

int buffer_delete(EditorBuffer *eb) {
  if (eb == NULL) {
    return -1;
  }

  if (eb->gap_start == 0) {
    return -2;
  }

  eb->gap_start--;

  return 0;
}

int buffer_move_left(EditorBuffer *eb) {
  if (eb == NULL) {
    return -1;
  }

  if (eb->gap_start == 0) {
    return -2;
  }

  eb->data[eb->gap_end - 1] = eb->data[eb->gap_start - 1];

  eb->gap_start--;
  eb->gap_end--;

  return 0;
}

int buffer_move_right(EditorBuffer *eb) {
  if (eb == NULL) {
    return -1;
  }

  if (eb->gap_end == eb->capacity) {
    return -2;
  }

  eb->data[eb->gap_start] = eb->data[eb->gap_end];

  eb->gap_start++;
  eb->gap_end++;

  return 0;
}

int buffer_free(EditorBuffer *eb) {
  if (eb == NULL) {
    return -1;
  }

  free(eb->data);

  eb->data = NULL;
  eb->capacity = 0;

  return 0;
}
