#ifndef EDITOR_H
#define EDITOR_H

#include "buffer.h"

typedef enum EditorMode {
    MODE_NORMAL,
    MODE_INSERT,
} EditorMode;

typedef struct Editor {
    EditorBuffer buffer;
    EditorMode mode;
    int is_running;
    size_t preferred_column;
} Editor;

void process_normal_mode(Editor *editor, int ch);
void process_insert_mode(Editor *editor, int ch);

#endif
