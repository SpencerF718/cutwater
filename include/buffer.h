#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>

typedef struct EditorBuffer {
  char *data;
  size_t capacity;
  size_t gap_start;
  size_t gap_end;
} EditorBuffer;

int buffer_init(EditorBuffer *eb, size_t initial_capacity);
int buffer_grow(EditorBuffer *eb);
int buffer_insert(EditorBuffer *eb, char c);
int buffer_delete(EditorBuffer *eb);
int buffer_move_left(EditorBuffer *eb);
int buffer_move_right(EditorBuffer *eb);
int buffer_free(EditorBuffer *eb);

#endif
