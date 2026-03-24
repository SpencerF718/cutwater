#ifndef BUFFER_INTERNAL_H
#define BUFFER_INTERNAL_H

#include "buffer.h"

BufferStatus buffer_grow(EditorBuffer *buffer);
BufferStatus buffer_move_gap(EditorBuffer *buffer, size_t target_position);

#endif
