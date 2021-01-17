/* gtkemu8086
 * Copyright 2020 KRC
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * emu8086win.c
 * Window class
 */
#include <emu8086win.h>
#include <emu8086app.h>

#include <code.h>
#include <code_buffer.h>
#include <emu_8086_app_runner.h>
#include <emu_plugin_box.h>
#include <emu_8086_plugins_engine.h>
#include <libpeas/peas-activatable.h>
#include <libpeas/peas-extension-set.h>

typedef enum
{
    PROP_0,
    PROP_UPDATES,
    PROP_MEM,
    PROP_THEME
} Emu8086AppWindowProperty;

typedef struct _Emu8086AppWindowPrivate Emu8086AppWindowPrivate;
struct _Emu8086AppWindowPrivate
{
    PeasExtensionSet *exten_set;

    guint tos;
    GtkWidget *AX_;
    GtkWidget *CX_;
    GtkWidget *DX_;
    GtkWidget *BX_;
    GtkWidget *SP_;
    GtkWidget *BP_;
    GtkWidget *SI_;
    GtkWidget *DI_;

    GtkWidget *FLAGS_;
    GtkWidget *IP_;
    GtkWidget *CS_;
    GtkWidget *DS_;
    GtkWidget *ip;
    GtkWidget *SS_;
    GtkWidget *stack;
    GtkWidget *spinner;
    GtkWidget *tool_bar;
    GtkWidget *ES_;
    Emu8086AppCode *code;
    Emu8086AppCodeRunner *runner;

    GtkWidget *messages; //GtkWidget *spinner;
    gchar *fname;
    GtkWidget *revealer;
    GtkWidget *window_m;
    GtkWidget *scrolled;
    GtkWidget *gears;
    Emu8086App *app;
    GSettings *settings;
    gint open;
};

struct _Emu8086AppWindow
{
    GtkApplicationWindow parent;
    Emu8086AppWindowState state;
    gboolean updates;
    Emu8086AppWindowPrivate *priv;
    gchar *theme;
    gboolean memory;
};
G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppWindow, emu_8086_app_window, GTK_TYPE_APPLICATION_WINDOW);

static void emu_8086_app_window_update_wids(Emu8086AppCodeRunner *runner, gpointer data);
static void reset_win(Emu8086AppCodeRunner *runner, gpointer user_data);
static gboolean check(gchar *n);
gboolean save_new(Emu8086AppWindow *win, gchar *file_name, char *buf);
static void emu_8086_app_window_flash(Emu8086AppCodeRunner *runner, gpointer user_data);
static void emu_8086_app_window_flash2(Emu8086AppWindow *win, gchar *message);
static void add_recent(gchar *uri);
void quick_message(GtkWindow *parent, gchar *message, gchar *title);

static void
copy_activated(GSimpleAction *action,
               GVariant *parameter,
               gpointer appe)
{
    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(appe);
    PRIV;
    GtkTextBuffer *buffer;
    buffer = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code)));
    if (gtk_text_buffer_get_has_selection(buffer))
    {
        GtkClipboard *clipboard;
        clipboard = gtk_widget_get_clipboard(priv->code, GDK_SELECTION_CLIPBOARD);
        gtk_text_buffer_copy_clipboard(buffer, clipboard);
    }
    //   et_enabled(cp, gtk_text_buffer_get_has_selection(GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(code)))));
}

static void
paste_activated(GSimpleAction *action,
                GVariant *parameter,
                gpointer appe)
{
    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(appe);
    PRIV;
    GtkTextBuffer *buffer;
    buffer = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code)));

    GtkClipboard *clipboard;
    clipboard = gtk_widget_get_clipboard(priv->code, GDK_SELECTION_CLIPBOARD);

    gtk_text_buffer_paste_clipboard(buffer, clipboard, NULL, TRUE);
}

static void
select_all_activated(GSimpleAction *action,
                     GVariant *parameter,
                     gpointer appe)
{
    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(appe);
    PRIV;
    GtkTextBuffer *buffer;
    buffer = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code)));
    GtkTextIter start;
    GtkTextIter end;
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    gtk_text_buffer_select_range(buffer, &start, &end);
}

static void
cut_activated(GSimpleAction *action,
              GVariant *parameter,
              gpointer appe)
{
    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(appe);
    PRIV;
    GtkTextBuffer *buffer;
    buffer = GTK_TEXT_BUFFER(gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code)));
    if (gtk_text_buffer_get_has_selection(buffer))
    {
        GtkClipboard *clipboard;
        clipboard = gtk_widget_get_clipboard(priv->code, GDK_SELECTION_CLIPBOARD);
        gtk_text_buffer_cut_clipboard(buffer, clipboard, TRUE);
    }
}

static GActionEntry win_entries[] = {

    {"copy", copy_activated, NULL, NULL, NULL},

    {"cut", cut_activated, NULL, NULL, NULL},

    {"paste", paste_activated, NULL, NULL, NULL},

    {"select", select_all_activated, NULL, NULL, NULL}

};

Emu8086AppWindow *emu_8086_app_window_new(Emu8086App *app)
{

    //  emu_8086_app_get_type()
    return g_object_new(EMU_8086_APP_WINDOW_TYPE, "application", app,
                        NULL);
};

static void open_memory(Emu8086AppWindow *win, gboolean b)
{
    win->memory = b;
    PRIV;
    gtk_revealer_set_reveal_child(GTK_REVEALER(priv->revealer), b);
}

void emu_8086_window_set_memory(Emu8086AppWindow *win, gboolean b)
{
    open_memory(win, b);

    g_object_notify(G_OBJECT(win), "memory");
    //   g_object_set_property(G_OBJECT(win), "memory", &value);
}

static Emu8086AppWindow *
get_drop_window(GtkWidget *widget)
{
    GtkWidget *target_window;

    target_window = gtk_widget_get_toplevel(widget);
    g_return_val_if_fail(EMU_8086_IS_APP_WINDOW(target_window), NULL);

    return EMU_8086_APP_WINDOW(target_window);
}

