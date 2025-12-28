#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "vim.h"
#include "cutwater.h"

static gboolean on_key_pressed(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_data);
static void execute_operator(VimState *vim_state, GtkTextIter *start, GtkTextIter *end);
static void delete_text(GtkTextBuffer *buffer, GtkTextIter *start, GtkTextIter *end);

void vim_init(CutwaterApp *app) {
  VimState *vim_state = g_new0(VimState, 1);
  vim_state->app = app;
  vim_state->mode = NORMAL_MODE;
  vim_state->saved_col = 0;
  vim_state->pending_operator = '\0';

  app->vim_state = vim_state;
  GtkEventController *controller = gtk_event_controller_key_new();
  g_signal_connect(controller, "key-pressed", G_CALLBACK(on_key_pressed), app);
  gtk_widget_add_controller(app->text_view, controller);
}

static void update_saved_col(VimState *vim_state, GtkTextIter *iter) {
  vim_state->saved_col = gtk_text_iter_get_line_offset(iter);
}

static void delete_text(GtkTextBuffer *buffer, GtkTextIter *start, GtkTextIter *end) {
  gtk_text_buffer_delete(buffer, start, end);
}

static void execute_operator(VimState *vim_state, GtkTextIter *start, GtkTextIter *end) {
  switch(vim_state->pending_operator) {
    case 'd':
      delete_text(vim_state->app->buffer, start, end);
      break;
  }
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
        update_saved_col(vim_state, &iter);
        break;
      case GDK_KEY_a:
        if (!gtk_text_iter_is_end(&iter)) {
          gtk_text_iter_forward_char(&iter);
        }
        vim_state->mode = INSERT_MODE;
        update_saved_col(vim_state, &iter);
        break;
      case GDK_KEY_A:
        gtk_text_iter_forward_to_line_end(&iter);
        vim_state->mode = INSERT_MODE;
        update_saved_col(vim_state, &iter);
        break;
      case GDK_KEY_o:
        gtk_text_iter_forward_to_line_end(&iter);
        gtk_text_buffer_insert(buffer, &iter, "\n", 1);
        vim_state->mode = INSERT_MODE;
        update_saved_col(vim_state, &iter);
        break;
      case GDK_KEY_O:
        gtk_text_iter_set_line_offset(&iter, 0);
        gtk_text_buffer_insert(buffer, &iter, "\n", 1);
        gtk_text_iter_backward_char(&iter);
        vim_state->mode = INSERT_MODE;
        update_saved_col(vim_state, &iter);
        break;

      // -- Operators --
      case GDK_KEY_d:
        vim_state->mode = OPERATOR_PENDING_MODE;
        vim_state->pending_operator = 'd';
        g_print("Operator pending: d\n");
        return TRUE;

      // -- Character Movement --
      case GDK_KEY_h:
        if (gtk_text_iter_get_line_offset(&iter) > 0) {
          gtk_text_iter_backward_char(&iter);
          update_saved_col(vim_state, &iter);
        }
        break;
      case GDK_KEY_j:
        {
          gint line = gtk_text_iter_get_line(&iter);
          if (line < gtk_text_buffer_get_line_count(buffer) - 1) {
            gtk_text_iter_set_line(&iter, line + 1);
            gint chars_on_next_line = gtk_text_iter_get_chars_in_line(&iter);

            if (chars_on_next_line == 0) {
              gtk_text_iter_set_line_offset(&iter, 0);
            } else if (vim_state->saved_col >= chars_on_next_line) {
              gtk_text_iter_set_line_offset(&iter, chars_on_next_line - 1);
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
            gint chars_on_prev_line = gtk_text_iter_get_chars_in_line(&iter);

            if (chars_on_prev_line == 0) {
              gtk_text_iter_set_line_offset(&iter, 0);
            } else if (vim_state->saved_col >= chars_on_prev_line) {
              gtk_text_iter_set_line_offset(&iter, chars_on_prev_line - 1);
            } else {
              gtk_text_iter_set_line_offset(&iter, vim_state->saved_col);
            }
          }
        }
        break;
      case GDK_KEY_l:
        if (!gtk_text_iter_ends_line(&iter)) {
          gtk_text_iter_forward_char(&iter);
          update_saved_col(vim_state, &iter);
        }
        break;

      // -- Word Movement --
      case GDK_KEY_w:
        gtk_text_iter_forward_word_end(&iter);
        if (!gtk_text_iter_is_end(&iter)) {
          gtk_text_iter_forward_char(&iter);
        }
        update_saved_col(vim_state, &iter);
        break;
      case GDK_KEY_b:
        gtk_text_iter_backward_word_start(&iter);
        update_saved_col(vim_state, &iter);
        break;
      case GDK_KEY_e:
        gtk_text_iter_forward_word_end(&iter);
        update_saved_col(vim_state, &iter);
        break;

      // -- Line Movement --
      case GDK_KEY_0:
        gtk_text_iter_set_line_offset(&iter, 0);
        update_saved_col(vim_state, &iter);
        break;
      case GDK_KEY_dollar:
        gtk_text_iter_forward_to_line_end(&iter);
        update_saved_col(vim_state, &iter);
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

  } else if (vim_state->mode == OPERATOR_PENDING_MODE) {

    GtkTextIter start_iter;
    gtk_text_buffer_get_iter_at_mark(buffer, &start_iter, gtk_text_buffer_get_insert(buffer));
    GtkTextIter end_iter = start_iter;

    switch(keyval) {
      case GDK_KEY_w:
        gtk_text_iter_forward_word_end(&end_iter);
        if (!gtk_text_iter_is_end(&end_iter)) {
          gtk_text_iter_forward_char(&end_iter);
        }
        execute_operator(vim_state, &start_iter, &end_iter);
        break;
      case GDK_KEY_d:
        gtk_text_iter_set_line_offset(&start_iter, 0);
        gtk_text_iter_forward_to_line_end(&end_iter);
        if (!gtk_text_iter_is_end(&end_iter)) {
          gtk_text_iter_forward_char(&end_iter);
        }
        execute_operator(vim_state, &start_iter, &end_iter);
        break;
      default:
        break;
    }
    vim_state->mode = NORMAL_MODE;
    vim_state->pending_operator = '\0';
    return TRUE;
  }
  return FALSE;
}

