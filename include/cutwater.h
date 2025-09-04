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

void update_window_title(CutwaterApp *app);

#endif