void emu_8086_app_window_open_drag_data(Emu8086AppWindow *win, GtkSelectionData *selection_data)
{
    PRIV;
    gchar **uri_list;
    uri_list = g_uri_list_extract_uris((gchar *)gtk_selection_data_get_data(selection_data));

    int i = 0;

    gchar *uri;
    uri = uri_list[i];
    i++;
    char *filename, *base;
    gchar *contents;
    gsize length;
    GtkTextBuffer *buffer;
    GFile *file;
    file = g_file_new_for_uri(uri);
    filename = uri + strlen("file//") + 1;
    gint len = g_strv_length(uri_list);

    while (i < len)
    {

        // g_print("file: %s\n", uri_list[i++]);
        gchar *fname = uri_list[i++];
        // g_print("fil: %s\n", fname + strlen("file//") + 1);

        emu_8086_open_file(priv->app, g_file_new_for_uri(fname));
        /* code */
    }

    base = g_file_get_basename(file);

    if (!check(base))
    {
        char err[256];
        // buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));

        sprintf(err, "unsupported file %s", base);
        quick_message(GTK_WINDOW(win), err, "Error");
        if (base != NULL)
            g_free(base);
        free(uri);
        g_object_unref(file);
        // if (file != NULL)
        //     g_free(file);
        return;
    }

    strcpy(win->state.file_name, base);
    //win->state.file_name[strlen(base) - 1] = '\0';
    gtk_window_set_title(GTK_WINDOW(win), win->state.file_name);

    strcpy(win->state.file_path, filename);

    //g_file_get_contents(filename, )
    if (g_file_load_contents(file, NULL, &contents, &length, NULL, NULL))
    {

        PRIV;
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
        refreshLines(EMU_8086_APP_CODE_BUFFER(buffer));
        // uri = gtk_file_chooser_get_uri(chooser);
        add_recent(uri);
        // g_free(uri);
        emu_8086_app_window_set_open(win);
        set_fname(priv->runner, win->state.file_path);
        gtk_text_buffer_set_text(buffer, contents, length);
        strcpy(win->state.file_name, base);
        gtk_window_set_title(GTK_WINDOW(win), win->state.file_name);

        strcpy(win->state.file_path, filename);
        g_free(contents);
        win->state.isSaved = TRUE;
        win->state.file_path_set = TRUE;
    }
    g_strfreev(uri_list);
    g_free(base);
    g_object_unref(file);
}
static void drag_data_received_cb(GtkWidget *widget,
                                  GdkDragContext *context,
                                  gint x,
                                  gint y,
                                  GtkSelectionData *selection_data,
                                  guint info,
                                  guint timestamp,
                                  gpointer data)
{

    Emu8086AppWindow *win;
    win = get_drop_window(widget);

    if (win == NULL)
        return;
    emu_8086_app_window_open_drag_data(win, selection_data);
    gtk_drag_finish(context, TRUE, FALSE, timestamp);
}

static void emu_8086_app_window_init(Emu8086AppWindow *win)
{
    GtkBuilder *builder;
    GtkTargetList *tl;
    GMenuModel *menu;
    gtk_widget_init_template(GTK_WIDGET(win));
    win->priv = emu_8086_app_window_get_instance_private(win);
    PRIV;
    priv->settings = g_settings_new("com.krc.emu8086app");
    GAction *action, *action2;
    priv->runner = emu_8086_app_code_runner_new(NULL, FALSE);
    // g_property_action_new
    action = (GAction *)g_property_action_new("check-updates", win, "updates");
    action2 = (GAction *)g_property_action_new("open_mem", win, "memory");
    builder = gtk_builder_new_from_resource("/com/krc/emu8086app/ui/gears.ui");
    menu = G_MENU_MODEL(gtk_builder_get_object(builder, "menu"));
    g_action_map_add_action(G_ACTION_MAP(win), action);
    g_action_map_add_action(G_ACTION_MAP(win), action2);

    g_object_unref(action);
    g_object_unref(action2);
    g_action_map_add_action_entries(G_ACTION_MAP(win),
                                    win_entries, G_N_ELEMENTS(win_entries),
                                    win);

    gtk_menu_button_set_menu_model(GTK_MENU_BUTTON(priv->gears), menu);

    gtk_drag_dest_set(GTK_WIDGET(win),
                      GTK_DEST_DEFAULT_MOTION |
                          GTK_DEST_DEFAULT_HIGHLIGHT |
                          GTK_DEST_DEFAULT_DROP,
                      NULL,
                      0,
                      GDK_ACTION_COPY);

    if (tl == NULL)
    {
        tl = gtk_target_list_new(NULL, 0);
        gtk_drag_dest_set_target_list(GTK_WIDGET(win), tl);
        gtk_target_list_unref(tl);
    }

    gtk_target_list_add_uri_targets(tl, 1000);

    g_signal_connect(win,
                     "drag_data_received",
                     G_CALLBACK(drag_data_received_cb),
                     NULL);

    g_signal_connect(priv->runner, "exec_stopped", G_CALLBACK(reset_win), win);

    g_signal_connect(priv->runner, "error_occured", G_CALLBACK(emu_8086_app_window_flash), win);
    g_signal_connect(priv->runner, "exec_ins", G_CALLBACK(emu_8086_app_window_update_wids), win);

    Emu8086AppPluginBox *box;
    box = emu_8086_app_plugin_box_new(win, priv->runner);
    gtk_container_add(GTK_CONTAINER(priv->stack), box);
    gtk_widget_show_all(box);

    g_object_unref(builder);
};

void quick_message(GtkWindow *parent, gchar *message, gchar *title)
{
    GtkWidget *dialog, *label, *content_area;
    GtkDialogFlags flags;

    // Create the widgets
    flags = GTK_DIALOG_DESTROY_WITH_PARENT;
    dialog = gtk_dialog_new_with_buttons(title,
                                         parent,
                                         flags,
                                         ("_OK"),
                                         GTK_RESPONSE_NONE,
                                         NULL);
    content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    label = gtk_label_new(message);

    // Ensure that the dialog box is destroyed when the user responds

    g_signal_connect_swapped(dialog,
                             "response",
                             G_CALLBACK(gtk_widget_destroy),
                             dialog);

    // Add the label, and show everything we’ve added
    gtk_widget_set_margin_bottom(label, 20);
    gtk_widget_set_margin_bottom(content_area, 20);
    gtk_widget_set_margin_top(content_area, 20);
    gtk_widget_set_margin_start(content_area, 20);
    gtk_widget_set_margin_end(content_area, 20);
    gtk_container_add(GTK_CONTAINER(content_area), label);
    gtk_widget_show_all(dialog);
}

