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

BufferStatus editor_init(Editor *editor, size_t initial_capacity);
void editor_destroy(Editor *editor);
void editor_handle_key(Editor *editor, int ch);

#endif
