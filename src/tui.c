#include <ncurses.h>
#include <stddef.h>
#include "tui.h"

void render_buffer(EditorBuffer *eb) {
    int cursor_y = 0, cursor_x = 0, y = 0, x = 0;

    for (size_t i = 0; i < eb->capacity; i++) {
        if (i == eb->gap_start) {
            cursor_y = y;
            cursor_x = x;
        }

        if (i < eb->gap_start || i >= eb->gap_end) {
            addch(eb->data[i]);

            if (eb->data[i] == '\n') {
                y++;
                x = 0;
            } else {
                x++;
            }
       }
    }
    move(cursor_y, cursor_x);
}

