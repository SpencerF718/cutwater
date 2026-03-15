#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include "buffer.h"

#define MIN_CAPACITY 1024

typedef enum BufferCharClass {
    BUFFER_CHAR_SPACE,
    BUFFER_CHAR_KEYWORD,
    BUFFER_CHAR_PUNCTUATION
} BufferCharClass;

static BufferStatus buffer_grow(EditorBuffer *eb);
static BufferStatus buffer_move_gap(EditorBuffer *eb, size_t target_position);
static size_t buffer_find_line_start_before(const EditorBuffer *eb, size_t position);
static size_t buffer_find_line_end_after(const EditorBuffer *eb, size_t position);
static BufferCharClass buffer_classify_char(char c);
static int buffer_is_keyword(char c);
static int buffer_is_blank(char c);
static int buffer_is_space(char c);
static size_t buffer_find_run_start(const EditorBuffer *eb, size_t position, BufferCharClass class);
static size_t buffer_advance_run(const EditorBuffer *eb, size_t position, BufferCharClass class);

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
    if (eb->capacity < MIN_CAPACITY) {
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

static size_t buffer_find_line_start_before(const EditorBuffer *eb, size_t position) {
    while (position > 0 && eb->data[position - 1] != '\n') {
        position--;
    }

    return position;
}

static size_t buffer_find_line_end_after(const EditorBuffer *eb, size_t position) {
    while (position < eb->capacity && eb->data[position] != '\n') {
        position++;
    }

    return position;
}

static int buffer_is_keyword(char c) {
    return isalnum((unsigned char)c) || c == '_';
}

static int buffer_is_blank(char c) {
    return c == ' ' || c == '\t';
}

static int buffer_is_space(char c) {
    return isspace((unsigned char)c);
}

static BufferCharClass buffer_classify_char(char c) {
    if (buffer_is_space(c)) {
        return BUFFER_CHAR_SPACE;
    }

    if (buffer_is_keyword(c)) {
        return BUFFER_CHAR_KEYWORD;
    }

    return BUFFER_CHAR_PUNCTUATION;
}

static size_t buffer_find_run_start(const EditorBuffer *eb, size_t position, BufferCharClass class) {
    while (position > 0 && buffer_classify_char(eb->data[position - 1]) == class) {
        position--;
    }

    return position;
}

static size_t buffer_advance_run(const EditorBuffer *eb, size_t position, BufferCharClass class) {
    while (position < eb->capacity && buffer_classify_char(eb->data[position]) == class) {
        position++;
    }

    return position;
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

size_t buffer_get_column(const EditorBuffer *eb) {
    size_t current_line_start = buffer_find_line_start_before(eb, eb->gap_start);
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

    size_t current_line_start = buffer_find_line_start_before(eb, eb->gap_start);

    if (current_line_start == 0) {
        return BUFFER_ERR_FIRST_LINE;
    }

    size_t previous_line_end = current_line_start - 1;
    size_t previous_line_start = buffer_find_line_start_before(eb, previous_line_end);
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

    size_t current_line_end = buffer_find_line_end_after(eb, eb->gap_end);

    if (current_line_end == eb->capacity) {
        return BUFFER_ERR_LAST_LINE;
    }

    size_t next_line_start = current_line_end + 1;
    size_t next_line_end = buffer_find_line_end_after(eb, next_line_start);
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

    size_t target_position = buffer_find_line_start_before(eb, eb->gap_start);
    return buffer_move_gap(eb, target_position);
}

BufferStatus buffer_move_line_first_non_blank(EditorBuffer *eb) {
    if (eb == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    BufferStatus line_start_result = buffer_move_line_start(eb);
    if (line_start_result != BUFFER_SUCCESS) {
        return line_start_result;
    }

    size_t target_position = eb->gap_end;

    while (target_position < eb->capacity && buffer_is_blank(eb->data[target_position])) {
        target_position++;
    }

    if (target_position >= eb->capacity || eb->data[target_position] == '\n') {
        return BUFFER_SUCCESS;
    }

    return buffer_move_gap(eb, target_position);
}

BufferStatus buffer_move_line_end(EditorBuffer *eb) {
    if (eb == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    size_t target_position = buffer_find_line_end_after(eb, eb->gap_end);
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

    switch (buffer_classify_char(eb->data[target_position])) {
        case BUFFER_CHAR_KEYWORD:
            target_position = buffer_find_run_start(eb, target_position, BUFFER_CHAR_KEYWORD);
            break;
        case BUFFER_CHAR_PUNCTUATION:
            target_position = buffer_find_run_start(eb, target_position, BUFFER_CHAR_PUNCTUATION);
            break;
        case BUFFER_CHAR_SPACE:
        default:
            break;
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

    BufferCharClass current_class = buffer_classify_char(eb->data[target_position]);

    if (current_class == BUFFER_CHAR_KEYWORD || current_class == BUFFER_CHAR_PUNCTUATION) {
        target_position = buffer_advance_run(eb, target_position, current_class);
    }

    while (target_position < eb->capacity && buffer_is_space(eb->data[target_position])) {
        target_position++;
    }

    return buffer_move_gap(eb, target_position);
}

BufferStatus buffer_move_word_end(EditorBuffer *eb) {
    if (eb == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    size_t target_position = eb->gap_end;

    if (target_position < eb->capacity) {
        BufferCharClass current_class = buffer_classify_char(eb->data[target_position]);
        int is_word_class = current_class == BUFFER_CHAR_KEYWORD || current_class == BUFFER_CHAR_PUNCTUATION;
        int next_matches_class = target_position + 1 < eb->capacity &&
            buffer_classify_char(eb->data[target_position + 1]) == current_class;

        if (is_word_class && !next_matches_class) {
            target_position++;
        }
    }

    while (target_position < eb->capacity && buffer_is_space(eb->data[target_position])) {
        target_position++;
    }

    if (target_position >= eb->capacity) {
        return BUFFER_SUCCESS;
    }

    BufferCharClass target_class = buffer_classify_char(eb->data[target_position]);

    if (target_class == BUFFER_CHAR_KEYWORD || target_class == BUFFER_CHAR_PUNCTUATION) {
        target_position = buffer_advance_run(eb, target_position, target_class);
    }

    BufferStatus move_gap_result = buffer_move_gap(eb, target_position);
    if (move_gap_result != BUFFER_SUCCESS) {
        return move_gap_result;
    }

    if (eb->gap_start > 0 && eb->data[eb->gap_start - 1] != '\n') {
        return buffer_move_left(eb);
    }

    return BUFFER_SUCCESS;
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