static gboolean check(gchar *n)
{
    gboolean ret = FALSE;
    // gchar *n = win->state.file_name;
    gint len = strlen(n);
    if (len < 4)
        return ret;
    n += len - 3;
    if (strcmp(n, "asm") == 0)
        ret = TRUE;
    else if (strcmp(n, "ASM") == 0)
        ret = TRUE;
    n -= len - 3;
    return ret;
}

static void add_recent(gchar *uri)
{
    GtkRecentManager *manager;
    GtkRecentData recent_data;

    static gchar *groups[2] = {
        "emu8086",
        NULL};
    manager = gtk_recent_manager_get_default();
    recent_data.display_name = NULL;
    recent_data.description = NULL;
    recent_data.mime_type = (gchar *)"text/x-asm";
    recent_data.app_name = (gchar *)g_get_application_name();
    recent_data.app_exec = g_strjoin(" ", g_get_prgname(), "%u", NULL);
    recent_data.groups = groups;
    // recent_data.
    gboolean v = gtk_recent_manager_add_full(manager, uri, &recent_data);
    g_free(recent_data.app_exec);
}

static void _open(Emu8086AppWindow *win)
{
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;
    gboolean ret = FALSE;
    PRIV;
    stop(priv->runner, FALSE);
    dialog = gtk_file_chooser_dialog_new("Open File",
                                         win,
                                         action,
                                         "_Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         "_Open",
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);
    chooser = GTK_FILE_CHOOSER(dialog);
    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        char *filename, *base;
        gchar *contents;
        gsize length;
        filename = gtk_file_chooser_get_filename(chooser);
        GFile *file = gtk_file_chooser_get_file(chooser);

        base = g_file_get_basename(file);
        if (!check(base))
        {
            char err[256];
            sprintf(err, "unsupported file\n %s", base);
            quick_message(GTK_WINDOW(win), err, "Error");
            g_free(base);
            free(filename);
            gtk_widget_destroy(dialog);
            return;
        }

        gchar *uri;

        strcpy(win->state.file_name, base);
        //win->state.file_name[strlen(base) - 1] = '\0';
        gtk_window_set_title(GTK_WINDOW(win), win->state.file_name);

        strcpy(win->state.file_path, filename);

        //g_file_get_contents(filename, )
        if (g_file_load_contents(file, NULL, &contents, &length, NULL, NULL))
        {
            GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));

            refreshLines(EMU_8086_APP_CODE_BUFFER(buffer));
            uri = gtk_file_chooser_get_uri(chooser);
            add_recent(uri);
            g_free(uri);
            emu_8086_app_window_set_open(win);

            emu_8086_app_window_set_open(win);
            set_fname(priv->runner, win->state.file_path);
            gtk_text_buffer_set_text(buffer, contents, length);
            // update(buffer, EMU_8086_APP_CODE(priv->code));
            strcpy(win->state.file_name, base);
            //win->state.file_name[strlen(base) - 1] = '\0';
            gtk_window_set_title(GTK_WINDOW(win), win->state.file_name);

            // strcpy(win->state.file_path, filename);
            g_free(contents);
            win->state.isSaved = TRUE;
            win->state.file_path_set = TRUE;
        }

        free(filename);
        free(base);
    }
    gtk_widget_destroy(dialog);
}

static gboolean
emu_8086_window_key_press_event(GtkWidget *widget,
                                GdkEventKey *event)
{
    static gpointer grand_parent_class = NULL;
    gboolean handled = FALSE;
    GtkWindow *window = GTK_WINDOW(widget);

    if (event->state & GDK_CONTROL_MASK)
    {
        if ((event->keyval == GDK_KEY_o))
        {
            handled = TRUE;
            _open(EMU_8086_APP_WINDOW(widget));
        }
        else if ((event->keyval == GDK_KEY_plus) || (event->keyval == GDK_KEY_KP_Add) || (event->keyval == GDK_KEY_equal))
        {
            handled = TRUE;
            Emu8086AppWindow *win = EMU_8086_APP_WINDOW(window);
            PRIV;
            if (win->state.fontSize < 300)
            {
                win->state.fontSize++;
                editFontSize(priv->code, 1);
            }

            else
            {
                emu_8086_app_window_flash2(win, "Max font size reached");
            }
        }

        else if ((event->keyval == GDK_KEY_minus) || (event->keyval == GDK_KEY_KP_Subtract))
        {
            handled = TRUE;
            Emu8086AppWindow *win = EMU_8086_APP_WINDOW(window);
            PRIV;
            if (win->state.fontSize > -100)
            {
                win->state.fontSize--;
                editFontSize(priv->code, -1);
            }

            else
            {
                emu_8086_app_window_flash(win, "Min font size reached");
            }
        }
        else if ((event->keyval == GDK_KEY_s))
        {
            handled = TRUE;
            emu_8086_app_window_save_doc(EMU_8086_APP_WINDOW(widget));
        }
        // else if( )
    }
    if (grand_parent_class == NULL)
        grand_parent_class = g_type_class_peek_parent(emu_8086_app_window_parent_class);

    if (!handled)
        handled = gtk_window_propagate_key_event(window, event);

    /* handle mnemonics and accelerators */
    if (!handled)
        handled = gtk_window_activate_key(window, event);

    /* Chain up, invokes binding set */
    if (!handled)
        handled = GTK_WIDGET_CLASS(grand_parent_class)->key_press_event(widget, event);

    return handled;
}
void emu_8086_app_window_open_activate_cb(Emu8086AppWindow *win)
{
    _open(win);
}

