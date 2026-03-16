#ifndef EDITOR_H
#define EDITOR_H

#include "buffer.h"

typedef enum EditorMode {
    MODE_NORMAL,
    MODE_INSERT,
} EditorMode;

typedef enum EditorMotionPrefix {
    MOTION_PREFIX_NONE,
    MOTION_PREFIX_G,
} EditorMotionPrefix;

typedef struct Editor {
    EditorBuffer buffer;
    EditorMode mode;
    int is_running;
    size_t preferred_column;
    EditorMotionPrefix pending_motion_prefix;
} Editor;

void process_normal_mode(Editor *editor, int ch);
void process_insert_mode(Editor *editor, int ch);

#endif
