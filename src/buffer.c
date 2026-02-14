#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include "buffer.h"

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

size_t buffer_get_column(EditorBuffer *eb) {
    size_t current_line_start = eb->gap_start;

    while (current_line_start > 0 && eb->data[current_line_start - 1] != '\n') {
        current_line_start--;
    }

    return eb->gap_start - current_line_start;
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

int buffer_move_up(EditorBuffer *eb, size_t preferred_column) {
    if (eb == NULL) {
        return -1;
    }

    size_t current_line_start = eb->gap_start;

    while (current_line_start > 0 && eb->data[current_line_start - 1] != '\n') {
        current_line_start--;
    }

    if (current_line_start == 0) {
        return -2;
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
    size_t move_size = eb->gap_start - target_position;

    if (move_size > 0) {
        memmove(eb->data + eb->gap_end - move_size, eb->data + target_position, move_size);
        eb->gap_start -= move_size;
        eb->gap_end -= move_size;
    }

    return 0;
}

int buffer_move_down(EditorBuffer *eb, size_t preferred_column) {
    if (eb == NULL) {
        return -1;
    }

    size_t scan_position = eb->gap_end;

    while (scan_position < eb->capacity && eb->data[scan_position] != '\n') {
        scan_position++;
    }

    if (scan_position == eb->capacity) {
        return -2;
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
    size_t move_size = target_position - eb->gap_end;

    if (move_size > 0) {
        memmove(eb->data + eb->gap_start, eb->data + eb->gap_end, move_size);
        eb->gap_start += move_size;
        eb->gap_end += move_size;
    }

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
