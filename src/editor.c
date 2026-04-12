#include <ncurses.h>
#include <stdint.h>

#include "internal/editor_internal.h"

BufferStatus editor_init(Editor *editor, size_t initial_capacity) {
    BufferStatus init_result;

    if (editor == NULL) {
        return BUFFER_ERR_INVALID_ARGUMENT;
    }

    init_result = buffer_init(&editor->buffer, initial_capacity);
    if (init_result != BUFFER_SUCCESS) {
        return init_result;
    }

    editor->mode = MODE_NORMAL;
    editor->is_running = 1;
    editor->preferred_column = 0;
    editor->pending_motion_prefix = MOTION_PREFIX_NONE;
    editor->pending_count = 0;

    return BUFFER_SUCCESS;
}

void editor_destroy(Editor *editor) {
    if (editor == NULL) {
        return;
    }

    (void)buffer_free(&editor->buffer);
}

void editor_handle_key(Editor *editor, int ch) {
    if (editor == NULL) {
        return;
    }

    switch (editor->mode) {
        case MODE_NORMAL:
            editor_handle_normal_mode_key(editor, ch);
            break;
        case MODE_INSERT:
            editor_handle_insert_mode_key(editor, ch);
            break;
    }
}

void editor_sync_preferred_column(Editor *editor) {
    editor->preferred_column = buffer_get_cursor_column(&editor->buffer);
}

void editor_clear_pending_count(Editor *editor) {
    if (editor == NULL) {
        return;
    }

    editor->pending_count = 0;
}

size_t editor_get_effective_count(const Editor *editor) {
    if (editor == NULL || editor->pending_count == 0) {
        return 1;
    }

    return editor->pending_count;
}

void editor_append_count_digit(Editor *editor, int digit) {
    if (editor == NULL || digit < 0 || digit > 9) {
        return;
    }

    if (editor->pending_count > (SIZE_MAX - (size_t)digit) / 10) {
        editor->pending_count = SIZE_MAX;
        return;
    }

    editor->pending_count = editor->pending_count * 10 + (size_t)digit;
}

void editor_clear_pending_motion_prefix(Editor *editor) {
    editor->pending_motion_prefix = MOTION_PREFIX_NONE;
}

void editor_clear_pending_command(Editor *editor) {
    if (editor == NULL) {
        return;
    }

    editor_clear_pending_motion_prefix(editor);
    editor_clear_pending_count(editor);
}

void editor_signal_invalid_command(void) {
    if (stdscr != NULL) {
        beep();
    }
}

void editor_move_cursor_left_if_not_on_newline(Editor *editor) {
    if (editor->buffer.gap_start > 0 &&
        editor->buffer.data[editor->buffer.gap_start - 1] != '\n') {
        (void)buffer_move_left(&editor->buffer);
    }
}
