#include <ncurses.h>
#include <locale.h>
#include <stdio.h>
#include "tui.h"
#include "buffer.h"
#include "editor.h"

#define INITIAL_CAPACITY 10
#define ESCAPE_ASCII_CODE 27

int main (void) {
    setlocale(LC_ALL, "");

    Editor editor;
    if (buffer_init(&editor.buffer, INITIAL_CAPACITY) != 0) {
        fprintf(stderr, "Failed to initialize buffer\n");
        return 1;
    }

    editor.mode = MODE_NORMAL;

    char *text = "Hello World\n";
    for (int i = 0; text[i] != '\0'; i++) {
        int result = buffer_insert(&editor.buffer, text[i]);

        if (result != 0) {
            fprintf(stderr, "Failed to insert character '%c'. ERROR: %d\n", text[i], result);
            buffer_free(&editor.buffer);
            return 1;
        }
    }

    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);

    int running = 1;

    while (running) {
        clear();
        render_buffer(&editor.buffer);
        refresh();
        int ch = getch();

        switch (editor.mode) {
            case MODE_NORMAL:
                if (ch =='q') {
                    running = 0;
                }

                if (ch == 'i') {
                    editor.mode = MODE_INSERT;
                }

                if (ch == 'h') {
                    buffer_move_left(&editor.buffer);
                }

                if (ch == 'l') {
                    buffer_move_right(&editor.buffer);
                }

                if (ch == 'j') {
                    buffer_move_down(&editor.buffer);
                }

                if (ch == 'k') {
                    buffer_move_up(&editor.buffer);
                }

                break;
            case MODE_INSERT:
                if (ch == ESCAPE_ASCII_CODE) {
                    editor.mode = MODE_NORMAL;
                } else {
                    buffer_insert(&editor.buffer, ch);
                }

                break;
        }
    }

    buffer_free(&editor.buffer);
    endwin();
    return 0;
}
