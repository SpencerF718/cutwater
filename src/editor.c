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
        buffer_move_left(&editor->buffer);
    }

    if (ch == 'l') {
        buffer_move_right(&editor->buffer);
    }

    if (ch == 'j') {
        buffer_move_down(&editor->buffer);
    }

    if (ch == 'k') {
        buffer_move_up(&editor->buffer);
    }
}

void process_insert_mode(Editor *editor, int ch) {
    if (ch == ESCAPE_ASCII_CODE) {
        editor->mode = MODE_NORMAL;
    } else {
        buffer_insert(&editor->buffer, ch);
    }
}