void emu_8086_app_window_save_activate_cb(Emu8086AppWindow *win)
{

    emu_8086_app_window_save_doc(win);
    // gtk_application_ex
}
void emu_8086_app_window_save_as_activate_cb(Emu8086AppWindow *win)
{

    if (save_new(win, "lol", win->state.file_path))
    {
        PRIV;
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
        // GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
        GtkTextIter start_iter, end_iter;
        gtk_text_buffer_get_start_iter(buffer, &start_iter);
        gtk_text_buffer_get_end_iter(buffer, &end_iter);
        gchar *con = gtk_text_buffer_get_text(buffer, &start_iter, &end_iter, FALSE);

        win->state.file_path_set = TRUE;
        emu_8086_app_window_write_to_file(win->state.file_path, con, win->state.file_name);
        gtk_window_set_title(GTK_WINDOW(win), win->state.file_name);
        win->state.isSaved = TRUE;
        return TRUE;
    };
}

void emu_8086_app_window_arr_sum_activate_cb(Emu8086AppWindow *win)
{
    PRIV;
    gchar *con;
    gsize len;
    gchar *path;
#ifdef __WIN32
    path = "egs/ArraySum.asm";
#endif

#ifdef __linux__
    path = "/usr/local/share/emu8086/egs/ArraySum.asm";

#endif // DEBUG
    GFile *file = g_file_new_for_path(path);
    g_return_if_fail(file != NULL);
    strcpy(win->state.file_path, path);
    strcpy(win->state.file_name, "ArraySum.asm");

    if (g_file_load_contents(file, NULL, &con, &len, NULL, NULL))
    {

        // memcpy
        GtkTextBuffer *buf;
        buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
        refreshLines(EMU_8086_APP_CODE_BUFFER(buf));
        emu_8086_app_window_set_open(win);
        set_fname(priv->runner, win->state.file_path);
        gtk_text_buffer_set_text(buf, con, len);
        // update(buf, EMU_8086_APP_CODE(priv->code));

        // g_free(contents);
        win->state.isSaved = TRUE;
        // win->state.file_path_set = TRUE;
        g_free(con);
    }
}

void emu_8086_app_window_rev_str_activate_cb(Emu8086AppWindow *win)
{
    PRIV;
    gchar *con;
    gsize len;
    gchar *path;
#ifdef __WIN32
    path = "egs/RevStr.asm";
#endif

#ifdef __linux__
    path = "/usr/local/share/emu8086/egs/RevStr.asm";

#endif // DEBUG

    GFile *file = g_file_new_for_path(path);
    g_return_if_fail(file != NULL);
    strcpy(win->state.file_path, path);
    strcpy(win->state.file_name, "RevStr.asm");

    if (g_file_load_contents(file, NULL, &con, &len, NULL, NULL))
    {

        // memcpy
        GtkTextBuffer *buf;
        buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
        refreshLines(EMU_8086_APP_CODE_BUFFER(buf));
        emu_8086_app_window_set_open(win);
        set_fname(priv->runner, win->state.file_path);
        gtk_text_buffer_set_text(buf, con, len);
        // update(buf, EMU_8086_APP_CODE(priv->code));

        // g_free(contents);
        win->state.isSaved = TRUE;
        // win->state.file_path_set = TRUE;
        g_free(con);
    }
}

static void emu8086_win_change_theme(Emu8086AppWindow *win)
{
    PRIV;
    gint a;
    GdkRGBA color;

    //   ;
    if (strcmp("dark+", win->theme) == 0)
    {
        color.alpha = 1;
        color.red = 0.22;
        color.green = 0.22;
        color.blue = 0.22;
    }

    else if (strcmp("cobalt", win->theme) == 0)
    {
        color.alpha = 1;
        color.red = 0;
        color.green = 0.11;
        color.blue = 0.2;
    }

    else if (strcmp("light", win->theme) == 0)
    {
        color.alpha = 1;
        color.red = .98;
        color.green = .98;
        color.blue = .98;
    }
    gtk_widget_override_background_color(priv->code, GTK_STATE_NORMAL, &color);
}

static void
emu_8086_window_set_property(GObject *object,
                             guint property_id,
                             const GValue *value,
                             GParamSpec *pspec)
{
    Emu8086AppWindow *self = EMU_8086_APP_WINDOW(object);
    // g_print("l %d\n", *value);

    switch ((Emu8086AppWindowProperty)property_id)
    {
    case PROP_UPDATES:
        // *v = (gboolean *)value;

        self->updates = g_value_get_boolean(value);
        // g_print("filename: %s\n", self->filename);
        break;
    case PROP_MEM:
        // *v = (gboolean *)value;

        open_memory(self, g_value_get_boolean(value));
        // g_print("filename: %s\n", self->filename);
        break;
    case PROP_THEME:
        // *v = (gboolean *)value;

        self->theme = g_value_get_string(value);
        emu8086_win_change_theme(self);
        // g_print("filename: %s\n", self->filename);
        break;
    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void
emu_8086_window_get_property(GObject *object,
                             guint property_id,
                             GValue *value,
                             GParamSpec *pspec)
{
    Emu8086AppWindow *self = EMU_8086_APP_WINDOW(object);

    switch ((Emu8086AppWindowProperty)property_id)
    {
    case PROP_UPDATES:
        g_value_set_boolean(value, self->updates);
        break;
    case PROP_MEM:
        g_value_set_boolean(value, self->memory);
        break;
    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void emu_8086_app_window_class_init(Emu8086AppWindowClass *class)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                                                "/com/krc/emu8086app/ui/emu8086.ui");
    GObjectClass *object_class = G_OBJECT_CLASS(class);
    object_class->set_property = emu_8086_window_set_property;
    object_class->get_property = emu_8086_window_get_property;
    widget_class->key_press_event = emu_8086_window_key_press_event;
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, gears);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, spinner);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, revealer);

    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, tool_bar);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, stack);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, scrolled);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, messages);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, FLAGS_);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, AX_);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, BX_);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, CX_);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, DX_);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, SP_);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, BP_);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, SI_);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, DI_);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, DS_);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, SS_);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, ip);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, CS_);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, ES_);

    g_object_class_install_property(object_class, PROP_THEME,
                                    g_param_spec_string("theme",
                                                        "Theme",
                                                        "The window's theme",
                                                        "dark+",
                                                        G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_UPDATES,
                                    g_param_spec_boolean("updates",
                                                         "Updates",
                                                         "The window's state",
                                                         FALSE,
                                                         G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_MEM,
                                    g_param_spec_boolean("memory",
                                                         "Memory",
                                                         "Toggle memory view",
                                                         FALSE,
                                                         G_PARAM_READWRITE));
};

void char_ins(GtkTextView *text_view,
              gchar *string,
              gpointer user_data)
{
    exit(1);
}

void emu_8086_app_window_upd(Emu8086AppWindow *win)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(win->priv->code));
    gboolean modified = gtk_text_buffer_get_modified(buffer);
    if (win->state.isSaved && (win->priv->open == 0) && modified)
    {
        char buf[strlen(win->state.file_name)+ 2];
        sprintf(buf, "%s*", win->state.file_name);
        gtk_window_set_title(GTK_WINDOW(win), buf);
        win->state.isSaved = FALSE;
    }
    unsetOpen(win);
    gtk_text_buffer_set_modified(buffer, FALSE);
}

