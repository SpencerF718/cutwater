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

  // Strikethrough
  GtkTextTag *strike = gtk_text_tag_new("strikethrough");
  g_object_set(strike, "strikethrough", TRUE, NULL);
  gtk_text_tag_table_add(table, strike);

  // Code inline
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

  // Codeblock
  GtkTextTag *codeblock = gtk_text_tag_new("codeblock");
  g_object_set(codeblock, "family", "monospace", NULL);

  GdkRGBA cb_bg, cb_fg;
  gdk_rgba_parse(&cb_bg, "#3c3c3c");
  gdk_rgba_parse(&cb_fg, "#dcdcaa");
  g_object_set(codeblock,
               "background-rgba", &cb_bg,
               "foreground-rgba", &cb_fg,
               NULL);

  gtk_text_tag_table_add(table, codeblock);

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
  gboolean in_codeblock = FALSE;

  while (*p) {
    const char *line_end = strchr(p, '\n');
    if (!line_end) line_end = p + strlen(p);

    if (strncmp(p, "```", 3) == 0) {
      in_codeblock = !in_codeblock;
      p = (*line_end == '\n') ? line_end + 1 : line_end;
      continue;
    }

    if (in_codeblock) {
      gtk_text_buffer_insert_with_tags_by_name(dest, &iter,
                                               p, line_end - p,
                                               "codeblock", NULL);
      gtk_text_buffer_insert(dest, &iter, "\n", -1);
      p = (*line_end == '\n') ? line_end + 1 : line_end;
      continue;
    }

    int heading_level = 0;
    const char *cursor = p;
    while (*cursor == '#') {
      heading_level++;
      cursor++;
    }

    if (heading_level > 0 && heading_level <= 6 && *cursor == ' ') {
      cursor++;
      char *line = strndup(cursor, line_end - cursor);

      char tag_name[8];
      sprintf(tag_name, "h%d", heading_level);

      gtk_text_buffer_insert_with_tags_by_name(dest, &iter,
                                               line,
                                               line_end - cursor,
                                               tag_name, NULL);
      gtk_text_buffer_insert(dest, &iter, "\n", -1);

      free(line);
    } else {
      const char *inline_cursor = p;
      while (inline_cursor < line_end) {
        if (inline_cursor[0] == '*' && inline_cursor[1] == '*') {
          inline_cursor += 2;
          const char *bold_end = strstr(inline_cursor, "**");
          if (!bold_end || bold_end > line_end) bold_end = line_end;
          gtk_text_buffer_insert_with_tags_by_name(dest, &iter,
                                                   inline_cursor,
                                                   bold_end - inline_cursor,
                                                   "bold", NULL);
          inline_cursor = bold_end + ((bold_end < line_end) ? 2 : 0);

        } else if (inline_cursor[0] == '*') {
          inline_cursor++;
          const char *italic_end = strchr(inline_cursor, '*');
          if (!italic_end || italic_end > line_end) italic_end = line_end;
          gtk_text_buffer_insert_with_tags_by_name(dest, &iter,
                                                   inline_cursor,
                                                   italic_end - inline_cursor,
                                                   "italic", NULL);
          inline_cursor = italic_end + ((italic_end < line_end) ? 1 : 0);

        } else if (inline_cursor[0] == '~' && inline_cursor[1] == '~') {
          inline_cursor += 2;
          const char *strike_end = strstr(inline_cursor, "~~");
          if (!strike_end || strike_end > line_end) strike_end = line_end;
          gtk_text_buffer_insert_with_tags_by_name(dest, &iter,
                                                   inline_cursor,
                                                   strike_end - inline_cursor,
                                                   "strikethrough", NULL);
          inline_cursor = strike_end + ((strike_end < line_end) ? 2 : 0);

        } else if (inline_cursor[0] == '`') {
          inline_cursor++;
          const char *code_end = strchr(inline_cursor, '`');
          if (!code_end || code_end > line_end) code_end = line_end;
          gtk_text_buffer_insert_with_tags_by_name(dest, &iter,
                                                   inline_cursor,
                                                   code_end - inline_cursor,
                                                   "code", NULL);
          inline_cursor = code_end + ((code_end < line_end) ? 1 : 0);

        } else {
          const char *next = strpbrk(inline_cursor, "*`");
          if (!next || next > line_end) next = line_end;
          gtk_text_buffer_insert(dest, &iter,
                                 inline_cursor, next - inline_cursor);
          inline_cursor = next;
        }
      }
      gtk_text_buffer_insert(dest, &iter, "\n", -1);
    }

    p = (*line_end == '\n') ? line_end + 1 : line_end;
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

