#ifndef CUTWATER_H
#define CUTWATER_H

#include <gtk/gtk.h>
#include "vim.h"

// App structure
typedef struct {
  GtkWidget *window;
  GtkWidget *text_view;
  GtkTextBuffer *buffer;
  char *current_file;
  gboolean modified;
  VimState *vim_state;
} CutwaterApp;

// App utilities
void update_window_title(CutwaterApp *app);
void cutwater_app_free(CutwaterApp *app);

// Markdown preview
void setup_preview_tags(GtkTextBuffer *buffer);
void render_markdown_to_preview(GtkTextBuffer *src, GtkTextBuffer *dest);

// Callbacks
void on_buffer_changed(GtkTextBuffer *buffer, gpointer user_data);
void on_editor_changed(GtkTextBuffer *buffer, gpointer user_data);

#endif
