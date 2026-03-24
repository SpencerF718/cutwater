#ifndef TUI_H
#define TUI_H

#include "editor.h"

void tui_init(void);
void tui_shutdown(void);
void tui_render_editor(const Editor *editor);

#endif
