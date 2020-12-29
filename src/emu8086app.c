/* gtkemu8086
 * Copyright 2020 KRC
 *
 * Permission is hereby granted, free of charge, to any person obtaining 
 * a copy of this software and associated documentation files (the 
 * "Software"), to deal in the Software without restriction, including 
 * without limitation the rights to use, copy, modify, merge, publish, 
 * distribute, sublicense, and/or sell copies of the Software, and to 
 * permit persons to whom the Software is furnished to do so, subject 
 * to the following conditions: 
 *
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software. 
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE. 
 *
 * (i.e. the MIT License)
 *
 * code.c
 * General emulation functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <emu8086app.h>
#include <emu8086win.h>
#include <emu8086aboutwin.h>
#include <emu8086appprefs.h>

struct _Emu8086App
{
    GtkApplication parent;
};
typedef struct _Emu8086AppPrivate Emu8086AppPrivate;

struct _Emu8086AppPrivate
{
    Emu8086AppWindow *win;
    GSettings *settings;
    gint to;
};

G_DEFINE_TYPE_WITH_PRIVATE(Emu8086App, emu_8086_app, GTK_TYPE_APPLICATION);

static void
emu_8086_app_init(Emu8086App *app)
{

    Emu8086AppPrivate *priv;
    // GApplication *application = g_application_new("com.krc.emu8086app", G_APPLICATION_NON_UNIQUE );
    // gboolean g = g_application_register(application, NULL, NULL);
    // g_print("%d", g);
    priv = emu_8086_app_get_instance_private(app);
    priv->settings = g_settings_new("com.krc.emu8086app");
}
void user_function3(GtkApplication *application,
                    GtkWindow *window,
                    gpointer user_data)
{
    g_print("hhh\n");
}
static void emu_8086_app_open(GApplication *app, GFile **files,
                              gint n_files,
                              const gchar *hint)
{
    GList *windows;
    Emu8086AppWindow *win;
    int i;
    windows = gtk_application_get_windows(GTK_APPLICATION(app));
    if (windows)
        win = EMU_8086_APP_WINDOW(windows->data);
    else
    {

        win = emu_8086_app_window_new(EMU_8086_APP(app));
    }
    // exit(0);

    gtk_window_present(GTK_WINDOW(win));
    g_signal_connect(app, "window-removed", G_CALLBACK(user_function3), NULL);
}

//

static void
emu_8086_activate(GApplication *app)
{
    Emu8086AppWindow *win;
    _PRIV;
    win = emu_8086_app_window_new(EMU_8086_APP(app));
    priv->win = win;
    emu_8086_app_window_up(win);
    emu_8086_app_window_set_app(win, app);
    gtk_window_present(GTK_WINDOW(win));
}
static void
emu_8086_open(GApplication *app,
              GFile **files,
              gint n_files,
              const gchar *hint)
{
    GList *windows;
    Emu8086AppWindow *win; // *win;
    int i;
    _PRIV;
    windows = gtk_application_get_windows(GTK_APPLICATION(app));
    if (windows)
        win = EMU_8086_APP_WINDOW(windows->data);
    else
    {
        win = emu_8086_app_window_new(EMU_8086_APP(app));
        emu_8086_app_window_set_app(win, app);
    }

    for (i = 0; i < n_files; i++)
        emu_8086_app_window_open(win, files[i]);
    priv->win = win;
    gtk_window_present(GTK_WINDOW(win));
}

static void
quit_activated(GSimpleAction *action,
               GVariant *parameter,
               gpointer app)
{
    g_application_quit(G_APPLICATION(app));
}

static void
open_activated(GSimpleAction *action,
               GVariant *parameter,
               gpointer appe)
{
    Emu8086App *app = EMU_8086_APP(appe);
    _PRIV;
    open_activate_cb(priv->win);
}
static void
save_activated(GSimpleAction *action,
               GVariant *parameter,
               gpointer appe)
{
    Emu8086App *app = EMU_8086_APP(appe);
    _PRIV;
    save_activate_cb(priv->win);
}

static void
save_as_activated(GSimpleAction *action,
                  GVariant *parameter,
                  gpointer appe)
{
    Emu8086App *app = EMU_8086_APP(appe);
    _PRIV;
    save_as_activate_cb(priv->win);
}

static void
help_activated(GSimpleAction *action,
               GVariant *parameter,
               gpointer appe)
{
    Emu8086App *app = EMU_8086_APP(appe);
    open_help();
}

static void
ex1_activated(GSimpleAction *action,
              GVariant *parameter,
              gpointer appe)
{
    Emu8086App *app = EMU_8086_APP(appe);

    _PRIV;

    arr_sum_activate_cb(priv->win);
}

static void
ex2_activated(GSimpleAction *action,
              GVariant *parameter,
              gpointer appe)
{
    Emu8086App *app = EMU_8086_APP(appe);

    _PRIV;
    rev_str_activate_cb(priv->win);
}

static void
copy_activated(GSimpleAction *action,
               GVariant *parameter,
               gpointer appe)
{
    Emu8086App *app = EMU_8086_APP(appe);
    _PRIV;
}

static void
redo_activated(GSimpleAction *action,
               GVariant *parameter,
               gpointer appe)
{
    Emu8086App *app = EMU_8086_APP(appe);
    _PRIV;
}

static void
paste_activated(GSimpleAction *action,
                GVariant *parameter,
                gpointer appe)
{
    Emu8086App *app = EMU_8086_APP(appe);
    _PRIV;
}

static void
select_all_activated(GSimpleAction *action,
                     GVariant *parameter,
                     gpointer appe)
{
    Emu8086App *app = EMU_8086_APP(appe);
    _PRIV;
}

static void
undo_activated(GSimpleAction *action,
               GVariant *parameter,
               gpointer appe)
{
    Emu8086App *app = EMU_8086_APP(appe);
    _PRIV;
}

static void
cut_activated(GSimpleAction *action,
              GVariant *parameter,
              gpointer appe)
{
    Emu8086App *app = EMU_8086_APP(appe);
    _PRIV;
}
static void
pref_activated(GSimpleAction *action,
               GVariant *parameter,
               gpointer app)
{
    GtkWindow *win;

    win = gtk_application_get_active_window(GTK_APPLICATION(app));
    Emu8086AppPrefs *prefs;
    prefs = emu8086_app_prefs_new(EMU_8086_APP_WINDOW(win));
    // gtk_window_set_default_size(GTK_WINDOW(prefs), 400, 500);
    gtk_window_set_title(prefs, "Settings");
    gtk_widget_show_all(prefs);
}

static GActionEntry app_entries[] = {
    {"open", open_activated, NULL, NULL, NULL},
    {"save", save_activated, NULL, NULL, NULL},

    {"save_as", save_as_activated, NULL, NULL, NULL},
    {"quit", quit_activated, NULL, NULL, NULL},

    {"help", help_activated, NULL, NULL, NULL},
    {"ex1", ex1_activated, NULL, NULL, NULL},

    {"ex2", ex2_activated, NULL, NULL, NULL},

    {"copy", copy_activated, NULL, NULL, NULL},

    {"redo", redo_activated, NULL, NULL, NULL},

    {"undo", undo_activated, NULL, NULL, NULL},

    {"cut", cut_activated, NULL, NULL, NULL},

    {"paste", paste_activated, NULL, NULL, NULL},

    {"select_all", select_all_activated, NULL, NULL, NULL},

    {"pref", pref_activated, NULL, NULL, NULL},

};

static void emu_8086_startup(GApplication *app)
{
    GtkBuilder *builder;
    GMenuModel *app_menu;
    const gchar *quit_accels[2] = {"<Ctrl>Q", NULL};
    const gchar *open_accels[2] = {"<Ctrl>O", NULL};
    const gchar *save_accels[2] = {"<Ctrl>S", NULL};

    G_APPLICATION_CLASS(emu_8086_app_parent_class)->startup(app);
    g_action_map_add_action_entries(G_ACTION_MAP(app),
                                    app_entries, G_N_ELEMENTS(app_entries),
                                    app);
    gtk_application_set_accels_for_action(GTK_APPLICATION(app), "app.quit",
                                          quit_accels);
    gtk_application_set_accels_for_action(GTK_APPLICATION(app), "app.open",
                                          open_accels);
    gtk_application_set_accels_for_action(GTK_APPLICATION(app), "app.save",
                                          save_accels);

    builder = gtk_builder_new_from_resource("/com/krc/emu8086app/ui/menu.ui");
    app_menu = G_MENU_MODEL(gtk_builder_get_object(builder, "appmenu"));
    gtk_application_set_menubar(GTK_APPLICATION(app), app_menu);
    g_object_unref(builder);
}

static void
emu_8086_app_class_init(Emu8086AppClass *class)
{
    G_APPLICATION_CLASS(class)->startup = emu_8086_startup;
    G_APPLICATION_CLASS(class)->activate = emu_8086_activate;
    G_APPLICATION_CLASS(class)->open = emu_8086_open;
}

Emu8086App *emu_8086_app_new(void)
{
    return g_object_new(EMU_8086_APP_TYPE,
                        "application-id", "com.krc.emu8086",
                        "flags", G_APPLICATION_HANDLES_OPEN,
                        NULL);
}

void open_help()
{
    GtkBuilder *builder;
    Emu8086AboutWindow *window;
    // const gchar *title = ;
    window = emu_8086_about_window_new("Help");
    // gtk_window_set_icon_from_file(window, "/usr/share/datausage/resources/leo.png", NULL);
    //

    // #ifdef __linux__
    GError *error = NULL;
    GdkPixbuf *pic;
#ifdef __linux__
    pic = gdk_pixbuf_new_from_file("/usr/local/share/emu8086/pics/leo.png", &error);

#endif
#ifdef _WIN32
    pic = gdk_pixbuf_new_from_resource("/com/krc/emu8086app/pics/emu8086.png", error);
#endif
    if (error == NULL)
        gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(window), pic);
    else
        g_debug(error->message);
    // g_free(pic);
    if (error != NULL)
        g_error_free(error);

    // gtk_window_set_default_icon_list()
    gtk_window_present(GTK_WINDOW(window));
}

void quit(Emu8086App *app)
{
    stop(app, FALSE);

    exit(EXIT_SUCCESS);
}