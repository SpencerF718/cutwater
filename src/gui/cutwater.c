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

void setup_preview_tags(GtkTextBuffer *buffer) {
  GtkTextTagTable *table = gtk_text_buffer_get_tag_table(buffer);

  for (int i = 1; i <= 6; i++) {
    char name[8];
    sprintf(name, "h%d", i);
    GtkTextTag *tag = gtk_text_tag_new(name);

    double scale = 2.0 - ((i-1) * 0.2);
    g_object_set(tag, "weight", PANGO_WEIGHT_BOLD, "scale", scale, NULL);
    gtk_text_tag_table_add(table, tag);
  }

  // Bold
  GtkTextTag *bold = gtk_text_tag_new("bold");
  g_object_set(bold, "weight", PANGO_WEIGHT_BOLD, NULL);
  gtk_text_tag_table_add(table, bold);

  // Italic
  GtkTextTag *italic = gtk_text_tag_new("italic");
  g_object_set(italic, "style", PANGO_STYLE_ITALIC, NULL);
  gtk_text_tag_table_add(table, italic);

  // Code block
  GtkTextTag *code = gtk_text_tag_new("code");
  g_object_set(code, "family", "monospace", NULL);
  GdkRGBA bg, fg;
 
  // TODO: Make sure to fix these colors later.
  gdk_rgba_parse(&bg, "#3c3c3c");
  gdk_rgba_parse(&fg, "#dcdcaa");
  g_object_set(code,
               "background-rgba", &bg,
               "foreground-rgba", &fg,
               NULL);

  gtk_text_tag_table_add(table, code);
}

void render_markdown_to_preview(GtkTextBuffer *src, GtkTextBuffer *dest) {
  GtkTextIter start, end;
  gtk_text_buffer_get_start_iter(src, &start);
  gtk_text_buffer_get_end_iter(src, &end);

  char *text = gtk_text_buffer_get_text(src, &start, &end, FALSE);
  gtk_text_buffer_set_text(dest, "", -1);

  GtkTextIter iter;
  gtk_text_buffer_get_start_iter(dest, &iter);

  const char *p = text;
  while (*p) {
    int heading_level = 0;
    while (*p == '#') {
      heading_level++;
      p++;
    }

    if (heading_level > 0 && heading_level <= 6 && *p == ' ') {
      // Heading
      p++;
      const char *line_end = strchr(p, '\n');
      if (!line_end) line_end = p + strlen(p);
      char *line = strndup(p, line_end - p);

      char tag_name[8];
      sprintf(tag_name, "h%d", heading_level);

      gtk_text_buffer_insert_with_tags_by_name(dest, &iter, line, line_end - p, tag_name, NULL);
      gtk_text_buffer_insert(dest, &iter, "\n", -1);

      free(line);
      p = line_end;
      // Inline
    } else {
      const char *line_end = strchr(p, '\n');
      if (!line_end) line_end = p + strlen(p);

      const char *cursor = p;
      while (cursor < line_end) {
        // Bold
        if (cursor[0] == '*' && cursor[1] == '*') {
          cursor += 2;
          const char *bold_end = strstr(cursor, "**");

          if (!bold_end) bold_end = line_end;
          gtk_text_buffer_insert_with_tags_by_name(dest, &iter, cursor, bold_end - cursor, "bold", NULL);

          cursor = bold_end + (bold_end < line_end ? 2 : 0);

          // Italic
        } else if (cursor[0] == '*') {
          cursor++;
          const char *italic_end = strchr(cursor, '*');

          if (!italic_end) italic_end = line_end;
          gtk_text_buffer_insert_with_tags_by_name(dest, &iter, cursor, italic_end - cursor, "italic", NULL);

          cursor = italic_end + (italic_end < line_end ? 1 : 0);

        } else if (cursor[0] == '`') {
          cursor++;
          const char *code_end = strchr(cursor, '`');

          if (!code_end || code_end > line_end) code_end = line_end;
          gtk_text_buffer_insert_with_tags_by_name(dest, &iter,
                                                   cursor,
                                                   code_end - cursor,
                                                   "code", NULL);
          cursor = code_end + ((code_end < line_end) ? 1 : 0);

          // Plain text
        } else {
          const char *next = strchr(cursor, '*');
          if (!next || next > line_end) next = line_end;
          gtk_text_buffer_insert(dest, &iter, cursor, next - cursor);
          cursor = next;
        }
      }

      gtk_text_buffer_insert(dest, &iter, "\n", -1);
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

