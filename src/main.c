#include <ncurses.h>
#include <locale.h>
#include <stdio.h>
#include "tui.h"
#include "buffer.h"

#define INITIAL_CAPACITY 10

int main (void) {
    setlocale(LC_ALL, "");

    EditorBuffer eb;
    if (buffer_init(&eb, INITIAL_CAPACITY) != 0) {
        fprintf(stderr, "Failed to initialize buffer\n");
        return 1;
    }

    char *text = "Hello World\n";
    for (int i = 0; text[i] != '\0'; i++) {
        buffer_insert(&eb, text[i]);
    }

    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);

    while (1) {
        clear();
        render_buffer(&eb);
        refresh();
        int ch = getch();

        if (ch == 'q') {
            break;
        }
    }

    endwin();
    return 0;
}
