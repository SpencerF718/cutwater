#include <stdlib.h>
#include <string.h>

#include "internal/buffer_internal.h"

#define MIN_BUFFER_CAPACITY 1024

static size_t buffer_select_expanded_capacity(size_t current_capacity) {
    if (current_capacity < MIN_BUFFER_CAPACITY) {
        return MIN_BUFFER_CAPACITY;
    }

    return current_capacity << 1;
}

BufferStatus buffer_init(EditorBuffer *buffer, size_t initial_capacity) {
    if (buffer == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    buffer->data = malloc(initial_capacity * sizeof(char));
    if (buffer->data == NULL) {
        return BUFFER_ERR_ALLOCATION_FAILED;
    }

    buffer->gap_start = 0;
    buffer->gap_end = initial_capacity;
    buffer->capacity = initial_capacity;

    return BUFFER_SUCCESS;
}

BufferStatus buffer_grow(EditorBuffer *buffer) {
    size_t new_capacity;
    char *new_data;
    size_t new_gap_end;

    if (buffer == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    new_capacity = buffer_select_expanded_capacity(buffer->capacity);
    new_data = realloc(buffer->data, new_capacity * sizeof(char));
    if (new_data == NULL) {
        return BUFFER_ERR_ALLOCATION_FAILED;
    }

    buffer->data = new_data;

    new_gap_end = buffer->gap_end + (new_capacity - buffer->capacity);
    memmove(&buffer->data[new_gap_end],
            &buffer->data[buffer->gap_end],
            buffer->capacity - buffer->gap_end);

    buffer->gap_end = new_gap_end;
    buffer->capacity = new_capacity;

    return BUFFER_SUCCESS;
}

BufferStatus buffer_move_gap(EditorBuffer *buffer, size_t target_position) {
    if (buffer == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    if (target_position < buffer->gap_start) {
        size_t move_size = buffer->gap_start - target_position;

        memmove(buffer->data + buffer->gap_end - move_size,
                buffer->data + target_position,
                move_size);

        buffer->gap_start -= move_size;
        buffer->gap_end -= move_size;
    } else if (target_position > buffer->gap_end) {
        size_t move_size = target_position - buffer->gap_end;

        memmove(buffer->data + buffer->gap_start,
                buffer->data + buffer->gap_end,
                move_size);

        buffer->gap_start += move_size;
        buffer->gap_end += move_size;
    }

    return BUFFER_SUCCESS;
}

BufferStatus buffer_insert_char(EditorBuffer *buffer, char c) {
    BufferStatus grow_result;

    if (buffer == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    if (buffer->gap_start >= buffer->gap_end) {
        grow_result = buffer_grow(buffer);
        if (grow_result != BUFFER_SUCCESS) {
            return grow_result;
        }
    }

    buffer->data[buffer->gap_start] = c;
    buffer->gap_start++;

    return BUFFER_SUCCESS;
}

BufferStatus buffer_delete_before_cursor(EditorBuffer *buffer) {
    if (buffer == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    if (buffer->gap_start == 0) {
        return BUFFER_ERR_START_OF_BUFFER;
    }

    buffer->gap_start--;

    return BUFFER_SUCCESS;
}

BufferStatus buffer_free(EditorBuffer *buffer) {
    if (buffer == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    free(buffer->data);

    buffer->data = NULL;
    buffer->capacity = 0;
    buffer->gap_start = 0;
    buffer->gap_end = 0;

    return BUFFER_SUCCESS;
}
