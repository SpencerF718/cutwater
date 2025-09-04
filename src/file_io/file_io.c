#include "file_io.h"
#include <glib.h>

static void open_file_response(GObject *source, GAsyncResult *res, gpointer user_data) {
  CutwaterApp *app = user_data;
  GtkFileDialog *dialog = GTK_FILE_DIALOG(source);
  GError *error = NULL;
  GFile *file = gtk_file_dialog_open_finish(dialog, res, &error);

  if (!file) {
    if (error) {
      g_warning("Open file failed: %s", error->message);
      g_error_free(error);
    }
    return;
  }

  char *contents;
  gsize length;
  if (g_file_load_contents(file, NULL, &contents, &length, NULL, &error)) {
    gtk_text_buffer_set_text(app->buffer, contents, length);
    g_free(contents);

    g_free(app->current_file);
    app->current_file = g_file_get_path(file);
    app->modified = FALSE;
    update_window_title(app);
  } else {
    g_warning("Failed to load file: %s", error->message);
    g_error_free(error);
  }

  g_object_unref(file);
}

static void save_file_response(GObject *source, GAsyncResult *res, gpointer user_data) {
  CutwaterApp *app = user_data;
  GtkFileDialog *dialog = GTK_FILE_DIALOG(source);
  GError *error = NULL;
  GFile *file = gtk_file_dialog_save_finish(dialog, res, &error);

  if (!file) {
    if (error) {
      g_warning("Save file dialog failed: %s", error->message);
      g_error_free(error);
    }
    return;
  }

  g_free(app->current_file);
  app->current_file = g_file_get_path(file);
  g_object_unref(file);

  save_file(app);
}

void save_file(CutwaterApp *app) {
  if (!app->current_file) {
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_save(dialog, GTK_WINDOW(app->window), NULL,
                         save_file_response, app);
    return;
  }

  GtkTextIter start, end;
  gtk_text_buffer_get_bounds(app->buffer, &start, &end);
  char *text = gtk_text_buffer_get_text(app->buffer, &start, &end, FALSE);

  GError *error = NULL;
  if (!g_file_set_contents(app->current_file, text, -1, &error)) {
    g_warning("Failed to save: %s", error->message);
    g_error_free(error);
  } else {
    app->modified = FALSE;
  }
  g_free(text);

  update_window_title(app);
}

void open_file_action(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  CutwaterApp *app = user_data;
  GtkFileDialog *dialog = gtk_file_dialog_new();
  gtk_file_dialog_open(dialog, GTK_WINDOW(app->window), NULL, open_file_response, app);
}

void save_file_action(GSimpleAction *action, GVariant *parameter, gpointer user_data) {
  CutwaterApp *app = (CutwaterApp *)user_data;
  save_file(app);
}

