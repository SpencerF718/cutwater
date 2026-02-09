#include <ncurses.h>
#include <stddef.h>
#include "tui.h"

void render_buffer(EditorBuffer *eb) {
    int cursor_y = -1;
    int cursor_x = -1;

    move(0, 0);

    for (size_t i = 0; i < eb->capacity; i++) {
        if (i == eb->gap_start) {
            getyx(stdscr, cursor_y, cursor_x);
        }

        if (i < eb->gap_start || i >= eb->gap_end) {
            addch(eb->data[i]);
        }
    }

    if (cursor_y == -1) {
        getyx(stdscr, cursor_y, cursor_x);
    }

    move(cursor_y, cursor_x);
}

