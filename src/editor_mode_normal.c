#include <stddef.h>

#include <ncurses.h>

#include "internal/editor_internal.h"

static void editor_repeat_buffer_motion(Editor *editor,
                                        size_t count,
                                        BufferStatus (*motion)(EditorBuffer *),
                                        int sync_preferred_column) {
    size_t i;
    int did_move = 0;

    for (i = 0; i < count; i++) {
        if (motion(&editor->buffer) != BUFFER_SUCCESS) {
            break;
        }

        did_move = 1;
    }

    if (sync_preferred_column && did_move) {
        editor_sync_preferred_column(editor);
    }
}

static void editor_repeat_vertical_motion(Editor *editor,
                                          size_t count,
                                          BufferStatus (*motion)(EditorBuffer *, size_t)) {
    size_t i;
    size_t preferred_column = editor->preferred_column;

    for (i = 0; i < count; i++) {
        if (motion(&editor->buffer, preferred_column) != BUFFER_SUCCESS) {
            break;
        }
    }
}

static void editor_move_to_absolute_line(Editor *editor, size_t line_number) {
    size_t i;

    (void)buffer_move_to_file_start(&editor->buffer);

    for (i = 1; i < line_number; i++) {
        if (buffer_move_down(&editor->buffer, 0) != BUFFER_SUCCESS) {
            break;
        }
    }

    editor->preferred_column = 0;
}

static void editor_move_to_first_non_blank_with_count(Editor *editor, size_t count) {
    if (count > 1) {
        editor_repeat_vertical_motion(editor, count - 1, buffer_move_down);
    }

    (void)buffer_move_to_first_non_blank(&editor->buffer);
    editor_sync_preferred_column(editor);
}

static void editor_move_cursor_to_line_end(Editor *editor) {
    (void)buffer_move_to_line_end(&editor->buffer);
    editor_move_cursor_left_if_not_on_newline(editor);
    editor->preferred_column = (size_t)-1;
}

static void editor_move_to_line_end_with_count(Editor *editor, size_t count) {
    if (count > 1) {
        editor_repeat_vertical_motion(editor, count - 1, buffer_move_down);
    }

    editor_move_cursor_to_line_end(editor);
}

static void editor_handle_pending_g_prefix(Editor *editor, int ch) {
    if (ch == 'g') {
        if (editor->pending_count > 0) {
            editor_move_to_absolute_line(editor, editor_get_effective_count(editor));
        } else {
            (void)buffer_move_to_file_start(&editor->buffer);
            editor_sync_preferred_column(editor);
        }

        editor_clear_pending_command(editor);
        return;
    }

    editor_clear_pending_command(editor);
    editor_signal_invalid_command();
}

void editor_handle_normal_mode_key(Editor *editor, int ch) {
    size_t count;
    int has_pending_count;

    if (editor->pending_motion_prefix == MOTION_PREFIX_G) {
        editor_handle_pending_g_prefix(editor, ch);
        return;
    }

    if (ch >= '1' && ch <= '9') {
        editor_append_count_digit(editor, ch - '0');
        return;
    }

    if (ch == '0' && editor->pending_count > 0) {
        editor_append_count_digit(editor, 0);
        return;
    }

    count = editor_get_effective_count(editor);
    has_pending_count = editor->pending_count > 0;

    switch (ch) {
        case 'q':
            if (has_pending_count) {
                editor_clear_pending_count(editor);
                editor_signal_invalid_command();
                break;
            }

            editor->is_running = 0;
            break;

        case 'i':
            if (has_pending_count) {
                editor_clear_pending_count(editor);
                editor_signal_invalid_command();
                break;
            }

            editor->mode = MODE_INSERT;
            break;

        case 'h':
        case KEY_LEFT:
            editor_repeat_buffer_motion(editor, count, buffer_move_left, 1);
            editor_clear_pending_count(editor);
            break;

        case 'l':
        case KEY_RIGHT:
            editor_repeat_buffer_motion(editor, count, buffer_move_right, 1);
            editor_clear_pending_count(editor);
            break;

        case '0':
            (void)buffer_move_to_line_start(&editor->buffer);
            editor_sync_preferred_column(editor);
            editor_clear_pending_count(editor);
            break;

        case '^':
            editor_move_to_first_non_blank_with_count(editor, count);
            editor_clear_pending_count(editor);
            break;

        case '$':
            editor_move_to_line_end_with_count(editor, count);
            editor_clear_pending_count(editor);
            break;

        case 'e':
            editor_repeat_buffer_motion(editor, count, buffer_move_to_word_end, 1);
            editor_clear_pending_count(editor);
            break;

        case 'w':
            editor_repeat_buffer_motion(editor, count, buffer_move_to_next_word, 1);
            editor_clear_pending_count(editor);
            break;

        case 'b':
            editor_repeat_buffer_motion(editor, count, buffer_move_to_previous_word, 1);
            editor_clear_pending_count(editor);
            break;

        case 'g':
            editor->pending_motion_prefix = MOTION_PREFIX_G;
            break;

        case 'G':
            if (has_pending_count) {
                editor_move_to_absolute_line(editor, count);
            } else {
                (void)buffer_move_to_file_end(&editor->buffer);
                editor_sync_preferred_column(editor);
            }

            editor_clear_pending_count(editor);
            break;

        case 'j':
        case KEY_DOWN:
            editor_repeat_vertical_motion(editor, count, buffer_move_down);
            editor_clear_pending_count(editor);
            break;

        case 'k':
        case KEY_UP:
            editor_repeat_vertical_motion(editor, count, buffer_move_up);
            editor_clear_pending_count(editor);
            break;

        default:
            if (has_pending_count) {
                editor_clear_pending_count(editor);
                editor_signal_invalid_command();
            }
            break;
    }
}
