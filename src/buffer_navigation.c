#include <ctype.h>

#include "internal/buffer_internal.h"

typedef enum BufferCharClass {
    BUFFER_CHAR_SPACE,
    BUFFER_CHAR_KEYWORD,
    BUFFER_CHAR_PUNCTUATION
} BufferCharClass;

static size_t buffer_find_line_start_before(const EditorBuffer *buffer, size_t position) {
    while (position > 0 && buffer->data[position - 1] != '\n') {
        position--;
    }

    return position;
}

static size_t buffer_find_line_end_after(const EditorBuffer *buffer, size_t position) {
    while (position < buffer->capacity && buffer->data[position] != '\n') {
        position++;
    }

    return position;
}

static int buffer_is_keyword_char(char c) {
    return isalnum((unsigned char)c) || c == '_';
}

static int buffer_is_blank_char(char c) {
    return c == ' ' || c == '\t';
}

static int buffer_is_space_char(char c) {
    return isspace((unsigned char)c);
}

static BufferCharClass buffer_classify_char(char c) {
    if (buffer_is_space_char(c)) {
        return BUFFER_CHAR_SPACE;
    }

    if (buffer_is_keyword_char(c)) {
        return BUFFER_CHAR_KEYWORD;
    }

    return BUFFER_CHAR_PUNCTUATION;
}

static size_t buffer_find_run_start(const EditorBuffer *buffer,
                                    size_t position,
                                    BufferCharClass char_class) {
    while (position > 0 &&
        buffer_classify_char(buffer->data[position - 1]) == char_class) {
        position--;
    }

    return position;
}

static size_t buffer_advance_run(const EditorBuffer *buffer,
                                 size_t position,
                                 BufferCharClass char_class) {
    while (position < buffer->capacity &&
        buffer_classify_char(buffer->data[position]) == char_class) {
        position++;
    }

    return position;
}

size_t buffer_get_cursor_column(const EditorBuffer *buffer) {
    size_t current_line_start = buffer_find_line_start_before(buffer, buffer->gap_start);

    return buffer->gap_start - current_line_start;
}

