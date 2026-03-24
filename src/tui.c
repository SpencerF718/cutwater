#include <ncurses.h>
#include <stddef.h>

#include "tui.h"

#define TUI_ESCAPE_DELAY_TIME 5

static void tui_render_buffer(const EditorBuffer *buffer) {
    int cursor_y = -1;
    int cursor_x = -1;

    move(0, 0);

    for (size_t i = 0; i < buffer->gap_start; i++) {
        addch(buffer->data[i]);
    }

    getyx(stdscr, cursor_y, cursor_x);

    for (size_t i = buffer->gap_end; i < buffer->capacity; i++) {
        addch(buffer->data[i]);
    }

    move(cursor_y, cursor_x);
}

void tui_init(void) {
    initscr();
    set_escdelay(TUI_ESCAPE_DELAY_TIME);
    raw();
    noecho();
    keypad(stdscr, TRUE);
}

void tui_shutdown(void) {
    endwin();
}

void tui_render_editor(const Editor *editor) {
    clear();
    tui_render_buffer(&editor->buffer);
    refresh();
}
