#include "editor.h"
#include "buffer.h"
#include <ncurses.h>

#define ESCAPE_ASCII_CODE 27

void process_normal_mode(Editor *editor, int ch) {
    if (ch =='q') {
        editor->is_running = 0;
    }

    if (ch == 'i') {
        editor->mode = MODE_INSERT;
    }

    if (ch == 'h') {
        if (buffer_move_left(&editor->buffer) == 0) {
            editor->preferred_column = buffer_get_column(&editor->buffer);
        }
    }

    if (ch == 'l') {
        if (buffer_move_right(&editor->buffer) == 0) {
            editor->preferred_column = buffer_get_column(&editor->buffer);
        }
    }

    if (ch == 'j') {
        buffer_move_down(&editor->buffer, editor->preferred_column);
    }

    if (ch == 'k') {
        buffer_move_up(&editor->buffer, editor->preferred_column);
    }

    if (ch == '0') {
        buffer_move_line_start(&editor->buffer);
        editor->preferred_column = buffer_get_column(&editor->buffer);
    }

    if (ch == '$') {
        buffer_move_line_end(&editor->buffer);

        if (editor->buffer.gap_start > 0 && editor->buffer.data[editor->buffer.gap_start - 1] != '\n') {
            buffer_move_left(&editor->buffer);
        }

        editor->preferred_column = buffer_get_column(&editor->buffer);
    }

    if (ch == 'w') {
        if (buffer_move_next_word(&editor->buffer) == 0) {
            editor->preferred_column = buffer_get_column(&editor->buffer);
        }
    }
}

void process_insert_mode(Editor *editor, int ch) {
    if (ch == ESCAPE_ASCII_CODE) {
        editor->mode = MODE_NORMAL;
    } else if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
        buffer_delete(&editor->buffer);
        editor->preferred_column = buffer_get_column(&editor->buffer);
    } else if (ch == KEY_LEFT) {
        if (buffer_move_left(&editor->buffer) == 0) {
            editor->preferred_column = buffer_get_column(&editor->buffer);
        }
    } else if (ch == KEY_RIGHT) {
        if (buffer_move_right(&editor->buffer) == 0) {
            editor->preferred_column = buffer_get_column(&editor->buffer);
        }
    } else if (ch == KEY_UP) {
        buffer_move_up(&editor->buffer, editor->preferred_column);
    } else if (ch == KEY_DOWN) {
        buffer_move_down(&editor->buffer, editor->preferred_column);
    } else if (ch >= 0 && ch <= 255) {
        buffer_insert(&editor->buffer, ch);
        editor->preferred_column = buffer_get_column(&editor->buffer);
    }
}
