#include "editor.h"
#include "buffer.h"
#include <ncurses.h>

#define ESCAPE_ASCII_CODE 27

static void sync_column(Editor *editor) {
    editor->preferred_column = buffer_get_column(&editor->buffer);
}

void process_normal_mode(Editor *editor, int ch) {
    switch (ch) {
        case 'q':
            editor->is_running = 0;
            break;
        case 'i':
            editor->mode = MODE_INSERT;
            break;

        case 'h':
        case KEY_LEFT:
            if (buffer_move_left(&editor->buffer) == 0) sync_column(editor);
            break;
        case 'l':
        case KEY_RIGHT:
            if (buffer_move_right(&editor->buffer) == 0) sync_column(editor);
            break;
        case '0':
            buffer_move_line_start(&editor->buffer);
            sync_column(editor);
            break;
        case '$':
            buffer_move_line_end(&editor->buffer);
            if (editor->buffer.gap_start > 0 && editor->buffer.data[editor->buffer.gap_start - 1] != '\n') {
                buffer_move_left(&editor->buffer);
            }
            editor->preferred_column = (size_t) - 1;
            break;
        case 'w':
            if (buffer_move_next_word(&editor->buffer) == 0) sync_column(editor);
            break;

        case 'j':
        case KEY_DOWN:
            buffer_move_down(&editor->buffer, editor->preferred_column);
            break;
        case 'k':
        case KEY_UP:
            buffer_move_up(&editor->buffer, editor->preferred_column);
            break;

        default:
            break;
    }
}

void process_insert_mode(Editor *editor, int ch) {
    switch (ch) {
        case ESCAPE_ASCII_CODE:
            editor->mode = MODE_NORMAL;
            break;

        case KEY_BACKSPACE:
        case 127:
        case '\b':
            buffer_delete(&editor->buffer);
            sync_column(editor);
            break;

        case KEY_LEFT:
            if (buffer_move_left(&editor->buffer) == 0) sync_column(editor);
            break;
        case KEY_RIGHT:
            if (buffer_move_right(&editor->buffer) == 0) sync_column(editor);
            break;
        case KEY_UP:
            buffer_move_up(&editor->buffer, editor->preferred_column);
            break;
        case KEY_DOWN:
            buffer_move_down(&editor->buffer, editor->preferred_column);
            break;

        default:
            if (ch >= 0 && ch <= 255) {
                if (buffer_insert(&editor->buffer, ch) == BUFFER_SUCCESS) {
                    sync_column(editor);
                } else {
                    beep();
                }
            }
            break;
    }
}
