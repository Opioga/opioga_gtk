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
#include <assembler.h>
#include <emu8086.h>
#include <emu8086app.h>
#include <emu8086win.h>
#include <emu8086aboutwin.h>
#include <emu8086appprefs.h>

extern struct instruction *_instruction_list;
extern struct instruction *_current_instruction, *_first_instruction;
extern struct label *label_list, *explore;
extern struct errors_list *first_err, *list_err;
extern int errors, assembler_step;

struct _Emu8086App
{
    GtkApplication parent;
};
typedef struct _Emu8086AppPrivate Emu8086AppPrivate;

struct _Emu8086AppPrivate
{
    struct emu8086 *aCPU;
    char *fname;
    gint state;
    Emu8086AppWindow *win;
    GSettings *settings;
    gint to;
};

G_DEFINE_TYPE_WITH_PRIVATE(Emu8086App, emu_8086_app, GTK_TYPE_APPLICATION);

static void emu_free(Emu8086App *app)
{
    _PRIV;
    struct emu8086 *aCPU = priv->aCPU;
    if (aCPU == NULL)
        return;
    struct instruction *_current_instruction2 = _first_instruction;

    struct instruction *prev = _current_instruction2->prev;
    struct instruction *next;
    next = _current_instruction2;
    //free(next);
    int i;
    i = 0;
    // printf("%d\n", label_identifier);
    while (next != NULL)
    {
        //  printf("jj\n");
        _current_instruction2 = next;

        next = next->next;
        free(_current_instruction2);
        //   }
        //}
        i++;
        //break;
    };
    i = 0;
    // g_print("herennn 00 \n\n ");/
    struct label *explore = label_list, *_next;
    if (label_list != NULL)
    {
        while (explore->left != NULL)
            explore = explore->left;
        _next = explore;
        while (_next != NULL)
        {
            explore = _next;
            _next = _next->right;
            free(explore);
            i++;
        }
    }
    // g_print("herennn \n\n ");
    if ((errors > 0) && (first_err != NULL))
    {
        struct errors_list *e = first_err, *n;
        n = first_err;
        if (first_err->next == NULL)
        {
            free(first_err);
        }
        else
        {
            while (n != NULL)
            {
                e = n;
                n = n->next;
                free(e);
                // i++;
            }
        }
    }
    first_err = NULL;
    label_list = NULL;
    _instruction_list = NULL;
    errors = 0;
    _first_instruction = NULL;
    aCPU->instructions_list = NULL;
    list_err = NULL;

    //  free(aCPU->mSFR);
    // free(aCPU->mDataMem);

    free(aCPU);
    set_app_state(app, STOPPED);
    priv->aCPU = NULL;
}
static void
emu_8086_app_init(Emu8086App *app)
{

    Emu8086AppPrivate *priv;
    // GApplication *application = g_application_new("com.krc.emu8086app", G_APPLICATION_NON_UNIQUE );
    // gboolean g = g_application_register(application, NULL, NULL);
    // g_print("%d", g);
    priv = emu_8086_app_get_instance_private(app);
    priv->aCPU = NULL;
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
    priv->state = STOPPED;
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
    priv->state = STOPPED;
    if (priv->aCPU != NULL)
        priv->aCPU = NULL;
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

    builder = gtk_builder_new_from_resource("/com/krc/emu8086app/menu.ui");
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

void set_app_state(Emu8086App *app, gint state)
{
    _PRIV;
    priv->state = state;
}

void execute(struct emu8086 *aCPU)
{

    if (IP < aCPU->end_address - 1)
    {
        int op = *(CODE_SEGMENT_IP), handled = 0;
        aCPU->op[op](aCPU, &handled);
        if (!handled)
        {
            char buf[15];
            sprintf(buf, "Unhandled instrution on line %d, opcode: %x", _INSTRUCTIONS->line_number, op); //message()
            message(buf, ERR, _INSTRUCTIONS->line_number);
        }
        if (aCPU->skip_next)
            aCPU->skip_next = 0;
        else if (_INSTRUCTIONS->next != NULL)
            _INSTRUCTIONS = _INSTRUCTIONS->next;
        if (IP > 0xffff)
        {
            IP = IP - 0xffff;
            CS = CS - 0x10000;
        }
        handled = 0;
    }
}

static void stop(Emu8086App *app, gboolean reset)
{
    _PRIV;
    g_return_if_fail(priv->state != STOPPED);
    if (priv->aCPU != NULL)
        emu_free(app);
    set_app_state(app, STOPPED);
    if (reset)
        reset_win(priv->win);
}

void stop_clicked_app(Emu8086App *app)
{
    stop(app, TRUE);
}

int emu_run(Emu8086App *app)
{
    _PRIV;
    // g_return_if_fail()
    if (priv == NULL)
        return;
    if (priv->state != PLAYING)
        return 0;
    struct emu8086 *aCPU = priv->aCPU;
    g_return_val_if_fail(aCPU != NULL, 0);
    g_return_val_if_fail(aCPU->instructions_list != NULL, 0);
    //struct emu8086 *aCPU = priv->aCPU;
    if (aCPU->is_first == 1)
    {
        emu_8086_app_window_update_wids(priv->win, priv->aCPU);
        aCPU->is_first = 0;
        return 1;
    }
    errors = 0;
    execute(aCPU);
    if (errors > 0)
    {
        emu_8086_app_window_flash(priv->win, first_err->message);
        g_debug(list_err->message);
        stop(app, FALSE);
        return 0;
        // exit(1);
    }
    if (priv->win != NULL)
        emu_8086_app_window_update_wids(priv->win, aCPU);
    if (IP == aCPU->end_address - 1)
    {
        // g_timeout_
        stop(app, TRUE);
        return 0;
    }
    return 1;
}
static int emu_init(Emu8086App *app, gchar *fname)
{
    _PRIV;
    struct emu8086 *aCPU = priv->aCPU;
    if (aCPU == NULL)
    {
        aCPU = emu8086_new();
        priv->aCPU = aCPU;

        if (aCPU == NULL)
            exit(1);
    }
    errors = 0;
    assembler_step = 0;
    do_assembly(priv->aCPU, fname);
    if (errors > 0)
    {
        // g_print(first_err->message);
        emu_8086_app_window_flash(priv->win, first_err->message);
        set_app_state(app, STOPPED);
        emu_free(app);
        return 0;
    }
    assembler_step = 1;
    errors = 0;
    do_assembly(priv->aCPU, fname);
    if (errors > 0)
    {
        // g_print(first_err->message);
        emu_8086_app_window_flash(priv->win, first_err->message);
        set_app_state(app, STOPPED);
        emu_free(app);
        return 0;
    }

    CS = aCPU->code_start_addr / 0x10;
    DS = 0x03ff;
    BX = 5;
    BP = 15;
    SP = 128;
    _SS = ((CS * 0x10) - 0x20000) / 0x10;
    //  g_print("knnkk\n");
    return 1;
}

void step_clicked_app(Emu8086App *app, gchar *fname)
{
    _PRIV;
    g_return_if_fail(priv->state != PLAYING);

    set_app_state(app, STEP);
    if (priv->state != STEP)
        return;
    if (priv->aCPU == NULL)
    {
        if (!emu_init(app, fname))
            return;
    }
    if (errors > 0)
    {
        if (!emu_init(app, fname))
            return;
    }
    if (priv->aCPU != NULL)
    {
        struct emu8086 *aCPU = priv->aCPU;
        if (aCPU->is_first == 1)
        {
            emu_8086_app_window_update_wids(priv->win, priv->aCPU);
            aCPU->is_first = 0;
            return;
        }
        emu_8086_app_window_update_wids(priv->win, priv->aCPU);
        errors = 0; // g_print("kkk\n");
        execute(priv->aCPU);
        if (errors > 0)
        {
            emu_8086_app_window_flash(priv->win, first_err->message);
            g_debug(list_err->message);
            stop(app, FALSE);
            return;
            // exit(1);
        }
        emu_8086_app_window_update_wids(priv->win, priv->aCPU);
    }
}
void step_over(Emu8086App *app, gchar *fname, uint16_t be)
{
    _PRIV;
    struct emu8086 *aCPU = priv->aCPU;

    if (aCPU != NULL)
    {

        aCPU->last_ip = be;
        while (IP == aCPU->last_ip && IP < aCPU->end_address - 1)

        {

            emu_8086_app_window_update_wids(priv->win, priv->aCPU);

            errors = 0;
            execute(priv->aCPU);
            if (errors > 0)
            {
                emu_8086_app_window_flash(priv->win, first_err->message);
                g_debug(list_err->message);
                stop(app, FALSE);
                return;
                // exit(1);
            }
            emu_8086_app_window_update_wids(priv->win, priv->aCPU);
        }
    }
}

void step_over_clicked_app(Emu8086App *app, gchar *fname)
{
    _PRIV;
    struct emu8086 *aCPU = NULL;
    aCPU = priv->aCPU;

    g_return_if_fail(priv->state != PLAYING);

    if (aCPU == NULL)
    {
        if (!emu_init(app, fname))
            return;
        aCPU = priv->aCPU;
    }
    if (errors > 0)
    {
        if (!emu_init(app, fname))
            return;
        aCPU = priv->aCPU;
    }

    set_app_state(app, STEP);
    if (priv->state != STEP)
        return;
    if (aCPU == NULL)
    {
        return;
    }
    if (aCPU->is_first == 1)
    {
        emu_8086_app_window_update_wids(priv->win, priv->aCPU);
        aCPU->is_first = 0;
        return;
    }
    errors = 0;
    step_over(app, fname, IP);
    if (errors > 0)
    {
        emu_8086_app_window_flash(priv->win, first_err->message);
        g_debug(list_err->message);
        stop(app, FALSE);
        return;
        // exit(1);
    }
}
void run_clicked_app(Emu8086App *app, gchar *fname)
{
    _PRIV;

    if (priv->state == PLAYING)
    {
        emu_8086_app_window_flash(priv->win, "RUNNING");

        return;
    }
    struct emu8086 *aCPU = priv->aCPU;
    if (aCPU == NULL)
    {
        if (!emu_init(app, fname))
            return;
    }
    if (errors > 0)
    {
        if (!emu_init(app, fname))
            return;
    }
    if (priv->state == STEP)
    {
        set_app_state(app, PLAYING);
        priv->to = g_timeout_add(1000, (GSourceFunc)emu_run, app);
        return;
    }

    priv->to = g_timeout_add(1000, (GSourceFunc)emu_run, app);
    set_app_state(app, PLAYING);
    // emu_8086_app_window_update_wids(priv->win, aCPU);

    // g_timeout_add
} //x

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