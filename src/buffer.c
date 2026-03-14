#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include "buffer.h"

#define MIN_CAPACITY 1024

static BufferStatus buffer_grow(EditorBuffer *eb);
static BufferStatus buffer_move_gap(EditorBuffer *eb, size_t target_position);
static int buffer_is_keyword(char c);
static int buffer_is_space(char c);
static int buffer_is_punctuation(char c);

BufferStatus buffer_init(EditorBuffer *eb, size_t initial_capacity) {
    if (eb == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    eb->data = malloc(initial_capacity * sizeof(char));

    if (eb->data == NULL) {
        return BUFFER_ERR_ALLOCATION_FAILED;
    }

    eb->gap_start = 0;
    eb->gap_end = initial_capacity;
    eb->capacity = initial_capacity;

    return BUFFER_SUCCESS;
}

static BufferStatus buffer_grow(EditorBuffer *eb) {
    if (eb == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    size_t new_capacity;
    if (eb->capacity <= MIN_CAPACITY) {
        new_capacity = MIN_CAPACITY;
    } else {
        new_capacity = eb->capacity << 1;
    }

    char *new_data = realloc(eb->data, new_capacity * sizeof(char));

    if (new_data == NULL) {
        return BUFFER_ERR_ALLOCATION_FAILED;
    }

    eb->data = new_data;

    size_t new_gap_end = eb->gap_end + (new_capacity - eb->capacity);
    memmove(&eb->data[new_gap_end], &eb->data[eb->gap_end], eb->capacity - eb->gap_end);

    eb->gap_end = new_gap_end;
    eb->capacity = new_capacity;

    return BUFFER_SUCCESS;
}

static BufferStatus buffer_move_gap(EditorBuffer *eb, size_t target_position) {
    if (eb == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    if (target_position < eb->gap_start) {
        size_t move_size = eb->gap_start - target_position;
        memmove(eb->data + eb->gap_end - move_size, eb->data + target_position, move_size);
        eb->gap_start -= move_size;
        eb->gap_end -= move_size;

    } else if (target_position > eb->gap_end) {
        size_t move_size = target_position - eb->gap_end;
        memmove(eb->data + eb->gap_start, eb->data + eb->gap_end, move_size);
        eb->gap_start += move_size;
        eb->gap_end += move_size;
    }

    return BUFFER_SUCCESS;
}

static int buffer_is_keyword(char c) {
    return isalnum((unsigned char)c) || c == '_';
}

static int buffer_is_space(char c) {
    return isspace((unsigned char)c);
}

static int buffer_is_punctuation(char c) {
    return !buffer_is_keyword(c) && !buffer_is_space(c);
}

BufferStatus buffer_insert(EditorBuffer *eb, char c) {
    if (eb == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    if (eb->gap_start >= eb->gap_end) {
        BufferStatus grow_result = buffer_grow(eb);
        if (grow_result != BUFFER_SUCCESS) {
            return grow_result;
        }
    }

    eb->data[eb->gap_start] = c;
    eb->gap_start++;

    return BUFFER_SUCCESS;
}

BufferStatus buffer_delete(EditorBuffer *eb) {
    if (eb == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    if (eb->gap_start == 0) {
        return BUFFER_ERR_START_OF_BUFFER;
    }

    eb->gap_start--;

    return BUFFER_SUCCESS;
}

size_t buffer_get_column(EditorBuffer *eb) {
    size_t current_line_start = eb->gap_start;

    while (current_line_start > 0 && eb->data[current_line_start - 1] != '\n') {
        current_line_start--;
    }

    return eb->gap_start - current_line_start;
}

BufferStatus buffer_move_left(EditorBuffer *eb) {
    if (eb == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    if (eb->gap_start == 0) {
        return BUFFER_ERR_START_OF_BUFFER;
    }

    if (eb->data[eb->gap_start - 1] == '\n') {
        return BUFFER_ERR_START_OF_LINE;
    }

    eb->data[eb->gap_end - 1] = eb->data[eb->gap_start - 1];

    eb->gap_start--;
    eb->gap_end--;

    return BUFFER_SUCCESS;
}

BufferStatus buffer_move_right(EditorBuffer *eb) {
    if (eb == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    if (eb->gap_end == eb->capacity) {
        return BUFFER_ERR_END_OF_BUFFER;
    }

    if (eb->data[eb->gap_end] == '\n') {
        return BUFFER_ERR_END_OF_LINE;
    }

    eb->data[eb->gap_start] = eb->data[eb->gap_end];

    eb->gap_start++;
    eb->gap_end++;

    return BUFFER_SUCCESS;
}

BufferStatus buffer_move_up(EditorBuffer *eb, size_t preferred_column) {
    if (eb == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    size_t current_line_start = eb->gap_start;

    while (current_line_start > 0 && eb->data[current_line_start - 1] != '\n') {
        current_line_start--;
    }

    if (current_line_start == 0) {
        return BUFFER_ERR_FIRST_LINE;
    }

    size_t previous_line_end = current_line_start - 1;
    size_t previous_line_start = previous_line_end;

    while (previous_line_start > 0 && eb->data[previous_line_start - 1] != '\n') {
        previous_line_start--;
    }

    size_t previous_line_length = previous_line_end - previous_line_start;
    size_t clamped_column = preferred_column;

    if (clamped_column > previous_line_length) {
        clamped_column = previous_line_length;
    }

    size_t target_position = previous_line_start + clamped_column;
    return buffer_move_gap(eb, target_position);
}

BufferStatus buffer_move_down(EditorBuffer *eb, size_t preferred_column) {
    if (eb == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    size_t scan_position = eb->gap_end;

    while (scan_position < eb->capacity && eb->data[scan_position] != '\n') {
        scan_position++;
    }

    if (scan_position == eb->capacity) {
        return BUFFER_ERR_LAST_LINE;
    }

    size_t next_line_start = scan_position + 1;
    size_t next_line_end = next_line_start;

    while (next_line_end < eb->capacity && eb->data[next_line_end] != '\n') {
        next_line_end++;
    }

    size_t next_line_length = next_line_end - next_line_start;
    size_t clamped_column = preferred_column;

    if (clamped_column > next_line_length) {
        clamped_column = next_line_length;
    }

    size_t target_position = next_line_start + clamped_column;
    return buffer_move_gap(eb, target_position);
}

BufferStatus buffer_move_line_start(EditorBuffer *eb) {
    if (eb == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    size_t target_position = eb->gap_start;

    while (target_position > 0 && eb->data[target_position - 1] != '\n') {
        target_position--;
    }

    return buffer_move_gap(eb, target_position);
}

BufferStatus buffer_move_line_end(EditorBuffer *eb) {
    if (eb == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    size_t target_position = eb->gap_end;

    while (target_position < eb->capacity && eb->data[target_position] != '\n') {
        target_position++;
    }

    return buffer_move_gap(eb, target_position);
}

BufferStatus buffer_move_prev_word(EditorBuffer *eb) {
    if (eb == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    if (eb->gap_start == 0) {
        return BUFFER_SUCCESS;
    }

    size_t target_position = eb->gap_start - 1;

    while (target_position > 0 && buffer_is_space(eb->data[target_position])) {
        target_position--;
    }

    if (buffer_is_keyword(eb->data[target_position])) {
        while (target_position > 0 && buffer_is_keyword(eb->data[target_position - 1])) {
            target_position--;
        }
    } else if (buffer_is_punctuation(eb->data[target_position])) {
        while (target_position > 0 && buffer_is_punctuation(eb->data[target_position - 1])) {
            target_position--;
        }
    }

    return buffer_move_gap(eb, target_position);
}

BufferStatus buffer_move_next_word(EditorBuffer *eb) {
    if (eb == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    size_t target_position = eb->gap_end;

    if (target_position >= eb->capacity) {
        return BUFFER_SUCCESS;
    }

    char current_char = eb->data[target_position];

    if (buffer_is_keyword(current_char)) {
        while (target_position < eb->capacity && buffer_is_keyword(eb->data[target_position])) {
            target_position++;
        }
    } else if (buffer_is_punctuation(current_char)) {
        while (target_position < eb->capacity && buffer_is_punctuation(eb->data[target_position])) {
            target_position++;
        }
    }

    while (target_position < eb->capacity && buffer_is_space(eb->data[target_position])) {
        target_position++;
    }

    return buffer_move_gap(eb, target_position);
}

BufferStatus buffer_free(EditorBuffer *eb) {
    if (eb == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    free(eb->data);

    eb->data = NULL;
    eb->capacity = 0;
    eb->gap_start = 0;
    eb->gap_end = 0;

    return BUFFER_SUCCESS;
}
