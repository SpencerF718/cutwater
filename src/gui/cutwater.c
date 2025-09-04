#include "cutwater.h"
#include <string.h>
#include <stdlib.h>

void update_window_title(CutwaterApp *app) {
  const char *filename = app->current_file ? app->current_file : "Untitled";
  char *title = g_strdup_printf("%s%s - Cutwater",
                                filename,
                                app->modified ? "*" : "");
  gtk_window_set_title(GTK_WINDOW(app->window), title);
  g_free(title);
}

void cutwater_app_free(CutwaterApp *app) {
  if (!app) return;
  g_free(app->current_file);
  g_free(app);
}

void render_markdown_to_preview(GtkTextBuffer *src, GtkTextBuffer *dest) {
  GtkTextIter start, end;
  gtk_text_buffer_get_start_iter(src, &start);
  gtk_text_buffer_get_end_iter(src, &end);

  char *text = gtk_text_buffer_get_text(src, &start, &end, FALSE);

  gtk_text_buffer_set_text(dest, "", -1);

  GtkTextIter dest_iter;
  gtk_text_buffer_get_start_iter(dest, &dest_iter);

  const char *p = text;

  while (*p) {
    if (g_str_has_prefix(p, "# ")) {
      p += 2;
      const char *line_end = strchr(p, '\n');
      if (!line_end) line_end = p + strlen(p);

      gtk_text_buffer_insert_with_tags_by_name(dest,
                                               &dest_iter, p, line_end - p, "heading1", NULL);
      gtk_text_buffer_insert(dest, &dest_iter, "\n", -1);
      p = line_end;
    } else {
      const char *line_end = strchr(p, '\n');
      if (!line_end) line_end = p + strlen(p);

      gtk_text_buffer_insert(dest, &dest_iter, p, line_end - p);
      gtk_text_buffer_insert(dest, &dest_iter, "\n", -1);
      p = line_end;
    }

    if (*p == '\n') p++;
  }

  g_free(text);
}

void on_buffer_changed(GtkTextBuffer *buffer, gpointer user_data) {
  CutwaterApp *app = user_data;
  if (!app->modified) {
    app->modified = TRUE;
    update_window_title(app);
  }
}

void on_editor_changed(GtkTextBuffer *buffer, gpointer user_data) {
  GtkTextBuffer *preview_buffer = GTK_TEXT_BUFFER(user_data);
  render_markdown_to_preview(buffer, preview_buffer);
}

