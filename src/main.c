#include <ncurses.h>
#include <locale.h>
#include <stdio.h>

#include "editor.h"
#include "tui.h"

#define INITIAL_BUFFER_CAPACITY 10

int main(void) {
    BufferStatus init_result;
    setlocale(LC_ALL, "");

    Editor editor;
    init_result = editor_init(&editor, INITIAL_BUFFER_CAPACITY);
    if (init_result != BUFFER_SUCCESS) {
        fprintf(stderr, "Failed to initialize buffer\n");
        return 1;
    }

    tui_init();

    while (editor.is_running) {
        int ch;

        tui_render_editor(&editor);
        ch = getch();
        editor_handle_key(&editor, ch);
    }

    editor_destroy(&editor);
    tui_shutdown();

    return 0;
}
