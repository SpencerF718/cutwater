#include <ncurses.h>
#include <locale.h>

int main (void) {
    setlocale(LC_ALL, "");

    initscr();

    raw();
    noecho();
    keypad(stdscr, TRUE);

    printw("test test test 123 123 123");

    getch();

    endwin();
    return 0;
}
