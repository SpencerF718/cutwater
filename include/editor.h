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
} Editor;

#endif
