#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "vim.h"
#include "cutwater.h"

static gboolean on_key_pressed(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_data);

void vim_init(CutwaterApp *app) {
  VimState *vim_state = g_new0(VimState, 1);
  vim_state->app = app;
  vim_state->mode = NORMAL_MODE;
  vim_state->saved_col = 0;

  app->vim_state = vim_state;
  GtkEventController *controller = gtk_event_controller_key_new();
  g_signal_connect(controller, "key-pressed", G_CALLBACK(on_key_pressed), app);
  gtk_widget_add_controller(app->text_view, controller);
}

static void update_saved_col(VimState *vim_state) {
  GtkTextIter iter;
  gtk_text_buffer_get_iter_at_mark(vim_state->app->buffer, &iter, gtk_text_buffer_get_insert(vim_state->app->buffer));
  vim_state->saved_col = gtk_text_iter_get_line_offset(&iter);
}

static gboolean on_key_pressed(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_data) {
  CutwaterApp *app = user_data;
  VimState *vim_state = app->vim_state;
  GtkTextBuffer *buffer = app->buffer;
  GtkTextIter iter;

  gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));

  if (vim_state->mode == NORMAL_MODE) {

    switch(keyval) {
      // -- Mode Switching --
      case GDK_KEY_i:
        vim_state->mode = INSERT_MODE;
        g_print("Switched to INSERT mode\n");
        update_saved_col(vim_state);
        break;

      // -- Character Movement --
      case GDK_KEY_h:
        if (gtk_text_iter_get_line_offset(&iter) > 0) {
          gtk_text_iter_backward_char(&iter);
        }
        update_saved_col(vim_state);
        break;
      case GDK_KEY_j:
        {
          gint line = gtk_text_iter_get_line(&iter);
          if (line < gtk_text_buffer_get_line_count(buffer) - 1) {
            gtk_text_iter_set_line(&iter, line + 1);

            GtkTextIter next_line_start;
            gtk_text_buffer_get_iter_at_line(buffer, &next_line_start, line + 1);
            GtkTextIter next_line_end = next_line_start;
            gtk_text_iter_forward_to_line_end(&next_line_end);
            gint next_line_length = gtk_text_iter_get_line_offset(&next_line_end);

            if (vim_state->saved_col > next_line_length) {
              iter = next_line_end;
            } else {
              gtk_text_iter_set_line_offset(&iter, vim_state->saved_col);
            }
          }
        }
        break;
      case GDK_KEY_k:
        {
          gint line = gtk_text_iter_get_line(&iter);
          if (line > 0) {
            gtk_text_iter_set_line(&iter, line - 1);

            GtkTextIter prev_line_start;
            gtk_text_buffer_get_iter_at_line(buffer, &prev_line_start, line - 1);
            GtkTextIter prev_line_end = prev_line_start;
            gtk_text_iter_forward_to_line_end(&prev_line_end);
            gint prev_line_length = gtk_text_iter_get_line_offset(&prev_line_end);

            if (vim_state->saved_col > prev_line_length) {
              iter = prev_line_end;
            } else {
              gtk_text_iter_set_line_offset(&iter, vim_state->saved_col);
            }
          }
        }
        break;
      case GDK_KEY_l:
        if (!gtk_text_iter_ends_line(&iter)) {
          gtk_text_iter_forward_char(&iter);
        }
        update_saved_col(vim_state);
        break;

      // -- Word Movement --
      case GDK_KEY_w:
        gtk_text_iter_forward_word_end(&iter);
        if (!gtk_text_iter_is_end(&iter)) {
          gtk_text_iter_forward_char(&iter);
        }
        update_saved_col(vim_state);
        break;
      case GDK_KEY_b:
        gtk_text_iter_backward_word_start(&iter);
        update_saved_col(vim_state);
        break;
      case GDK_KEY_e:
        gtk_text_iter_forward_word_end(&iter);
        update_saved_col(vim_state);
        break;

      // -- Line Movement --
      case GDK_KEY_0:
        gtk_text_iter_set_line_offset(&iter, 0);
        update_saved_col(vim_state);
        break;
      case GDK_KEY_dollar:
        gtk_text_iter_forward_to_line_end(&iter);
        update_saved_col(vim_state);
        break;

      default:
        break;
    }
    gtk_text_buffer_place_cursor(buffer, &iter);
    return TRUE;
  } else if (vim_state->mode == INSERT_MODE) {

    switch(keyval) {
      case GDK_KEY_Escape:
        vim_state->mode = NORMAL_MODE;
        g_print("Switched to NORMAL mode\n");
        return TRUE;
      default:
        return FALSE;
    }
  }
  return FALSE;
}

