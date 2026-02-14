#include <ncurses.h>
#include <locale.h>
#include <stdio.h>
#include "tui.h"
#include "buffer.h"
#include "editor.h"

#define INITIAL_CAPACITY 10
#define ESCAPE_DELAY_TIME 20

int main (void) {
    setlocale(LC_ALL, "");

    Editor editor;
    if (buffer_init(&editor.buffer, INITIAL_CAPACITY) != 0) {
        fprintf(stderr, "Failed to initialize buffer\n");
        return 1;
    }

    editor.mode = MODE_NORMAL;
    editor.preferred_column = 0;

    initscr();
    set_escdelay(ESCAPE_DELAY_TIME);
    raw();
    noecho();
    keypad(stdscr, TRUE);

    editor.is_running = 1;

    while (editor.is_running) {
        clear();
        render_buffer(&editor.buffer);
        refresh();
        int ch = getch();

        switch (editor.mode) {
            case MODE_NORMAL:
                process_normal_mode(&editor, ch);
                break;
            case MODE_INSERT:
                process_insert_mode(&editor, ch);
                break;
        }
    }

    buffer_free(&editor.buffer);
    endwin();
    return 0;
}
