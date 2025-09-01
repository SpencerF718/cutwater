#include <gtk/gtk.h>

typedef struct {
    GtkWidget *window;
    GtkWidget *text_view;
    GtkTextBuffer *buffer;
    char *current_file;
    gboolean modified;
} CutwaterApp;

static void on_buffer_changed(GtkTextBuffer *buffer, gpointer user_data) {
    CutwaterApp *app = user_data;
    if (!app->modified) {
        app->modified = TRUE;
        gtk_window_set_title(GTK_WINDOW(app->window), "Cutwater*");
    }
}

static void open_file_response(GObject *source, GAsyncResult *res, gpointer user_data) {
    CutwaterApp *app = user_data;
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source);
    GFile *file = gtk_file_dialog_open_finish(dialog, res, NULL);

    if (file) {
        char *contents;
        gsize length;
        if (g_file_load_contents(file, NULL, &contents, &length, NULL, NULL)) {
            gtk_text_buffer_set_text(app->buffer, contents, length);
            g_free(contents);

            if (app->current_file) g_free(app->current_file);
            app->current_file = g_file_get_path(file);
            gtk_window_set_title(GTK_WINDOW(app->window), app->current_file);

            app->modified = FALSE;
        }
        g_object_unref(file);
    }
}

static void save_file(CutwaterApp *app);

static void save_file_response(GObject *source, GAsyncResult *res, gpointer user_data) {
    CutwaterApp *app = user_data;
    GtkFileDialog *dialog = GTK_FILE_DIALOG(source);
    GFile *file = gtk_file_dialog_save_finish(dialog, res, NULL);

    if (file) {
        if (app->current_file) g_free(app->current_file);
        app->current_file = g_file_get_path(file);
        g_object_unref(file);

        gtk_window_set_title(GTK_WINDOW(app->window), app->current_file);
        save_file(app);
    }
}

static void save_file(CutwaterApp *app) {
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
    g_file_set_contents(app->current_file, text, -1, &error);
    g_free(text);

    if (error) {
        g_warning("Failed to save: %s", error->message);
        g_error_free(error);
    } else {
        app->modified = FALSE;
        gtk_window_set_title(GTK_WINDOW(app->window), app->current_file);
    }
}

static void open_file_action(GSimpleAction *a, GVariant *p, gpointer user_data) {
    CutwaterApp *app = user_data;
    GtkFileDialog *dialog = gtk_file_dialog_new();
    gtk_file_dialog_open(dialog, GTK_WINDOW(app->window), NULL, open_file_response, app);
}

static void save_file_action(GSimpleAction *a, GVariant *p, gpointer user_data) {
    save_file((CutwaterApp *)user_data);
}

static void activate(GtkApplication *gapp, gpointer user_data) {
    CutwaterApp *app = g_new0(CutwaterApp, 1);

    GtkWidget *window;
    GtkWidget *scrolled_window;
    GtkWidget *text_view;

    window = gtk_application_window_new(gapp);
    gtk_window_set_title(GTK_WINDOW(window), "Cutwater");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 500);

    scrolled_window = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    text_view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled_window), text_view);
    gtk_window_set_child(GTK_WINDOW(window), scrolled_window);

    app->window = window;
    app->text_view = text_view;
    app->buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    app->current_file = NULL;
    app->modified = FALSE;

    g_signal_connect(app->buffer, "changed", G_CALLBACK(on_buffer_changed), app);

    // Keyboard shortcuts
    const char *accels_open[] = { "<Ctrl>O", NULL };
    const char *accels_save[] = { "<Ctrl>S", NULL };
    gtk_application_set_accels_for_action(gapp, "app.open", accels_open);
    gtk_application_set_accels_for_action(gapp, "app.save", accels_save);

    // Actions
    g_action_map_add_action_entries(G_ACTION_MAP(gapp),
        (const GActionEntry[]) {
            { "open", open_file_action, NULL, NULL, NULL },
            { "save", save_file_action, NULL, NULL, NULL }
        },
        2, app);

    gtk_window_present(GTK_WINDOW(window));
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

