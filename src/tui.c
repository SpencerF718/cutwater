#include <ncurses.h>
#include <stddef.h>
#include "tui.h"

void render_buffer(EditorBuffer *eb) {
    int cursor_y = -1;
    int cursor_x = -1;

    move(0, 0);

    for (size_t i = 0; i < eb->gap_start; i++) {
        addch(eb->data[i]);
    }

    getyx(stdscr, cursor_y, cursor_x);

    for (size_t i = eb->gap_end; i < eb->capacity; i++) {
        addch(eb->data[i]);
    }

    move(cursor_y, cursor_x);
}