gchar *emu_8086_app_window_write_to_file(gchar *filename, gchar *buffer, char *buff)
{

    FILE *f;
    GFile *file;
    file = g_file_new_for_path(filename);
    // f = g_file_open_readwrite(f, );
    char buf[40];
    //g_fi

    f = fopen(filename, "w");
    if (f == NULL)
    {
        sprintf(buf, "Unable to save %s", filename);
        //  gtk_label_set_text(GTK_LABEL(priv->messages), buf);
        // g_free(filename);

        return;
    }
    if (fputs(buffer, f))
        // g_print(filename);
        fclose(f);
    //free(f);name(file)
    if (file != NULL)
    {
        gchar *base = g_file_get_basename(file);
        sprintf(buff, "%s", base);
        return base;
    }
    else
        sprintf(buff, "%s", "base");
    return "base";
}

gboolean save_new(Emu8086AppWindow *win, gchar *file_name, char *buf)
{
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;
    gboolean ret = FALSE;
    dialog = gtk_file_chooser_dialog_new("Save File",
                                         win,
                                         action,
                                         "_Cancel",
                                         GTK_RESPONSE_CANCEL,
                                         "_Save",
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);
    chooser = GTK_FILE_CHOOSER(dialog);

    gtk_file_chooser_set_do_overwrite_confirmation(chooser, TRUE);

    // (user_edited_a_new_document)
    gtk_file_chooser_set_current_name(chooser,
                                      win->state.file_name);

    res = gtk_dialog_run(GTK_DIALOG(dialog));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        gchar *filename;

        filename = gtk_file_chooser_get_filename(chooser);
        gchar *uri;
        uri = gtk_file_chooser_get_uri(chooser);
        add_recent(uri);
        //  save_to_file(filename);
        strcpy(buf, filename);
        g_free(filename);
        g_free(uri);
        ret = TRUE;
    }

    gtk_widget_destroy(dialog);
    return ret;
}

gboolean emu_8086_app_window_save_doc(Emu8086AppWindow *win)
{
    gchar *fname = win->state.file_path;
    gchar buf[5];
    strncpy(buf, fname, 4);
    buf[4] = '\0';
    if (strcmp(buf, "/usr") == 0)
    {
        return;
    }

    if (!win->state.isSaved)
    {
        PRIV;
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
        char buf2[50];
        // GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
        GtkTextIter start_iter, end_iter;
        gtk_text_buffer_get_start_iter(buffer, &start_iter);
        gtk_text_buffer_get_end_iter(buffer, &end_iter);
        gchar *con = gtk_text_buffer_get_text(buffer, &start_iter, &end_iter, FALSE);
        if (win->state.file_path_set)
        {
            emu_8086_app_window_write_to_file(win->state.file_path, con, win->state.file_name);
            set_fname(priv->runner, win->state.file_path);

            gtk_window_set_title(GTK_WINDOW(win), win->state.file_name);
            win->state.isSaved = TRUE;
            return TRUE;
        }
        else
        {
            if (save_new(win, "lol", win->state.file_path))
            {

                win->state.file_path_set = TRUE;
                emu_8086_app_window_write_to_file(win->state.file_path, con, win->state.file_name);
                set_fname(priv->runner, win->state.file_path);

                gtk_window_set_title(GTK_WINDOW(win), win->state.file_name);
                win->state.isSaved = TRUE;
                return TRUE;
            };
            return FALSE;
        }
        return TRUE;
    }
    return TRUE;
}

void play_clicked(GtkToolButton *toolbutton,
                  gpointer user_data)
{
    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(user_data);
    PRIV;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
    gint c = gtk_text_buffer_get_char_count(buffer);
    if (!emu_8086_app_window_save_doc(win))
    {
        return;
    }
    if (c > 0)
    {
        gtk_text_view_set_editable(GTK_TEXT_VIEW(priv->code), FALSE);
        emu_8086_window_set_memory(win, TRUE);
        run_clicked_app(priv->runner);
    }
    else
        emu_8086_app_window_flash2(win, "Nothing to run");
}

void pause_clicked(GtkToolButton *toolbutton,
                   gpointer user_data)
{
    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(user_data);
    PRIV;
    set_app_state(priv->runner, STEP);
    //
}

void step_clicked(GtkToolButton *toolbutton,
                  gpointer user_data)
{
    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(user_data);

    if (!emu_8086_app_window_save_doc(win))
    {
        return;
    }
    PRIV;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
    gint c = gtk_text_buffer_get_char_count(buffer);
    // g_print("herr\n");
    if (c > 0)
    {
        gtk_text_view_set_editable(GTK_TEXT_VIEW(priv->code), FALSE);
        if (!win->memory)
            emu_8086_window_set_memory(win, TRUE);
        step_clicked_app(priv->runner);
    }

    else
        emu_8086_app_window_flash2(win, "Nothing to run");
}
void step_over_clicked(GtkToolButton *toolbutton,
                       gpointer user_data)
{
    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(user_data);
    if (!emu_8086_app_window_save_doc(win))
    {
        return;
    }
    PRIV;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
    gint c = gtk_text_buffer_get_char_count(buffer);

    if (c > 0)
    {
        gtk_text_view_set_editable(GTK_TEXT_VIEW(priv->code), FALSE);
        if (!win->memory)
            emu_8086_window_set_memory(win, TRUE);

        gint bps[100], len;
        len = 0;
        // bps[100];

        step_over_clicked_app(priv->runner, priv->code);
    }
    else
        emu_8086_app_window_flash2(win, "Nothing to run");
}
void save_clicked(GtkToolButton *toolbutton,
                  gpointer user_data)
{
    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(user_data);
    emu_8086_app_window_save_doc(win);
    //
}

