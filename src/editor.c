#include "editor.h"
#include "buffer.h"

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
}

void process_insert_mode(Editor *editor, int ch) {
    if (ch == ESCAPE_ASCII_CODE) {
        editor->mode = MODE_NORMAL;
    } else {
        buffer_insert(&editor->buffer, ch);
        editor->preferred_column = buffer_get_column(&editor->buffer);
    }
}
