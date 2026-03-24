#ifndef BUFFER_H
#define BUFFER_H

#include <stddef.h>

typedef enum BufferStatus {
    BUFFER_SUCCESS = 0,
    BUFFER_ERR_INVALID_ARGUMENT = -1,
    BUFFER_ERR_ALLOCATION_FAILED = -2,
    BUFFER_ERR_START_OF_BUFFER = -3,
    BUFFER_ERR_END_OF_BUFFER = -4,
    BUFFER_ERR_START_OF_LINE = -5,
    BUFFER_ERR_END_OF_LINE = -6,
    BUFFER_ERR_FIRST_LINE = -7,
    BUFFER_ERR_LAST_LINE = -8
} BufferStatus;

typedef struct EditorBuffer {
    char *data;
    size_t capacity;
    size_t gap_start;
    size_t gap_end;
} EditorBuffer;

BufferStatus buffer_init(EditorBuffer *eb, size_t initial_capacity);
BufferStatus buffer_insert_char(EditorBuffer *eb, char c);
BufferStatus buffer_delete_before_cursor(EditorBuffer *eb);
size_t buffer_get_cursor_column(const EditorBuffer *eb);
BufferStatus buffer_move_left(EditorBuffer *eb);
BufferStatus buffer_move_right(EditorBuffer *eb);
BufferStatus buffer_move_up(EditorBuffer *eb, size_t preferred_column);
BufferStatus buffer_move_down(EditorBuffer *eb, size_t preferred_column);
BufferStatus buffer_move_to_line_start(EditorBuffer *eb);
BufferStatus buffer_move_to_first_non_blank(EditorBuffer *eb);
BufferStatus buffer_move_to_line_end(EditorBuffer *eb);
BufferStatus buffer_move_to_previous_word(EditorBuffer *eb);
BufferStatus buffer_move_to_next_word(EditorBuffer *eb);
BufferStatus buffer_move_to_word_end(EditorBuffer *eb);
BufferStatus buffer_move_to_file_start(EditorBuffer *eb);
BufferStatus buffer_move_to_file_end(EditorBuffer *eb);
BufferStatus buffer_free(EditorBuffer *eb);

#endif