void open_clicked(GtkToolButton *toolbutton,
                  gpointer user_data)
{
    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(user_data);
    PRIV;
    _open(win);
    //
}
void stop_clicked(GtkToolButton *toolbutton,
                  gpointer user_data)
{
    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(user_data);
    PRIV;
    stop_clicked_app(priv->runner);
    // emu_8086_window_set_memory(win, FALSE);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(priv->code), TRUE);

    //step_over_clicked_app
}

static void open_item(GtkRecentChooser *recents, gpointer user_data)
{
    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(user_data);
    gchar *uri;
    uri = gtk_recent_chooser_get_current_uri(recents);
    GFile *file = NULL, *contents = NULL, *base, *filename;
    gsize length;
    file = g_file_new_for_uri(uri);
    g_return_if_fail(file != NULL);
    base = g_file_get_basename(file);
    filename = g_file_get_path(file);
    if (g_file_load_contents(file, NULL, &contents, &length, NULL, NULL))
    {
        PRIV;
        stop(priv->runner, FALSE);
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
        refreshLines(EMU_8086_APP_CODE_BUFFER(buffer));

        strcpy(win->state.file_name, base);
        //win->state.file_name[strlen(base) - 1] = '\0';
        gtk_window_set_title(GTK_WINDOW(win), win->state.file_name);

        strcpy(win->state.file_path, filename);
        add_recent(uri);
        g_free(uri);
        emu_8086_app_window_set_open(win);
        set_fname(priv->runner, win->state.file_path);
        gtk_text_buffer_set_text(buffer, contents, length);
        // update(buffer, EMU_8086_APP_CODE(priv->code));

        g_free(contents);
        g_free(base);
        g_free(filename);
        win->state.isSaved = TRUE;
        win->state.file_path_set = TRUE;
    }
}

void populate_tools(Emu8086AppWindow *win)
{
    PRIV;
    GtkRecentManager *manager;
    manager = gtk_recent_manager_get_default();
    GtkToolButton *play, *step, *stop, *pause, *save, *step_over;
    GtkToolItem *sep;
    GtkMenuToolButton *recents;
    GtkWidget *recents_chooser;
    GtkRecentFilter *filter;
    GtkStyleProvider *provider;
    provider = GTK_STYLE_PROVIDER(gtk_css_provider_new());

    recents_chooser = gtk_recent_chooser_menu_new_for_manager(manager);
    filter = gtk_recent_filter_new();
    gtk_recent_filter_add_group(filter, "emu8086");
    // gtk_recent_filter_add_pattern(filter, "*.asm");
    gtk_recent_chooser_set_filter(GTK_RECENT_CHOOSER(recents_chooser),
                                  filter);
    recents = gtk_menu_tool_button_new(NULL, NULL);
    gtk_menu_tool_button_set_menu(recents, (recents_chooser));
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(recents), "document-open");
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(recents), ("Open"));
    gtk_recent_chooser_set_local_only(GTK_RECENT_CHOOSER(recents_chooser),
                                      TRUE);
    gtk_tool_item_set_is_important(recents, TRUE);
    gtk_tool_item_set_tooltip_text(GTK_TOOL_ITEM(recents),
                                   ("Open a file"));
    gtk_menu_tool_button_set_arrow_tooltip_text(GTK_MENU_TOOL_BUTTON(recents),
                                                ("Open a recently used file"));

    gtk_widget_show(recents);
    play = gtk_tool_button_new(NULL, NULL);
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(play), ("Run"));
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(play), "media-playback-start");
    gtk_tool_button_set_use_underline(GTK_TOOL_BUTTON(play), TRUE);
    gtk_tool_item_set_is_important(play, TRUE);
    gtk_widget_show(play);

    sep = gtk_separator_tool_item_new();
    gtk_tool_item_set_is_important(sep, TRUE);

    gtk_separator_tool_item_set_draw(sep, TRUE);
    gtk_widget_show(sep);

    stop = gtk_tool_button_new(NULL, NULL);

    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(stop), "media-playback-stop");
    gtk_widget_show(stop);
    gtk_tool_item_set_tooltip_text(stop, "Stop Executing");

    pause = gtk_tool_button_new(NULL, NULL);
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(pause), "media-playback-pause");
    gtk_tool_item_set_tooltip_text(pause, "Halt Execution");
    gtk_tool_button_set_use_underline(GTK_TOOL_BUTTON(pause), TRUE);

    gtk_widget_show(pause);

    step = gtk_tool_button_new(NULL, NULL);
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(step), "media-skip-forward");
    gtk_tool_item_set_tooltip_text(step, "Step to next instruction");
    gtk_tool_button_set_use_underline(GTK_TOOL_BUTTON(step), TRUE);
    gtk_widget_show(step);

    save = gtk_tool_button_new(NULL, NULL);
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(save), "document-save");
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(save), ("Save"));
    gtk_tool_button_set_use_underline(GTK_TOOL_BUTTON(save), TRUE);
    gtk_tool_item_set_is_important(save, TRUE);
    gtk_widget_show(save);

    // open = gtk_tool_button_new(NULL, NULL);
    // gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(open), "document-open");
    // gtk_tool_button_set_label(GTK_TOOL_BUTTON(open), ("Open"));
    // gtk_tool_button_set_use_underline(GTK_TOOL_BUTTON(open), TRUE);
    // gtk_tool_item_set_is_important(open, TRUE);
    // gtk_widget_show(open);
    //

    step_over = gtk_tool_button_new(NULL, NULL);
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(step_over), "forward");
    gtk_tool_item_set_tooltip_text(step_over, "Step Over to breakpoint");
    gtk_widget_show(step_over);
    // GtkToolItemGroup
    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, save, 0);
    // gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, open, -1);
    gtk_widget_set_margin_left(recents, 5);
    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, recents, -1);
    gtk_widget_set_margin_left(sep, 5);
    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, sep, -1);

    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, play, -1);
    gtk_widget_set_margin_left(pause, 5);
    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, pause, -1);
    gtk_widget_set_margin_left(step, 5);
    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, step, -1);
    gtk_widget_set_margin_left(step_over, 5);
    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, step_over, -1);
    gtk_widget_set_margin_left(stop, 5);
    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, stop, -1);

    g_signal_connect(play, "clicked", G_CALLBACK(play_clicked), win);
    g_signal_connect(pause, "clicked", G_CALLBACK(pause_clicked), win);
    g_signal_connect(step, "clicked", G_CALLBACK(step_clicked), win);
    g_signal_connect(recents, "clicked", G_CALLBACK(open_clicked), win);
    g_signal_connect(stop, "clicked", G_CALLBACK(stop_clicked), win);
    g_signal_connect(save, "clicked", G_CALLBACK(save_clicked), win);
    g_signal_connect(step_over, "clicked", G_CALLBACK(step_over_clicked), win);
    g_signal_connect(recents_chooser, "item-activated", G_CALLBACK(open_item), win);
}

