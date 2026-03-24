#ifndef EDITOR_INTERNAL_H
#define EDITOR_INTERNAL_H

#include "editor.h"

void editor_sync_preferred_column(Editor *editor);
void editor_clear_pending_motion_prefix(Editor *editor);
void editor_signal_invalid_command(void);
void editor_move_cursor_left_if_not_on_newline(Editor *editor);
void editor_handle_normal_mode_key(Editor *editor, int ch);
void editor_handle_insert_mode_key(Editor *editor, int ch);

#endif
