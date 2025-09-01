#include <gtk/gtk.h>

static void activate (GtkApplication *app, gpointer user_data) {

  GtkWidget *window;
  GtkWidget *scrolled_window;
  GtkWidget *text_view;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Cutwater");
  gtk_window_set_default_size (GTK_WINDOW (window), 400, 400);

  scrolled_window = gtk_scrolled_window_new();
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                 GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  text_view = gtk_text_view_new();
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);

  gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), text_view);

  gtk_window_set_child(GTK_WINDOW(window), scrolled_window);

  gtk_window_present(GTK_WINDOW(window));

}

int main (int argc, char **argv) {
  
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.cutwater", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
  
}

