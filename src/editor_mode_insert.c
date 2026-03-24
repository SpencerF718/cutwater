#include <ncurses.h>

#include "editor_internal.h"

#define EDITOR_ESCAPE_ASCII_CODE 27

static void editor_exit_insert_mode(Editor *editor) {
    editor->mode = MODE_NORMAL;
    editor_move_cursor_left_if_not_on_newline(editor);
    editor_sync_preferred_column(editor);
}

void editor_handle_insert_mode_key(Editor *editor, int ch) {
    switch (ch) {
        case EDITOR_ESCAPE_ASCII_CODE:
            editor_exit_insert_mode(editor);
            break;

        case KEY_BACKSPACE:
        case 127:
        case '\b':
            (void)buffer_delete_before_cursor(&editor->buffer);
            editor_sync_preferred_column(editor);
            break;

        case KEY_LEFT:
            if (buffer_move_left(&editor->buffer) == BUFFER_SUCCESS) {
                editor_sync_preferred_column(editor);
            }
            break;

        case KEY_RIGHT:
            if (buffer_move_right(&editor->buffer) == BUFFER_SUCCESS) {
                editor_sync_preferred_column(editor);
            }
            break;

        case KEY_UP:
            (void)buffer_move_up(&editor->buffer, editor->preferred_column);
            break;

        case KEY_DOWN:
            (void)buffer_move_down(&editor->buffer, editor->preferred_column);
            break;

        default:
            if (ch >= 0 && ch <= 255) {
                if (buffer_insert_char(&editor->buffer, (char)ch) == BUFFER_SUCCESS) {
                    editor_sync_preferred_column(editor);
                } else {
                    beep();
                }
            }
            break;
    }
}
