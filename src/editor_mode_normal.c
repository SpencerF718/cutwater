#include <stddef.h>

#include <ncurses.h>

#include "internal/editor_internal.h"

static void editor_handle_pending_g_prefix(Editor *editor, int ch) {
    editor_clear_pending_motion_prefix(editor);

    if (ch == 'g') {
        (void)buffer_move_to_file_start(&editor->buffer);
        editor_sync_preferred_column(editor);
        return;
    }

    editor_signal_invalid_command();
}

static void editor_move_cursor_to_line_end(Editor *editor) {
    (void)buffer_move_to_line_end(&editor->buffer);
    editor_move_cursor_left_if_not_on_newline(editor);
    editor->preferred_column = (size_t)-1;
}

void editor_handle_normal_mode_key(Editor *editor, int ch) {
    if (editor->pending_motion_prefix == MOTION_PREFIX_G) {
        editor_handle_pending_g_prefix(editor, ch);
        return;
    }

    switch (ch) {
        case 'q':
            editor->is_running = 0;
            break;

        case 'i':
            editor->mode = MODE_INSERT;
            break;

        case 'h':
        case KEY_LEFT:
            if (buffer_move_left(&editor->buffer) == BUFFER_SUCCESS) {
                editor_sync_preferred_column(editor);
            }
            break;

        case 'l':
        case KEY_RIGHT:
            if (buffer_move_right(&editor->buffer) == BUFFER_SUCCESS) {
                editor_sync_preferred_column(editor);
            }
            break;

        case '0':
            (void)buffer_move_to_line_start(&editor->buffer);
            editor_sync_preferred_column(editor);
            break;

        case '^':
            (void)buffer_move_to_first_non_blank(&editor->buffer);
            editor_sync_preferred_column(editor);
            break;

        case '$':
            editor_move_cursor_to_line_end(editor);
            break;

        case 'e':
            if (buffer_move_to_word_end(&editor->buffer) == BUFFER_SUCCESS) {
                editor_sync_preferred_column(editor);
            }
            break;

        case 'w':
            if (buffer_move_to_next_word(&editor->buffer) == BUFFER_SUCCESS) {
                editor_sync_preferred_column(editor);
            }
            break;

        case 'b':
            if (buffer_move_to_previous_word(&editor->buffer) == BUFFER_SUCCESS) {
                editor_sync_preferred_column(editor);
            }
            break;

        case 'g':
            editor->pending_motion_prefix = MOTION_PREFIX_G;
            break;

        case 'G':
            (void)buffer_move_to_file_end(&editor->buffer);
            editor_sync_preferred_column(editor);
            break;

        case 'j':
        case KEY_DOWN:
            (void)buffer_move_down(&editor->buffer, editor->preferred_column);
            break;

        case 'k':
        case KEY_UP:
            (void)buffer_move_up(&editor->buffer, editor->preferred_column);
            break;

        default:
            break;
    }
}