BufferStatus buffer_move_left(EditorBuffer *buffer) {
    if (buffer == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    if (buffer->gap_start == 0) {
        return BUFFER_ERR_START_OF_BUFFER;
    }

    if (buffer->data[buffer->gap_start - 1] == '\n') {
        return BUFFER_ERR_START_OF_LINE;
    }

    buffer->data[buffer->gap_end - 1] = buffer->data[buffer->gap_start - 1];
    buffer->gap_start--;
    buffer->gap_end--;

    return BUFFER_SUCCESS;
}

BufferStatus buffer_move_right(EditorBuffer *buffer) {
    if (buffer == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    if (buffer->gap_end == buffer->capacity) {
        return BUFFER_ERR_END_OF_BUFFER;
    }

    if (buffer->data[buffer->gap_end] == '\n') {
        return BUFFER_ERR_END_OF_LINE;
    }

    buffer->data[buffer->gap_start] = buffer->data[buffer->gap_end];
    buffer->gap_start++;
    buffer->gap_end++;

    return BUFFER_SUCCESS;
}

BufferStatus buffer_move_up(EditorBuffer *buffer, size_t preferred_column) {
    size_t current_line_start;
    size_t previous_line_end;
    size_t previous_line_start;
    size_t previous_line_length;
    size_t clamped_column;
    size_t target_position;

    if (buffer == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    current_line_start = buffer_find_line_start_before(buffer, buffer->gap_start);
    if (current_line_start == 0) {
        return BUFFER_ERR_FIRST_LINE;
    }

    previous_line_end = current_line_start - 1;
    previous_line_start = buffer_find_line_start_before(buffer, previous_line_end);
    previous_line_length = previous_line_end - previous_line_start;
    clamped_column = preferred_column;

    if (clamped_column > previous_line_length) {
        clamped_column = previous_line_length;
    }

    target_position = previous_line_start + clamped_column;
    return buffer_move_gap(buffer, target_position);
}

BufferStatus buffer_move_down(EditorBuffer *buffer, size_t preferred_column) {
    size_t current_line_end;
    size_t next_line_start;
    size_t next_line_end;
    size_t next_line_length;
    size_t clamped_column;
    size_t target_position;

    if (buffer == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    current_line_end = buffer_find_line_end_after(buffer, buffer->gap_end);
    if (current_line_end == buffer->capacity) {
        return BUFFER_ERR_LAST_LINE;
    }

    next_line_start = current_line_end + 1;
    next_line_end = buffer_find_line_end_after(buffer, next_line_start);
    next_line_length = next_line_end - next_line_start;
    clamped_column = preferred_column;

    if (clamped_column > next_line_length) {
        clamped_column = next_line_length;
    }

    target_position = next_line_start + clamped_column;
    return buffer_move_gap(buffer, target_position);
}

BufferStatus buffer_move_to_line_start(EditorBuffer *buffer) {
    size_t target_position;

    if (buffer == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    target_position = buffer_find_line_start_before(buffer, buffer->gap_start);
    return buffer_move_gap(buffer, target_position);
}

BufferStatus buffer_move_to_first_non_blank(EditorBuffer *buffer) {
    BufferStatus line_start_result;
    size_t target_position;

    if (buffer == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    line_start_result = buffer_move_to_line_start(buffer);
    if (line_start_result != BUFFER_SUCCESS) {
        return line_start_result;
    }

    target_position = buffer->gap_end;
    while (target_position < buffer->capacity &&
        buffer_is_blank_char(buffer->data[target_position])) {
        target_position++;
    }

    if (target_position >= buffer->capacity ||
        buffer->data[target_position] == '\n') {
        return BUFFER_SUCCESS;
    }

    return buffer_move_gap(buffer, target_position);
}

BufferStatus buffer_move_to_line_end(EditorBuffer *buffer) {
    size_t target_position;

    if (buffer == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    target_position = buffer_find_line_end_after(buffer, buffer->gap_end);
    return buffer_move_gap(buffer, target_position);
}

BufferStatus buffer_move_to_previous_word(EditorBuffer *buffer) {
    size_t target_position;

    if (buffer == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    if (buffer->gap_start == 0) {
        return BUFFER_SUCCESS;
    }

    target_position = buffer->gap_start - 1;
    while (target_position > 0 && buffer_is_space_char(buffer->data[target_position])) {
        target_position--;
    }

    switch (buffer_classify_char(buffer->data[target_position])) {
        case BUFFER_CHAR_KEYWORD:
            target_position = buffer_find_run_start(buffer,
                                                    target_position,
                                                    BUFFER_CHAR_KEYWORD);
            break;
        case BUFFER_CHAR_PUNCTUATION:
            target_position = buffer_find_run_start(buffer,
                                                    target_position,
                                                    BUFFER_CHAR_PUNCTUATION);
            break;
        case BUFFER_CHAR_SPACE:
        default:
            break;
    }

    return buffer_move_gap(buffer, target_position);
}

BufferStatus buffer_move_to_next_word(EditorBuffer *buffer) {
    BufferCharClass current_class;
    size_t target_position;

    if (buffer == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    target_position = buffer->gap_end;
    if (target_position >= buffer->capacity) {
        return BUFFER_SUCCESS;
    }

    current_class = buffer_classify_char(buffer->data[target_position]);
    if (current_class == BUFFER_CHAR_KEYWORD ||
        current_class == BUFFER_CHAR_PUNCTUATION) {
        target_position = buffer_advance_run(buffer, target_position, current_class);
    }

    while (target_position < buffer->capacity &&
        buffer_is_space_char(buffer->data[target_position])) {
        target_position++;
    }

    return buffer_move_gap(buffer, target_position);
}

BufferStatus buffer_move_to_word_end(EditorBuffer *buffer) {
    BufferCharClass current_class;
    BufferCharClass target_class;
    BufferStatus move_gap_result;
    size_t target_position;
    int is_word_class;
    int next_matches_class;

    if (buffer == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    target_position = buffer->gap_end;
    if (target_position < buffer->capacity) {
        current_class = buffer_classify_char(buffer->data[target_position]);
        is_word_class = current_class == BUFFER_CHAR_KEYWORD ||
            current_class == BUFFER_CHAR_PUNCTUATION;
        next_matches_class = target_position + 1 < buffer->capacity &&
            buffer_classify_char(buffer->data[target_position + 1]) == current_class;

        if (is_word_class && !next_matches_class) {
            target_position++;
        }
    }

    while (target_position < buffer->capacity &&
        buffer_is_space_char(buffer->data[target_position])) {
        target_position++;
    }

    if (target_position >= buffer->capacity) {
        return BUFFER_SUCCESS;
    }

    target_class = buffer_classify_char(buffer->data[target_position]);
    if (target_class == BUFFER_CHAR_KEYWORD ||
        target_class == BUFFER_CHAR_PUNCTUATION) {
        target_position = buffer_advance_run(buffer, target_position, target_class);
    }

    move_gap_result = buffer_move_gap(buffer, target_position);
    if (move_gap_result != BUFFER_SUCCESS) {
        return move_gap_result;
    }

    if (buffer->gap_start > 0 && buffer->data[buffer->gap_start - 1] != '\n') {
        return buffer_move_left(buffer);
    }

    return BUFFER_SUCCESS;
}

BufferStatus buffer_move_to_file_start(EditorBuffer *buffer) {
    if (buffer == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    return buffer_move_gap(buffer, 0);
}

BufferStatus buffer_move_to_file_end(EditorBuffer *buffer) {
    if (buffer == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    return buffer_move_gap(buffer, buffer->capacity);
}