static modified_changed(GtkTextBuffer *textbuffer,
                        gpointer user_data)
{
    emu_8086_app_window_upd(EMU_8086_APP_WINDOW(user_data));
}

static void populate_win(Emu8086AppWindow *win)
{

    PRIV;
    gtk_window_set_default_size(GTK_WINDOW(win), 800, 600);

    GtkWidget *scrolled;
    GtkWidget *code;
    int be = 0;

#ifdef __linux__
    gtk_window_set_icon_name(GTK_WINDOW(win), "emu8086");
#endif

#ifdef _WIN32
    GError *error = NULL;

    GdkPixbuf *pic = gdk_pixbuf_new_from_resource("/com/krc/emu8086app/pics/emu8086.png", error);
    if (error == NULL)
        gtk_window_set_icon(GTK_WINDOW(win), pic);
    else
        g_debug(error->message);

    if (error != NULL)
        g_error_free(error);
#endif

    scrolled = GTK_SCROLLED_WINDOW(priv->scrolled);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_hexpand(scrolled, TRUE);
    gtk_widget_set_vexpand(scrolled, TRUE);

    code = create_new(win);

    gtk_text_view_set_editable(GTK_TEXT_VIEW(code), TRUE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(code), TRUE);
    gtk_widget_set_hexpand(code, TRUE);
    gtk_widget_set_vexpand(code, FALSE);
    gtk_container_add(GTK_CONTAINER(scrolled), code);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(code));
    gtk_widget_show_all(scrolled);

    priv->code = code;
    // GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
    g_signal_connect(buffer, "modified-changed", G_CALLBACK(modified_changed), win);
    // priv->scrolled = scrolled;
    priv->tos = 0;
    strcpy(win->state.file_name, "Untitled.asm");
    win->state.isSaved = TRUE;
    win->state.file_path_set = FALSE;
    win->state.fontSize = 16;
    gtk_window_set_title(GTK_WINDOW(win), win->state.file_name);
    g_settings_bind(priv->settings, "theme", win, "theme", G_SETTINGS_BIND_GET);

    GtkClipboard *clipboard;
    clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_clipboard_set_can_store(clipboard, NULL, 0);
}

void emu_8086_app_window_up(Emu8086AppWindow *win)
{
    PRIV;
    populate_win(win);
    populate_tools(win);

    win->state.Open = FALSE;

    //  g_print("herre %s\n", "string");
}

char *milli(GtkWidget *label, char *r, int reg)
{
    char buf[9];
    sprintf(buf, "%s: %04x", r, reg);
    gtk_label_set_text(GTK_LABEL(label), buf);
}

static void emu_8086_app_window_update_wids(Emu8086AppCodeRunner *runner, gpointer user_data)
{
    Emu8086AppWindow *win;
    win = EMU_8086_APP_WINDOW(user_data);
    struct emu8086 *aCPU;
    aCPU = getCPU(runner);
    PRIV;
    milli(priv->AX_, "AX", AX);
    milli(priv->BX_, "BX", BX);

    milli(priv->CX_, "CX", CX);
    milli(priv->DX_, "DX", DX);
    milli(priv->SP_, "SP", SP);
    milli(priv->BP_, "BP", BP);

    milli(priv->DI_, "DI", DI);
    milli(priv->SI_, "SI", SI);
    milli(priv->CS_, "CS", CS);
    milli(priv->DS_, "DS", DS);
    milli(priv->ES_, "ES", ES);

    milli(priv->SS_, "SS", _SS);
    milli(priv->ip, "IP", IP);
    milli(priv->FLAGS_, "FL", FLAGS);
    if (_INSTRUCTIONS != NULL)
    {
        char buf[20];
        sprintf(buf, "ON LINE %d", _INSTRUCTIONS->line_number);
        gtk_label_set_text(GTK_LABEL(priv->messages), buf);
        select_line(priv->code, _INSTRUCTIONS->line_number - 1);
    }
    else
    {

        select_line(priv->code, 0);
    }
    // // sprintf(buf, "SS: %04x", _SS);
    // gtk_label_set_text(GTK_LABEL(priv->SS_), buf);
};

void emu_8086_app_window_set_app(Emu8086AppWindow *win, Emu8086App *app)
{
    PRIV;
    priv->app = app;
}
static gboolean clear_message(gpointer user_data)
{
    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(user_data);
    PRIV;

    gtk_label_set_text(GTK_LABEL(priv->messages), "  ");
    //
    priv->tos = 0;
    return 0;
    return G_SOURCE_REMOVE;
}

static void emu_8086_app_window_flash2(Emu8086AppWindow *win, gchar *message)
{
    PRIV;
    gtk_label_set_text(GTK_LABEL(priv->messages), message);

    if (priv->tos != 0)
    {
        g_source_remove(priv->tos);
    }
    priv->tos = gdk_threads_add_timeout_full(G_PRIORITY_LOW,
                                             1000,
                                             clear_message,
                                             win,
                                             NULL);
}

