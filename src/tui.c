#include "tui.h"

void render_buffer(EditorBuffer *eb) {
    for (size_t i = 0; i < eb->capacity; i++) {
        if (i < eb->gap_start || i >= eb->gap_end) {
            addch(eb->data[i]);
        }
    }
}

