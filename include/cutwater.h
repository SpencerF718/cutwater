#ifndef CUTWATER_H
#define CUTWATER_H

#include <gtk/gtk.h>

typedef struct {
  GtkWidget *window;
  GtkWidget *text_view;
  GtkTextBuffer *buffer;
  char *current_file;
  gboolean modified;
} CutwaterApp;

// App utilities
void update_window_title(CutwaterApp *app);
void cutwater_app_free(CutwaterApp *app);

// Markdown preview
void render_markdown_to_preview(GtkTextBuffer *src, GtkTextBuffer *dest);

// Callbacks
void on_buffer_changed(GtkTextBuffer *buffer, gpointer user_data);
void on_editor_changed(GtkTextBuffer *buffer, gpointer user_data);

#endif
