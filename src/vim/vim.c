#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include "vim.h"
#include "cutwater.h"

static gboolean on_key_pressed(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_date);

void vim_init(CutwaterApp *app) {
  VimState *vim_state = g_new0(VimState, 1);
  vim_state->app = app;
  vim_state->mode = NORMAL_MODE;

  app->vim_state = vim_state;
  GtkEventController *controller = gtk_event_controller_key_new();
  g_signal_connect(controller, "key-pressed", G_CALLBACK(on_key_pressed), app);
  gtk_widget_add_controller(app->text_view, controller);
}

static gboolean on_key_pressed(GtkEventControllerKey *controller, guint keyval, guint keycode, GdkModifierType state, gpointer user_data) {
  CutwaterApp *app = user_data;
  VimState *vim_state = app->vim_state;
  GtkTextBuffer *buffer = app->buffer;
  GtkTextIter iter;
  
  gtk_text_buffer_get_iter_at_mark(buffer, &iter, gtk_text_buffer_get_insert(buffer));

  if (vim_state->mode == NORMAL_MODE) {

    switch(keyval) {
      case GDK_KEY_i:
        vim_state->mode = INSERT_MODE;
        g_print("Switched to INSERT mode\n");
        break;
      case GDK_KEY_h:
        gtk_text_iter_backward_char(&iter);
        gtk_text_buffer_place_cursor(buffer, &iter);
        break;
        case GDK_KEY_j:
        gtk_text_iter_forward_line(&iter);
        gtk_text_buffer_place_cursor(buffer, &iter);
        break;
      case GDK_KEY_k:
        gtk_text_iter_backward_line(&iter);
        gtk_text_buffer_place_cursor(buffer, &iter);
        break;
      case GDK_KEY_l:
        gtk_text_iter_forward_char(&iter);
        gtk_text_buffer_place_cursor(buffer, &iter);
        break;
      default:
        break;
    }
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
