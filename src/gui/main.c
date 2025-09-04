#include <gtk/gtk.h>
#include "cutwater.h"
#include "file_io.h"
#include <string.h>

void update_window_title(CutwaterApp *app) {
  const char *filename = app->current_file ? app->current_file : "Untitled";
  char *title = g_strdup_printf("%s%s - Cutwater",
                                filename,
                                app->modified ? "*" : "");
  gtk_window_set_title(GTK_WINDOW(app->window), title);
  g_free(title);
}

static void render_markdown_to_preview(GtkTextBuffer *src, GtkTextBuffer *dest) {
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


static void on_buffer_changed(GtkTextBuffer *buffer, gpointer user_data) {
  CutwaterApp *app = user_data;
  if (!app->modified) {
    app->modified = TRUE;
    update_window_title(app);
  }
}

static void on_editor_changed(GtkTextBuffer *buffer, gpointer user_data) {
  GtkTextBuffer *preview_buffer = GTK_TEXT_BUFFER(user_data);
  render_markdown_to_preview(buffer, preview_buffer);
}

static void cutwater_app_free(CutwaterApp *app) {
  if (!app) return;
  g_free(app->current_file);
  g_free(app);
}

static void activate(GtkApplication *gapp, gpointer user_data) {
  CutwaterApp *app = g_new0(CutwaterApp, 1);

  GtkWidget *window;
  GtkWidget *paned;
  GtkWidget *scrolled_editor;
  GtkWidget *scrolled_preview;
  GtkWidget *text_view;
  GtkWidget *preview_view;

  window = gtk_application_window_new(gapp);
  gtk_window_set_default_size(GTK_WINDOW(window), 600, 500);

  // Split view
  paned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_window_set_child(GTK_WINDOW(window), paned);
 
  // Editor
  scrolled_editor = gtk_scrolled_window_new();
  text_view = gtk_text_view_new();
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_editor), text_view);

  scrolled_preview = gtk_scrolled_window_new();
  preview_view = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(preview_view), FALSE);
  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_preview), preview_view);

  gtk_paned_set_start_child(GTK_PANED(paned), scrolled_editor);
  gtk_paned_set_end_child(GTK_PANED(paned), scrolled_preview);

  app->window = window;
  app->text_view = text_view;
  app->buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
  app->current_file = NULL;
  app->modified = FALSE;

  GtkTextBuffer *preview_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(preview_view));

  GtkTextTagTable *tag_table = gtk_text_buffer_get_tag_table(preview_buffer);
  GtkTextTag *heading1 = gtk_text_tag_new("heading1");
  g_object_set(heading1,
               "weight", PANGO_WEIGHT_BOLD,
               "scale", 1.5, NULL);
  gtk_text_tag_table_add(tag_table, heading1);

  g_signal_connect(app->buffer, "changed", G_CALLBACK(on_buffer_changed), app);
  g_signal_connect(app->buffer, "changed", G_CALLBACK(on_editor_changed), preview_buffer);

  GtkCssProvider *provider = gtk_css_provider_new();
  GFile *css_file = g_file_new_for_path("../assets/cutwater.css");
  gtk_css_provider_load_from_file(provider, css_file);
  g_object_unref(css_file);

  gtk_style_context_add_provider_for_display(
    gdk_display_get_default(),
    GTK_STYLE_PROVIDER(provider),
    GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
  );
  g_object_unref(provider);

  const char *accels_open[] = { "<Ctrl>O", NULL };
  const char *accels_save[] = { "<Ctrl>S", NULL };
  gtk_application_set_accels_for_action(gapp, "app.open", accels_open);
  gtk_application_set_accels_for_action(gapp, "app.save", accels_save);

  g_action_map_add_action_entries(G_ACTION_MAP(gapp),
                                  (const GActionEntry[]) {
                                  { "open", open_file_action, NULL, NULL, NULL },
                                  { "save", save_file_action, NULL, NULL, NULL }
                                  },
                                  2, app);

  update_window_title(app);
  gtk_window_present(GTK_WINDOW(window));

  g_object_set_data_full(G_OBJECT(gapp), "cutwater-app", app,
                         (GDestroyNotify)cutwater_app_free);
}

int main(int argc, char **argv) {
  GtkApplication *gapp;
  int status;

  gapp = gtk_application_new("org.gtk.cutwater", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(gapp, "activate", G_CALLBACK(activate), NULL);

  status = g_application_run(G_APPLICATION(gapp), argc, argv);
  g_object_unref(gapp);

  return status;
}