static void emu_8086_app_window_flash(Emu8086AppCodeRunner *runner, gpointer user_data)
{
    // exit(1);
    Emu8086AppWindow *win;
    win = EMU_8086_APP_WINDOW(user_data);
    PRIV;

    gtk_label_set_text(GTK_LABEL(priv->messages), priv->runner->priv->em);

    if (priv->tos != 0)
    {
        g_source_remove(priv->tos);
    }
    priv->tos = gdk_threads_add_timeout_full(G_PRIORITY_LOW,
                                             1000,
                                             clear_message,
                                             win,
                                             NULL);
}

static void reset_win(Emu8086AppCodeRunner *runner, gpointer user_data)
{
    // exit(1);
    Emu8086AppWindow *win;
    win = EMU_8086_APP_WINDOW(user_data);
    PRIV;
    // gtk_label_set_text(GTK_LABEL(priv->messages), "ENDED");

    reset_code(priv->code);
    emu_8086_window_set_memory(win, FALSE);
    milli(priv->AX_, "AX", 0);
    milli(priv->BX_, "BX", 0);

    milli(priv->CX_, "CX", 0);
    milli(priv->DX_, "DX", 0);
    milli(priv->SP_, "SP", 0);
    milli(priv->BP_, "BP", 0);

    milli(priv->DI_, "DI", 0);
    milli(priv->SI_, "SI", 0);
    milli(priv->CS_, "CS", 0);
    milli(priv->DS_, "DS", 0);
    milli(priv->ES_, "ES", 0);

    milli(priv->SS_, "SS", 0);
    milli(priv->ip, "IP", 0);
    milli(priv->FLAGS_, "FL", 0);
}

void emu_8086_app_window_open(Emu8086AppWindow *win, GFile *file)
{
    PRIV;
    gchar *fname, *base, *con;
    gsize len;
    fname = g_file_get_path(file);
    win->state.Open = TRUE;
    populate_win(win);
    populate_tools(win);
    // priv->fname = fname;
    base = g_file_get_basename(file);
    strcpy(win->state.file_name, base);
    if (!check(base))
    {
        char err[256];
        sprintf(err, "unsupported file\n %s", base);
        quick_message(GTK_WINDOW(win), err, "Error");
        g_free(base);
        g_free(fname);
        return;
    }
    strcpy(win->state.file_name, base);
    strcpy(win->state.file_path, fname);
    g_free(fname);
    g_free(base);
    // priv->fname   // //  gtk_header_bar_set_title(GTK_HEADER_BAR(priv->head_bar), base);
    gtk_window_set_title(GTK_WINDOW(win), win->state.file_name);

    if (g_file_load_contents(file, NULL, &con, &len, NULL, NULL))
    {

        // memcpy
        GtkTextBuffer *buf;
        buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
        set_fname(priv->runner, win->state.file_path);
        emu_8086_app_window_set_open(win);
        gtk_text_buffer_set_text(buf, con, len);
        // update(buf, EMU_8086_APP_CODE(priv->code));

        // g_free(contents);
        win->state.isSaved = TRUE;
        win->state.file_path_set = TRUE;
        g_free(con);
    }

    else
    {
        char err[256];
        sprintf(err, "Cannot open file\n %s", win->state.file_path);
        quick_message(GTK_WINDOW(win), err, "Error");
    }
    // g_free(base);
}
void emu_8086_app_window_set_open(Emu8086AppWindow *win)
{

    win->priv->open = 1;
}

void unsetOpen(Emu8086AppWindow *win)
{

    win->priv->open = 0;
}

void emu_8086_app_window_stop_win(Emu8086AppWindow *win)
{
    stop(win->priv->runner, FALSE);
}

GtkWidget *emu8086_get_stack(Emu8086AppWindow *win)
{
    return win->priv->stack;
}

gboolean emu_8086_app_window_open_egs(Emu8086AppWindow *win)
{
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;
    GtkWindow *window = GTK_WINDOW(win);
    gboolean found;
    dialog = gtk_file_chooser_dialog_new("Open File",
                                         window,
                                         action,
                                         ("_Cancel"),
                                         GTK_RESPONSE_CANCEL,
                                         "_Open",
                                         GTK_RESPONSE_ACCEPT,
                                         NULL);
    found = gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), "/usr/local/share/emu8086/egs");

    if (!found)
        found = gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), "/usr/share/emu8086/egs");

    if (found)
    {
        gint res = gtk_dialog_run(GTK_DIALOG(dialog));
        if (res == GTK_RESPONSE_ACCEPT)
        {
            PRIV;
            char *filename, *base;
            gchar *contents;
            gsize length;
            // char *filename;
            GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
            filename = gtk_file_chooser_get_filename(chooser);
            GFile *file = gtk_file_chooser_get_file(chooser);

            base = g_file_get_basename(file);
            if (!check(base))
            {
                char err[256];
                sprintf(err, "unsupported file\n %s", base);
                quick_message(GTK_WINDOW(win), err, "Error");
                g_free(base);
                free(filename);
                gtk_widget_destroy(dialog);
                return;
            }

            gchar *uri;

            strcpy(win->state.file_name, base);
            //win->state.file_name[strlen(base) - 1] = '\0';
            gtk_window_set_title(GTK_WINDOW(win), win->state.file_name);

            strcpy(win->state.file_path, filename);

            //g_file_get_contents(filename, )
            if (g_file_load_contents(file, NULL, &contents, &length, NULL, NULL))
            {
                GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));

                refreshLines(EMU_8086_APP_CODE_BUFFER(buffer));
                uri = gtk_file_chooser_get_uri(chooser);
                add_recent(uri);
                g_free(uri);
                emu_8086_app_window_set_open(win);

                emu_8086_app_window_set_open(win);
                set_fname(priv->runner, win->state.file_path);
                gtk_text_buffer_set_text(buffer, contents, length);
                // update(buffer, EMU_8086_APP_CODE(priv->code));
                strcpy(win->state.file_name, base);
                //win->state.file_name[strlen(base) - 1] = '\0';
                gtk_window_set_title(GTK_WINDOW(win), win->state.file_name);

                // strcpy(win->state.file_path, filename);
                g_free(contents);
                win->state.isSaved = TRUE;
                win->state.file_path_set = TRUE;
            }

            free(filename);
            free(base);
        }
    }
    gtk_widget_destroy(dialog);
}