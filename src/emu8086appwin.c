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

#ifdef HAVE_CONFIG_H
#include <config.h>
#else
#define PACKAGE "/usr/local/lib"
#define DATADIR "/usr/local/share"
#endif

#include <emu8086appwin.h>
#include <emu8086app.h>
#include <emu8086appmemorywin.h>

#include <emu8086stylescheme.h>
#include <emu8086searchbar.h>
#include <emu8086appcode.h>
#include <emu8086appsidepane.h>
#include <emu8086apperrtextview.h>

#include <emu8086appcodebuffer.h>
#include <emu8086apprunner.h>
#include <emu8086apppluginbox.h>
// #include <emu8086appplugins_engine.h>
#include <libpeas/peas-activatable.h>
#include <libpeas/peas-extension-set.h>

typedef enum
{
    PROP_0,
    PROP_UPDATES,
    PROP_MEM,
    PROP_THEME,
    PROP_UL,
    PROP_LF,
    PROP_SEARCH_SHOW,
    PROP_BP
} Emu8086AppWindowProperty;

typedef struct _Emu8086AppWindowPrivate Emu8086AppWindowPrivate;
struct _Emu8086AppWindowPrivate
{
    PeasExtensionSet *exten_set;

    guint tos;
    gboolean bottom_notebook_visible;
    GtkWidget *stack;
    GtkWidget *left_box;
    GtkWidget *tool_bar;
    Emu8086AppMemoryWindow *mem_view;
    Emu8086AppCode *code;
    Emu8086AppCodeRunner *runner;
    GtkWidget *expander;
    GtkWidget *messages; //GtkWidget *left_box;
    gchar *fname;
    GtkWidget *revealer;
    GtkWidget *window_m;
    GtkWidget *scrolled;
    GtkWidget *gears;
    Emu8086App *app;
    GSettings *settings;
    gint open;
    Emu8086AppStyleScheme *scheme;
    gboolean ul;
    gchar *lf;
    GtkWidget *revealer_search;
    GtkWidget *editor_box;
    GtkWidget *vpaned;
    GtkWidget *hpaned;
    GtkWidget *toggle_btn2;
    GtkWidget *bottom_bar;
    Emu8086AppSearchBar *search_bar;
    gboolean search_show;
    GtkWidget *err_messages;
    Emu8086AppErrTextView *err_text_view;
    GtkWidget *toggle_btn;
    GtkWidget *left_pane;
    GtkTextBuffer *err_buffer;
    GtkWidget *bottom_notebook;
    gint vpanedl_size;
    gboolean errs_cleared;
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
G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppWindow, emu8086_app_window, GTK_TYPE_APPLICATION_WINDOW);

static void emu8086_app_window_update_wids(Emu8086AppCodeRunner *runner, gpointer data);
static void reset_win(Emu8086AppCodeRunner *runner, gpointer user_data);
static gboolean check(gchar *n);
gboolean save_new(Emu8086AppWindow *win, gchar *file_name, char *buf);
static void emu8086_app_window_show_errors(Emu8086AppCodeRunner *runner, gint errors, gpointer user_data);
static void emu8086_app_window_flash2(Emu8086AppWindow *win, gchar *message);
static void add_recent(gchar *uri);
static void emu8086_app_window_quick_message(GtkWindow *parent, gchar *message, gchar *title);
static void emu8086_win_change_theme(Emu8086AppStyleScheme *scheme, Emu8086AppWindow *win);
static void emu8086_window_set_search(Emu8086AppWindow *win, gboolean b);
static void unset_open(Emu8086AppWindow *win);
static void
copy_activated(GSimpleAction *action,
               GVariant *parameter,
               gpointer appe);

static void
copy_activated(GSimpleAction *action,
               GVariant *parameter,
               gpointer appe)
{
    Emu8086AppWindow *win = EMU8086_APP_WINDOW(appe);
    PRIV;
    GtkTextBuffer *buffer;
    buffer = (gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code)));
    if (gtk_text_buffer_get_has_selection(buffer))
    {
        GtkClipboard *clipboard;
        clipboard = gtk_widget_get_clipboard(GTK_WIDGET(priv->code), GDK_SELECTION_CLIPBOARD);
        gtk_text_buffer_copy_clipboard(buffer, clipboard);
    }
}

static void
find_activated(GSimpleAction *action,
               GVariant *parameter,
               gpointer appe)
{
    Emu8086AppWindow *win = EMU8086_APP_WINDOW(appe);
    emu8086_window_set_search(win, TRUE);
}

static void
paste_activated(GSimpleAction *action,
                GVariant *parameter,
                gpointer appe)
{
    Emu8086AppWindow *win = EMU8086_APP_WINDOW(appe);
    PRIV;
    GtkTextBuffer *buffer;
    buffer = (gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code)));

    GtkClipboard *clipboard;
    clipboard = gtk_widget_get_clipboard(GTK_WIDGET(priv->code), GDK_SELECTION_CLIPBOARD);

    gtk_text_buffer_paste_clipboard(buffer, clipboard, NULL, TRUE);
}

static void
select_all_activated(GSimpleAction *action,
                     GVariant *parameter,
                     gpointer appe)
{
    Emu8086AppWindow *win = EMU8086_APP_WINDOW(appe);
    PRIV;
    GtkTextBuffer *buffer;
    buffer = (gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code)));
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
    Emu8086AppWindow *win = EMU8086_APP_WINDOW(appe);
    PRIV;
    GtkTextBuffer *buffer;
    buffer = (gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code)));
    if (gtk_text_buffer_get_has_selection(buffer))
    {
        GtkClipboard *clipboard;
        clipboard = gtk_widget_get_clipboard(GTK_WIDGET(priv->code), GDK_SELECTION_CLIPBOARD);
        gtk_text_buffer_cut_clipboard(buffer, clipboard, TRUE);
    }
}

static void
redo_activated(GSimpleAction *action,
               GVariant *parameter,
               gpointer appe)
{
    Emu8086AppWindow *win = EMU8086_APP_WINDOW(appe);
    PRIV;
    emu8086_app_code_redo(priv->code);
}

static void
undo_activated(GSimpleAction *action,
               GVariant *parameter,
               gpointer appe)
{
    Emu8086AppWindow *win = EMU8086_APP_WINDOW(appe);
    PRIV;
    emu8086_app_code_undo(priv->code);
}

static GActionEntry win_entries[] = {
    {"find", find_activated, NULL, NULL, NULL},
    {"copy", copy_activated, NULL, NULL, NULL},

    {"cut", cut_activated, NULL, NULL, NULL},

    {"paste", paste_activated, NULL, NULL, NULL},

    {"select", select_all_activated, NULL, NULL, NULL},
    {"redo", redo_activated, NULL, NULL, NULL},
    {"undo", undo_activated, NULL, NULL, NULL}

};

static void
close_search(GtkButton *button,
             gpointer appe)
{
    Emu8086AppWindow *win = EMU8086_APP_WINDOW(appe);
    emu8086_window_set_search(win, FALSE);
    emu8086_app_search_bar_clear_out(win->priv->search_bar);
}

Emu8086AppWindow *emu8086_app_window_new(Emu8086App *app)
{

    //  emu8086_app_get_type()
    return g_object_new(EMU8086_APP_WINDOW_TYPE, "application", app,
                        NULL);
};

static void open_memory(Emu8086AppWindow *win, gboolean b)
{
    win->memory = b;
    PRIV;
    gtk_revealer_set_reveal_child(GTK_REVEALER(priv->revealer), b);
}
static void emu8086_window_set_search(Emu8086AppWindow *win, gboolean b)
{

    g_return_if_fail(EMU8086_IS_APP_WINDOW(win));
    win->priv->search_show = b;
    g_object_notify(G_OBJECT(win), "search-show");
}
void emu8086_window_set_memory(Emu8086AppWindow *win, gboolean b)
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
    g_return_val_if_fail(EMU8086_IS_APP_WINDOW(target_window), NULL);

    return EMU8086_APP_WINDOW(target_window);
}

void emu8086_app_window_open_drag_data(Emu8086AppWindow *win, GtkSelectionData *selection_data)
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

        emu8086_app_open_file(priv->app, g_file_new_for_uri(fname));
        /* code */
    }

    base = g_file_get_basename(file);

    if (!check(base))
    {
        char err[256];
        // buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));

        sprintf(err, "unsupported file %s", base);
        emu8086_app_window_quick_message(GTK_WINDOW(win), err, "Error");
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
        refreshLines(EMU8086_APP_CODE_BUFFER(buffer));
        // uri = gtk_file_chooser_get_uri(chooser);
        add_recent(uri);
        // g_free(uri);
        emu8086_app_window_set_open(win);
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
    emu8086_app_window_open_drag_data(win, selection_data);
    gtk_drag_finish(context, TRUE, FALSE, timestamp);
}

static GtkWidget *
build_tab_label(GtkWidget *item,
                const gchar *name,
                GtkWidget *icon)
{
    GtkWidget *hbox, *label_hbox, *label_ebox;
    GtkWidget *label;

    /* set hbox spacing and label padding (see below) so that there's an
	 * equal amount of space around the label */
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);

    label_ebox = gtk_event_box_new();
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(label_ebox), FALSE);
    gtk_box_pack_start(GTK_BOX(hbox), label_ebox, TRUE, TRUE, 0);

    label_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 4);
    gtk_container_add(GTK_CONTAINER(label_ebox), label_hbox);

    /* setup icon */
    gtk_box_pack_start(GTK_BOX(label_hbox), icon, FALSE, FALSE, 0);

    /* setup label */
    label = gtk_label_new(name);
    gtk_label_set_xalign(GTK_LABEL(label), 0.0);
    gtk_widget_set_margin_start(label, 0);
    gtk_widget_set_margin_end(label, 0);
    gtk_widget_set_margin_top(label, 0);
    gtk_widget_set_margin_bottom(label, 0);
    gtk_box_pack_start(GTK_BOX(label_hbox), label, TRUE, TRUE, 0);

    gtk_widget_set_tooltip_text(label_ebox, name);

    gtk_widget_show_all(hbox);

    g_object_set_data(G_OBJECT(item), "label", label);
    g_object_set_data(G_OBJECT(item), "hbox", hbox);

    return hbox;
}

void emu8086_app_window_bottom_notebook_add_item(Emu8086AppWindow *win, GtkWidget *item,
                                                 const gchar *name,
                                                 GtkWidget *image,
                                                 const gchar *icon_name)
{
    GtkWidget *tab_label;
    GtkWidget *menu_label;
    GtkWidget *im;
    gint w, h;
    im = image;
    if (im == NULL)
    {

        im = gtk_image_new_from_icon_name(icon_name,
                                          GTK_ICON_SIZE_MENU);
    }
    gtk_icon_size_lookup(GTK_ICON_SIZE_MENU, &w, &h);
    gtk_widget_set_size_request(im, w, h);
    tab_label = build_tab_label(item, name, im);

    menu_label = gtk_label_new(name);
    gtk_label_set_xalign(GTK_LABEL(menu_label), 0.0);

    if (!gtk_widget_get_visible(item))
        gtk_widget_show_all(item);
    gtk_notebook_append_page_menu(GTK_NOTEBOOK(win->priv->bottom_notebook),
                                  item,
                                  tab_label,
                                  menu_label);
}

static void emu8086_app_window_clear_err_msgs(Emu8086AppWindow *win)
{
    PRIV;
    if (priv->errs_cleared)
        return;
    gtk_text_buffer_set_text(priv->err_buffer, "Nothing Doing\n Errors: 0", -1);
    gtk_button_set_label(GTK_BUTTON(priv->toggle_btn), "No Errors");
    gtk_widget_show(priv->toggle_btn);
    gtk_widget_hide(priv->toggle_btn2);
    priv->errs_cleared = TRUE;
}
static void emu8086_app_window_toggle_bb(GtkButton *button, Emu8086AppWindow *win)
{
    PRIV;
    emu8086_app_window_set_bottom_pane(win, !win->priv->bottom_notebook_visible);
}
static void
vpanedl_size_allocate(GtkWidget *widget,
                      GtkAllocation *allocation,
                      Emu8086AppWindow *window)
{
    window->priv->vpanedl_size = allocation->height;
}

static void
vpaned_restore_position(GtkWidget *widget,
                        Emu8086AppWindow *window)
{
    GtkAllocation allocation;
    gint pos;

    gtk_widget_get_allocation(widget, &allocation);

    gtk_widget_get_allocation(widget, &allocation);
    pos = allocation.height -
          MAX(50, window->priv->vpanedl_size);

    gtk_paned_set_position(GTK_PANED(window->priv->vpaned), pos);
    g_signal_connect(window->priv->bottom_notebook,
                     "size-allocate",
                     G_CALLBACK(vpanedl_size_allocate),
                     window);

    g_signal_handlers_disconnect_by_func(widget, vpaned_restore_position, window);
}

static void populate_bottom_bar(Emu8086AppWindow *win)
{
    PRIV;
    GtkWidget *icon;
    GtkWidget *icon2;
    GtkWidget *icon3;
    GtkWidget *btn_widget;
    gchar *path, *path2, *path3;
    path = g_build_filename(DATADIR, PACKAGE, "pics/good.svg", NULL);
    path3 = g_build_filename(DATADIR, PACKAGE, "pics/errors.svg", NULL);
    path2 = g_build_filename(DATADIR, PACKAGE, "pics/memory.svg", NULL);

    icon = gtk_image_new_from_file(path);
    icon2 = gtk_image_new_from_file(path2);
    icon3 = gtk_image_new_from_file(path3);
    priv->runner = emu8086_app_code_runner_new(NULL, FALSE);
    priv->mem_view = emu8086_app_memory_window_open(GTK_WINDOW(win), priv->runner);
    priv->toggle_btn = gtk_button_new();
    priv->toggle_btn2 = gtk_button_new();
    btn_widget = gtk_button_new();
    gtk_widget_set_tooltip_text(priv->toggle_btn, "Toggle bottom panel");
    gtk_widget_set_tooltip_text(priv->toggle_btn2, "Toggle bottom panel");
    GtkButton *btn, *btn2, *btn3;
    btn = GTK_BUTTON(priv->toggle_btn);
    btn2 = GTK_BUTTON(btn_widget);
    btn3 = GTK_BUTTON(priv->toggle_btn2);
    gtk_widget_set_tooltip_text(btn_widget, "Toggle Memory");
    gtk_button_set_image(btn, icon);
    gtk_button_set_image(btn2, icon2);
    gtk_button_set_image(btn3, icon3);
    gtk_button_set_image_position(btn, GTK_POS_LEFT);
    gtk_button_set_always_show_image(btn, TRUE);
    gtk_button_set_relief(btn, GTK_RELIEF_NONE);
    gtk_button_set_image_position(btn3, GTK_POS_LEFT);
    gtk_button_set_always_show_image(btn3, TRUE);
    gtk_button_set_relief(btn3, GTK_RELIEF_NONE);
    gtk_button_set_relief(btn2, GTK_RELIEF_NONE);
    gtk_widget_show(priv->toggle_btn);
    gtk_widget_show(priv->toggle_btn2);
    gtk_widget_show(btn_widget);
    gtk_container_add(GTK_CONTAINER(priv->left_box), priv->toggle_btn);
    gtk_container_add(GTK_CONTAINER(priv->left_box), priv->toggle_btn2);
    gtk_container_add(GTK_CONTAINER(priv->left_box), btn_widget);
    g_signal_connect(priv->toggle_btn, "clicked",
                     G_CALLBACK(emu8086_app_window_toggle_bb), win);
    g_signal_connect(priv->toggle_btn2, "clicked",
                     G_CALLBACK(emu8086_app_window_toggle_bb), win);
    g_signal_connect(btn_widget, "clicked", G_CALLBACK(emu8086_app_memory_window_close), priv->mem_view);
    gtk_label_set_text(GTK_LABEL(priv->messages), "Version 1.0.2(Beta)");
    g_free(path);
    g_free(path2);
    g_free(path3);
}

static void load_vpaned(Emu8086AppWindow *win)
{
    GtkWidget *sv;
    PRIV;
    priv->bottom_notebook = gtk_notebook_new();
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(priv->bottom_notebook), GTK_POS_BOTTOM);
    priv->revealer = GTK_WIDGET(emu8086_app_side_pane_new(GTK_APPLICATION_WINDOW(win)));
    gtk_revealer_set_transition_type(GTK_REVEALER(priv->revealer), GTK_REVEALER_TRANSITION_TYPE_SLIDE_RIGHT);
    gtk_widget_show_all(priv->revealer);

    priv->err_messages = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);

    priv->err_buffer = gtk_text_buffer_new(NULL);
    priv->err_text_view = emu8086_app_err_text_view_new(); //
    emu8086_app_err_set_win(priv->err_text_view, win);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(priv->err_text_view), priv->err_buffer);

    sv = gtk_scrolled_window_new(NULL, NULL);

    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(sv), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    gtk_container_add(GTK_CONTAINER(sv), GTK_WIDGET(priv->err_text_view));
    gtk_container_add(GTK_CONTAINER(priv->err_messages), sv);

    gtk_widget_set_vexpand(sv, TRUE);
    emu8086_app_window_bottom_notebook_add_item(win, priv->err_messages, "Errors", NULL, "gtk-close");
    gtk_widget_set_size_request(priv->err_messages, -1, 200);
    //    gtk_paned_pack1 (GTK_PANED(priv->vpaned), frame1, TRUE,
    // 		 TRUE); gtk_widget_show_all(priv->vpaned);
    gtk_paned_pack2(GTK_PANED(priv->vpaned), priv->bottom_notebook, FALSE,
                    FALSE);
    // gtk_widget_show_all(priv->vpaned);

    g_signal_connect_after(priv->vpaned,
                           "map",
                           G_CALLBACK(vpaned_restore_position),
                           win);
    return;
}

static void emu8086_app_window_init(Emu8086AppWindow *win)
{
    GtkBuilder *builder;
    GtkTargetList *tl;
    GMenuModel *menu;
    gtk_widget_init_template(GTK_WIDGET(win));
    win->priv = emu8086_app_window_get_instance_private(win);
    
    PRIV;
    priv->errs_cleared = FALSE;
   
    priv->stack = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    priv->scrolled = gtk_scrolled_window_new(NULL, NULL);
    priv->vpaned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    priv->hpaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    priv->left_pane = gtk_grid_new();
    gtk_paned_pack1(GTK_PANED(priv->vpaned),
                    priv->scrolled,
                    TRUE,
                    TRUE);

    load_vpaned(win);
    populate_bottom_bar(win); //
    emu8086_app_window_clear_err_msgs(win);

    gtk_container_add(GTK_CONTAINER(priv->editor_box), priv->hpaned);
       gtk_paned_pack1(GTK_PANED(priv->hpaned),
                    priv->left_pane,
                    FALSE,
                    TRUE);

    gtk_paned_pack2(GTK_PANED(priv->hpaned),
                    priv->stack,
                    TRUE,
                    TRUE);
    gtk_container_add(GTK_CONTAINER(priv->stack), priv->revealer);
    gtk_container_add(GTK_CONTAINER(priv->stack), priv->vpaned);
    gtk_widget_show_all(priv->stack);
    gtk_widget_set_visible(priv->bottom_notebook, FALSE);

    priv->settings = g_settings_new("com.krc.emu8086app");
    GAction *action, *action2, *action3;

    priv->scheme = emu8086_app_style_scheme_get_default();
    g_settings_bind(priv->settings, "ul", win, "ul", G_SETTINGS_BIND_GET);
    g_settings_bind(priv->settings, "lf", win, "lf", G_SETTINGS_BIND_GET);
    action = (GAction *)g_property_action_new("check-updates", win, "updates");
    action2 = (GAction *)g_property_action_new("open_mem", win, "memory");
    action3 = (GAction *)g_property_action_new("open_bottom_pane", win, "bottom-pane");

    g_action_map_add_action(G_ACTION_MAP(win), action);
    g_action_map_add_action(G_ACTION_MAP(win), action2);
    g_action_map_add_action(G_ACTION_MAP(win), action3);
    g_object_unref(action);
    g_object_unref(action2);
    g_action_map_add_action_entries(G_ACTION_MAP(win),
                                    win_entries, G_N_ELEMENTS(win_entries),
                                    win);

    builder = gtk_builder_new_from_resource("/com/krc/emu8086app/ui/gears.ui");
    menu = G_MENU_MODEL(gtk_builder_get_object(builder, "menu"));
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

    g_signal_connect(priv->runner, "error_occured", G_CALLBACK(emu8086_app_window_show_errors), win);
    g_signal_connect(priv->runner, "exec_ins", G_CALLBACK(emu8086_app_window_update_wids), win);

    Emu8086AppPluginBox *box;
    box = emu8086_app_plugin_box_new(GTK_APPLICATION_WINDOW(win), priv->runner);
    g_object_bind_property(win, "search-show", priv->revealer_search, "reveal-child", G_BINDING_DEFAULT);

    gtk_container_add(GTK_CONTAINER(priv->stack), GTK_WIDGET(box));
    gtk_widget_set_size_request(priv->left_pane, 250, -1);
     gtk_widget_show_all(priv->hpaned);
     gtk_widget_hide(priv->bottom_notebook);
     gtk_widget_hide(priv->left_pane);

    g_signal_connect(priv->scheme, "theme_changed", G_CALLBACK(emu8086_win_change_theme), win);
    g_object_unref(builder); priv->bottom_notebook_visible = FALSE;
};

void emu8086_app_window_quick_message(GtkWindow *parent, gchar *message, gchar *title)
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
                                         GTK_WINDOW(win),
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
            emu8086_app_window_quick_message(GTK_WINDOW(win), err, "Error");
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

            refreshLines(EMU8086_APP_CODE_BUFFER(buffer));
            uri = gtk_file_chooser_get_uri(chooser);
            add_recent(uri);
            g_free(uri);
            emu8086_app_window_set_open(win);

            emu8086_app_window_set_open(win);
            set_fname(priv->runner, win->state.file_path);
            gtk_text_buffer_set_text(buffer, contents, length);
            // update(buffer, EMU8086_APP_CODE(priv->code));
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
emu8086_window_key_press_event(GtkWidget *widget,
                               GdkEventKey *event)
{
    static gpointer grand_parent_class = NULL;
    gboolean handled = FALSE;
    GtkWindow *window = GTK_WINDOW(widget);
    Emu8086AppWindow *win = EMU8086_APP_WINDOW(window);
    PRIV;
    if (event->state & GDK_CONTROL_MASK)
    {
        if ((event->keyval == GDK_KEY_plus) || (event->keyval == GDK_KEY_KP_Add) || (event->keyval == GDK_KEY_equal))
        {
            handled = TRUE;

            if (win->state.fontSize < 300)
            {
                win->state.fontSize++;
                editFontSize(priv->code, 1);
            }

            else
            {
                emu8086_app_window_flash2(win, "Max font size reached");
            }
        }

        else if ((event->keyval == GDK_KEY_minus) || (event->keyval == GDK_KEY_KP_Subtract))
        {
            handled = TRUE;
            Emu8086AppWindow *win = EMU8086_APP_WINDOW(window);
            PRIV;
            if (win->state.fontSize > -100)
            {
                win->state.fontSize--;
                editFontSize(priv->code, -1);
            }

            else
            {
                emu8086_app_window_flash2(win, "Min font size reached");
            }
        }
        else if ((event->keyval == GDK_KEY_s))
        {
            handled = TRUE;
            emu8086_app_window_save_doc(EMU8086_APP_WINDOW(widget));
        }

        else if ((event->keyval == GDK_KEY_F) || (event->keyval == GDK_KEY_f))
        {
            handled = TRUE;
            emu8086_window_set_search(EMU8086_APP_WINDOW(widget), TRUE);
        }
        else if ((event->keyval == GDK_KEY_G) || (event->keyval == GDK_KEY_g))
        {
            handled = TRUE;
            emu8086_app_window_set_bottom_pane(win, !win->priv->bottom_notebook_visible);
        }

        // else if( )
    }
    if (grand_parent_class == NULL)
        grand_parent_class = g_type_class_peek_parent(emu8086_app_window_parent_class);

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
void emu8086_app_window_open_activate_cb(Emu8086AppWindow *win)
{
    _open(win);
}

void emu8086_app_window_save_activate_cb(Emu8086AppWindow *win)
{

    emu8086_app_window_save_doc(win);
    // gtk_application_ex
}
void emu8086_app_window_save_as_activate_cb(Emu8086AppWindow *win)
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
        emu8086_app_window_write_to_file(win->state.file_path, con, win->state.file_name);
        gtk_window_set_title(GTK_WINDOW(win), win->state.file_name);
        win->state.isSaved = TRUE;
        return;
    };
}

void emu8086_app_window_arr_sum_activate_cb(Emu8086AppWindow *win)
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
    stop(priv->runner, FALSE);
    if (g_file_load_contents(file, NULL, &con, &len, NULL, NULL))
    {

        // memcpy
        GtkTextBuffer *buf;
        buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
        refreshLines(EMU8086_APP_CODE_BUFFER(buf));
        emu8086_app_window_set_open(win);
        set_fname(priv->runner, win->state.file_path);
        gtk_text_buffer_set_text(buf, con, len);
        // update(buf, EMU8086_APP_CODE(priv->code));

        // g_free(contents);
        win->state.isSaved = TRUE;
        // win->state.file_path_set = TRUE;
        g_free(con);
    }
}

void emu8086_app_window_rev_str_activate_cb(Emu8086AppWindow *win)
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
    stop(priv->runner, FALSE);
    if (g_file_load_contents(file, NULL, &con, &len, NULL, NULL))
    {

        // memcpy
        GtkTextBuffer *buf;
        buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
        refreshLines(EMU8086_APP_CODE_BUFFER(buf));
        emu8086_app_window_set_open(win);
        set_fname(priv->runner, win->state.file_path);
        gtk_text_buffer_set_text(buf, con, len);
        // update(buf, EMU8086_APP_CODE(priv->code));

        // g_free(contents);
        win->state.isSaved = TRUE;
        // win->state.file_path_set = TRUE;
        g_free(con);
    }
}

static void emu8086_win_change_theme(Emu8086AppStyleScheme *scheme, Emu8086AppWindow *win)
{
    PRIV;
    gchar *a;
    GdkRGBA color;

    a = emu8086_app_style_scheme_get_color_by_index(priv->scheme, 8);
    gdk_rgba_parse(&color, a);
    gtk_widget_override_background_color(GTK_WIDGET(priv->code), GTK_STATE_NORMAL, &color);
    gtk_widget_override_background_color(GTK_WIDGET(priv->err_text_view), GTK_STATE_NORMAL, &color);
}

static void
emu8086_window_set_property(GObject *object,
                            guint property_id,
                            const GValue *value,
                            GParamSpec *pspec)
{
    Emu8086AppWindow *self = EMU8086_APP_WINDOW(object);
    // g_print("l %d\n", *value);
    gchar *men;
    Emu8086AppWindowPrivate *priv = emu8086_app_window_get_instance_private(self);

    switch ((Emu8086AppWindowProperty)property_id)
    {
    case PROP_UPDATES:
        // *v = (gboolean *)value;

        self->updates = g_value_get_boolean(value);
        // g_print("filename: %s\n", self->filename);
        break;
    case PROP_BP:
        // *v = (gboolean *)value;

        priv->bottom_notebook_visible = g_value_get_boolean(value);
        gtk_widget_set_visible(priv->bottom_notebook, priv->bottom_notebook_visible);
        // g_print("filename: %s\n", self->filename);
        break;

    case PROP_UL:
        // *v = (gboolean *)value;

        priv->ul = g_value_get_boolean(value);
        // g_print("filename: %s\n", self->filename);
        break;

    case PROP_LF:
        // *v = (gboolean *)value;
        men = g_strdup(g_value_get_string(value));

        if (priv->lf != NULL)
            g_free(priv->lf);
        priv->lf = men;

        // g_print("filename: %s\n", self->filename);
        break;

    case PROP_MEM:
        // *v = (gboolean *)value;

        open_memory(self, g_value_get_boolean(value));
        // g_print("filename: %s\n", self->filename);
        break;

    case PROP_SEARCH_SHOW:
        // *v = (gboolean *)value;

        priv->search_show = g_value_get_boolean(value);
        // g_print("filename: %s\n", self->filename);
        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void
emu8086_window_get_property(GObject *object,
                            guint property_id,
                            GValue *value,
                            GParamSpec *pspec)
{
    Emu8086AppWindow *self = EMU8086_APP_WINDOW(object);

    switch ((Emu8086AppWindowProperty)property_id)
    {
    case PROP_UPDATES:
        g_value_set_boolean(value, self->updates);
        break;
    case PROP_BP:
        // *v = (gboolean *)value;

        g_value_set_boolean(value, self->priv->bottom_notebook_visible);
        // g_print("filename: %s\n", self->filename);
        break;

    case PROP_MEM:
        g_value_set_boolean(value, self->memory);
        break;

    case PROP_SEARCH_SHOW:
        g_value_set_boolean(value, self->priv->search_show);
        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void emu8086_app_window_dispose(GObject *object)
{
    Emu8086AppWindow *win;
    win = EMU8086_APP_WINDOW(object);
    PRIV;
    gtk_widget_destroy(GTK_WIDGET(priv->mem_view));

    G_OBJECT_CLASS(emu8086_app_window_parent_class)->dispose(object);
}
static void emu8086_app_window_class_init(Emu8086AppWindowClass *class)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                                                "/com/krc/emu8086app/ui/emu8086.ui");
    GObjectClass *object_class = G_OBJECT_CLASS(class);
    object_class->set_property = emu8086_window_set_property;
    object_class->get_property = emu8086_window_get_property;
    object_class->dispose = emu8086_app_window_dispose;
    widget_class->key_press_event = emu8086_window_key_press_event;
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, gears);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, left_box);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, messages);

    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, editor_box);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, bottom_bar);

    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, tool_bar);

    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, revealer_search);

    g_object_class_install_property(object_class, PROP_THEME,
                                    g_param_spec_string("theme",
                                                        "Theme",
                                                        "The window's theme",
                                                        "dark+",
                                                        G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_LF,
                                    g_param_spec_string("lf",
                                                        "License File",
                                                        "The window's license file",
                                                        NULL,
                                                        G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_UL,
                                    g_param_spec_boolean("ul",
                                                         "UL",
                                                         "The window should use license",
                                                         FALSE,
                                                         G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_UPDATES,
                                    g_param_spec_boolean("updates",
                                                         "Updates",
                                                         "The window's state",
                                                         FALSE,
                                                         G_PARAM_READWRITE));
    g_object_class_install_property(object_class, PROP_BP,
                                    g_param_spec_boolean("bottom-pane",
                                                         "Bottom Pane",
                                                         "If bottom pane visible",
                                                         FALSE,
                                                         G_PARAM_READWRITE));
    g_object_class_install_property(object_class, PROP_MEM,
                                    g_param_spec_boolean("memory",
                                                         "Memory",
                                                         "Toggle memory view",
                                                         FALSE,
                                                         G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_SEARCH_SHOW,
                                    g_param_spec_boolean("search-show",
                                                         "searchShow",
                                                         "Toggle search",
                                                         FALSE,
                                                         G_PARAM_READWRITE));
};

void char_ins(GtkTextView *text_view,
              gchar *string,
              gpointer user_data)
{
    exit(1);
}

void emu8086_app_window_upd(Emu8086AppWindow *win)
{
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(win->priv->code));
    gboolean modified = gtk_text_buffer_get_modified(buffer);
    if (win->state.isSaved && (win->priv->open == 0) && modified)
    {
        char buf[strlen(win->state.file_name) + 2];
        sprintf(buf, "%s*", win->state.file_name);
        gtk_window_set_title(GTK_WINDOW(win), buf);
        win->state.isSaved = FALSE;
    }
    unset_open(win);
    gtk_text_buffer_set_modified(buffer, FALSE);
}

void emu8086_app_window_write_to_file(gchar *filename, gchar *buffer, char *buff)
{

    FILE *f;
    GFile *file;
    file = g_file_new_for_path(filename);
    // f = g_file_open_readwrite(f, );
    char buf[256];
    //g_fi

    f = fopen(filename, "w");
    if (f == NULL)
    {
        sprintf(buf, "Unable to save %s\n", filename);
        g_print("%s", buf);
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
    }
    else
        sprintf(buff, "%s", "Failed to save");
    return;
}

gboolean save_new(Emu8086AppWindow *win, gchar *file_name, char *buf)
{
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;
    gboolean ret = FALSE;
    dialog = gtk_file_chooser_dialog_new("Save File",
                                         GTK_WINDOW(win),
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

gboolean emu8086_app_window_save_doc(Emu8086AppWindow *win)
{
    gchar *fname = win->state.file_path;
    gchar buf[5];
    strncpy(buf, fname, 4);
    buf[4] = '\0';

    if (strcmp(buf, "/usr") == 0)
    {
        return TRUE;
    }
    gtk_text_buffer_set_modified(gtk_text_view_get_buffer(GTK_TEXT_VIEW(win->priv->code)), FALSE);
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
            emu8086_app_window_write_to_file(win->state.file_path, con, win->state.file_name);
            set_fname(priv->runner, win->state.file_path);

            gtk_window_set_title(GTK_WINDOW(win), win->state.file_name);
            win->state.isSaved = TRUE;
            return TRUE;
        }
        else
        {
            if (save_new(win, NULL, win->state.file_path))
            {

                win->state.file_path_set = TRUE;
                emu8086_app_window_write_to_file(win->state.file_path, con, win->state.file_name);
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
    Emu8086AppWindow *win = EMU8086_APP_WINDOW(user_data);
    PRIV;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
    gint c = gtk_text_buffer_get_char_count(buffer);
    if (!emu8086_app_window_save_doc(win))
    {
        return;
    }
    if (c > 0)
    {
        gtk_text_view_set_editable(GTK_TEXT_VIEW(priv->code), FALSE);
        emu8086_window_set_memory(win, TRUE);
        run_clicked_app(priv->runner);
    }
    else
        emu8086_app_window_flash2(win, "Nothing to run");
}

void pause_clicked(GtkToolButton *toolbutton,
                   gpointer user_data)
{
    Emu8086AppWindow *win = EMU8086_APP_WINDOW(user_data);
    PRIV;
    set_app_state(priv->runner, STEP);
    //
}

void step_clicked(GtkToolButton *toolbutton,
                  gpointer user_data)
{
    Emu8086AppWindow *win = EMU8086_APP_WINDOW(user_data);

    if (!emu8086_app_window_save_doc(win))
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
            emu8086_window_set_memory(win, TRUE);
        step_clicked_app(priv->runner);
    }

    else
        emu8086_app_window_flash2(win, "Nothing to run");
}
void step_over_clicked(GtkToolButton *toolbutton,
                       gpointer user_data)
{
    Emu8086AppWindow *win = EMU8086_APP_WINDOW(user_data);
    if (!emu8086_app_window_save_doc(win))
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
            emu8086_window_set_memory(win, TRUE);

        gint bps[100], len;
        len = 0;
        // bps[100];

        step_over_clicked_app(priv->runner, priv->code);
    }
    else
        emu8086_app_window_flash2(win, "Nothing to run");
}
void save_clicked(GtkToolButton *toolbutton,
                  gpointer user_data)
{
    Emu8086AppWindow *win = EMU8086_APP_WINDOW(user_data);
    emu8086_app_window_save_doc(win);
    //
}

void open_clicked(GtkToolButton *toolbutton,
                  gpointer user_data)
{
    Emu8086AppWindow *win = EMU8086_APP_WINDOW(user_data);
    PRIV;
    _open(win);
    //
}
void stop_clicked(GtkToolButton *toolbutton,
                  gpointer user_data)
{
    Emu8086AppWindow *win = EMU8086_APP_WINDOW(user_data);
    PRIV;
    stop_clicked_app(priv->runner);
    // emu8086_window_set_memory(win, FALSE);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(priv->code), TRUE);

    //step_over_clicked_app
}

static void open_item(GtkRecentChooser *recents, gpointer user_data)
{
    Emu8086AppWindow *win = EMU8086_APP_WINDOW(user_data);
    gchar *uri;
    uri = gtk_recent_chooser_get_current_uri(recents);
    GFile *file = NULL;
    char *base, *filename, *contents = NULL;
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
        refreshLines(EMU8086_APP_CODE_BUFFER(buffer));

        strcpy(win->state.file_name, base);
        //win->state.file_name[strlen(base) - 1] = '\0';
        gtk_window_set_title(GTK_WINDOW(win), win->state.file_name);

        strcpy(win->state.file_path, filename);
        add_recent(uri);
        g_free(uri);
        emu8086_app_window_set_open(win);
        set_fname(priv->runner, win->state.file_path);
        gtk_text_buffer_set_text(buffer, contents, length);
        // update(buffer, EMU8086_APP_CODE(priv->code));

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
    GtkToolItem *play, *step, *stop, *pause, *save, *step_over;
    GtkToolItem *sep;
    GtkMenuToolButton *recents;
    GtkToolItem *rec;
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
    rec = gtk_menu_tool_button_new(NULL, NULL);
    recents = GTK_MENU_TOOL_BUTTON(rec);
    gtk_menu_tool_button_set_menu(recents, (recents_chooser));
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(recents), "document-open");
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(recents), ("Open"));
    gtk_recent_chooser_set_local_only(GTK_RECENT_CHOOSER(recents_chooser),
                                      TRUE);
    gtk_tool_item_set_is_important(rec, TRUE);
    gtk_tool_item_set_tooltip_text(rec,
                                   ("Open a file"));
    gtk_menu_tool_button_set_arrow_tooltip_text(recents,
                                                ("Open a recently used file"));

    gtk_widget_show(GTK_WIDGET(recents));
    play = gtk_tool_button_new(NULL, NULL);
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(play), ("Run"));
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(play), "media-playback-start");
    gtk_tool_button_set_use_underline(GTK_TOOL_BUTTON(play), TRUE);
    gtk_tool_item_set_is_important(play, TRUE);
    gtk_widget_show(GTK_WIDGET(play));

    sep = gtk_separator_tool_item_new();
    gtk_tool_item_set_is_important(sep, TRUE);

    gtk_separator_tool_item_set_draw(GTK_SEPARATOR_TOOL_ITEM(sep), TRUE);
    gtk_widget_show(GTK_WIDGET(sep));

    stop = gtk_tool_button_new(NULL, NULL);

    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(stop), "media-playback-stop");
    gtk_widget_show(GTK_WIDGET(stop));
    gtk_tool_item_set_tooltip_text(stop, "Stop Executing");

    pause = gtk_tool_button_new(NULL, NULL);
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(pause), "media-playback-pause");
    gtk_tool_item_set_tooltip_text(pause, "Halt Execution");
    gtk_tool_button_set_use_underline(GTK_TOOL_BUTTON(pause), TRUE);

    gtk_widget_show(GTK_WIDGET(pause));

    step = gtk_tool_button_new(NULL, NULL);
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(step), "media-skip-forward");
    gtk_tool_item_set_tooltip_text(step, "Step to next instruction");
    gtk_tool_button_set_use_underline(GTK_TOOL_BUTTON(step), TRUE);
    gtk_widget_show(GTK_WIDGET(step));

    save = gtk_tool_button_new(NULL, NULL);
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(save), "document-save");
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(save), ("Save"));
    gtk_tool_button_set_use_underline(GTK_TOOL_BUTTON(save), TRUE);
    gtk_tool_item_set_is_important(save, TRUE);
    gtk_widget_show(GTK_WIDGET(save));

    step_over = gtk_tool_button_new(NULL, NULL);
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(step_over), "forward");
    gtk_tool_item_set_tooltip_text(step_over, "Step Over to breakpoint");
    gtk_widget_show(GTK_WIDGET(step_over));

    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, save, 0);
    // gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, open, -1);
    gtk_widget_set_margin_start(GTK_WIDGET(recents), 5);
    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, rec, -1);
    gtk_widget_set_margin_start(GTK_WIDGET(sep), 5);
    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, sep, -1);

    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, play, -1);
    gtk_widget_set_margin_start(GTK_WIDGET(pause), 5);
    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, pause, -1);
    gtk_widget_set_margin_start(GTK_WIDGET(step), 5);
    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, step, -1);
    gtk_widget_set_margin_start(GTK_WIDGET(step_over), 5);
    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, step_over, -1);
    gtk_widget_set_margin_start(GTK_WIDGET(stop), 5);
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

static void modified_changed(GtkTextBuffer *textbuffer,
                             gpointer user_data)
{
    emu8086_app_window_upd(EMU8086_APP_WINDOW(user_data));
}

static void populate_win(Emu8086AppWindow *win)
{

    PRIV;
    gtk_window_set_default_size(GTK_WINDOW(win), 1000, 800);

    GtkWidget *scrolled;
    GtkWidget *code, *btn_close;
    Emu8086AppCode *c;
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

    scrolled = priv->scrolled;
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_hexpand(scrolled, TRUE);
    gtk_widget_set_vexpand(scrolled, TRUE);

    c = create_new(CODE_VIEW);
    code = GTK_WIDGET(c);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(c), TRUE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(c), TRUE);
    gtk_widget_set_hexpand(code, TRUE);
    gtk_widget_set_vexpand(code, FALSE);
    gtk_container_add(GTK_CONTAINER(scrolled), code);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(c));
    gtk_widget_show_all(scrolled);

    priv->code = c;
    btn_close = gtk_button_new_from_icon_name("gtk-close", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_relief(GTK_BUTTON(btn_close), GTK_RELIEF_NONE);
    priv->search_bar = emu8086_app_search_bar_create(GTK_TEXT_VIEW(c));
    gtk_grid_attach(GTK_GRID(priv->search_bar), btn_close, 4, 0, 1, 1);
    gtk_container_add(GTK_CONTAINER(priv->revealer_search), GTK_WIDGET(priv->search_bar));

    gtk_widget_show_all(GTK_WIDGET(priv->search_bar));

    emu8086_win_change_theme(NULL, win);
    // GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
    g_signal_connect(buffer, "modified-changed", G_CALLBACK(modified_changed), win);
    g_signal_connect(btn_close, "clicked", G_CALLBACK(close_search), win);
    // priv->scrolled = scrolled;
    priv->tos = 0;
    strcpy(win->state.file_name, "Untitled.asm");
    win->state.isSaved = TRUE;
    win->state.file_path_set = FALSE;
    win->state.fontSize = 16;
    gtk_window_set_title(GTK_WINDOW(win), win->state.file_name);

    GtkClipboard *clipboard;
    clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    gtk_clipboard_set_can_store(clipboard, NULL, 0);
}

static void load_license(Emu8086AppWindow *win)
{
    PRIV;

    if (priv->ul && strcmp(priv->lf, "none") != 0)
    {
        gchar buf[256];
        FILE *file;
        file = NULL;
        file = fopen(priv->lf, "r");
        if (file == NULL)
            return;
        GString *s;
        s = g_string_new("  ;\n");
        while (fgets(buf, sizeof buf, file))
        {
            /* code */
            gchar *lin = g_strconcat("  ; ", buf, NULL);
            g_string_append(s, lin);
        }
        g_string_append(s,
                        ";--------------------------------------------------------------------------------------------");
        fclose(file);
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
        gtk_text_buffer_set_text(buffer, g_string_free(s, FALSE), -1);
    }
}

void emu8086_app_window_up(Emu8086AppWindow *win)
{
    PRIV;
    populate_win(win);
    populate_tools(win);

    win->state.Open = FALSE;
    load_license(win);
    //  g_print("herre %s\n", "string");
}

static void emu8086_app_window_update_wids(Emu8086AppCodeRunner *runner, gpointer user_data)
{
    Emu8086AppWindow *win;
    win = EMU8086_APP_WINDOW(user_data);
    struct emu8086 *aCPU;
    aCPU = getCPU(runner);
    PRIV;
    emu8086_app_side_pane_update_view(EMU8086_APP_SIDE_PANE(priv->revealer), aCPU->mSFR);
    if (!priv->errs_cleared)
        emu8086_app_window_clear_err_msgs(win);
    if (_INSTRUCTIONS != NULL)
    {

        select_line(GTK_WIDGET(priv->code), _INSTRUCTIONS->line_number - 1);
    }
    else
    {

        select_line(GTK_WIDGET(priv->code), 0);
    }
};

void emu8086_app_window_set_app(Emu8086AppWindow *win, Emu8086App *app)
{
    PRIV;
    priv->app = app;
}
static gboolean clear_message(gpointer user_data)
{
    Emu8086AppWindow *win = EMU8086_APP_WINDOW(user_data);
    PRIV;

    gtk_label_set_text(GTK_LABEL(priv->messages), "  ");
    //
    priv->tos = 0;
    return 0;
    return G_SOURCE_REMOVE;
}

static void emu8086_app_window_flash2(Emu8086AppWindow *win, gchar *message)
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

static void emu8086_app_window_show_errors(Emu8086AppCodeRunner *runner, gint errors, gpointer user_data)
{
    // exit(1);
    Emu8086AppWindow *win;
    win = EMU8086_APP_WINDOW(user_data);
    PRIV;
    gchar buf[15];
    GtkWidget *icon3;

    //priv->err_prsent  = gtk_image_new_from_file(path3);
    sprintf(buf, "errors: %d", errors);

    // gtk_button_set_image(btn, priv->err_prsent);
    gtk_button_set_label(GTK_BUTTON(priv->toggle_btn2), buf);
    gtk_widget_hide(priv->toggle_btn);
    gtk_widget_show(priv->toggle_btn2);
    gtk_text_buffer_set_text(priv->err_buffer, emu8086_app_code_runner_get_errors(runner), -1);
    priv->errs_cleared = FALSE;
    //     gtk_label_set_text(GTK_LABEL(priv->messages), priv->runner->priv->em);

    //     if (priv->tos != 0)
    //     {
    //         g_source_remove(priv->tos);
    //     }
    //     priv->tos = gdk_threads_add_timeout_full(G_PRIORITY_LOW,
    //                                              1000,
    //                                              clear_message,
    //                                              win,
    //                                              NULL);
    //
}

static void reset_win(Emu8086AppCodeRunner *runner, gpointer user_data)
{
    // exit(1);
    Emu8086AppWindow *win;
    win = EMU8086_APP_WINDOW(user_data);
    PRIV;
    // gtk_label_set_text(GTK_LABEL(priv->messages), "ENDED");

    reset_code(GTK_WIDGET(priv->code));
    emu8086_window_set_memory(win, FALSE);
}

void emu8086_app_window_open(Emu8086AppWindow *win, GFile *file)
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
        emu8086_app_window_quick_message(GTK_WINDOW(win), err, "Error");
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
        emu8086_app_window_set_open(win);
        gtk_text_buffer_set_text(buf, con, len);
        // update(buf, EMU8086_APP_CODE(priv->code));

        // g_free(contents);
        win->state.isSaved = TRUE;
        win->state.file_path_set = TRUE;
        g_free(con);
    }

    else
    {
        char err[256 + 22];
        sprintf(err, "Cannot open file\n %s", win->state.file_path);
        emu8086_app_window_quick_message(GTK_WINDOW(win), err, "Error");
    }
    // g_free(base);
}
void emu8086_app_window_set_open(Emu8086AppWindow *win)
{

    win->priv->open = 1;
}

static void unset_open(Emu8086AppWindow *win)
{

    win->priv->open = 0;
}

void emu8086_app_window_stop_win(Emu8086AppWindow *win)
{
    stop(win->priv->runner, FALSE);
}

gboolean emu8086_app_window_open_egs(Emu8086AppWindow *win)
{
    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;
    GtkWindow *window = GTK_WINDOW(win);
    gboolean found;
    stop(win->priv->runner, FALSE);

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
                emu8086_app_window_quick_message(GTK_WINDOW(win), err, "Error");
                g_free(base);
                free(filename);
                gtk_widget_destroy(dialog);
                return FALSE;
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

                refreshLines(EMU8086_APP_CODE_BUFFER(buffer));
                uri = gtk_file_chooser_get_uri(chooser);
                add_recent(uri);
                g_free(uri);
                emu8086_app_window_set_open(win);

                emu8086_app_window_set_open(win);
                set_fname(priv->runner, win->state.file_path);
                gtk_text_buffer_set_text(buffer, contents, length);
                // update(buffer, EMU8086_APP_CODE(priv->code));
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
    return TRUE;
}

void emu8086_app_window_set_bottom_pane(Emu8086AppWindow *win, gboolean open)
{
    win->priv->bottom_notebook_visible = open;
    g_object_notify(G_OBJECT(win), "bottom-pane");
    gtk_widget_set_visible(win->priv->bottom_notebook,
                           win->priv->bottom_notebook_visible);
}

/**
 * emu8086_app_window_get_stack:
 * @win: a #Emu8086AppWindow
 *
 * Gets the stack of the @win.
 *
 * Returns: (transfer none): the stack #GtkWidget.
 */
GtkWidget *emu8086_app_window_get_stack(Emu8086AppWindow *win)
{
    return win->priv->left_pane;
}

/**
 * emu8086_app_window_get_revealer:
 * @win: a #Emu8086AppWindow
 *
 * Gets the revealer of the @win.
 *
 * Returns: (transfer none): the bottom #GtkWidget.
 */
GtkWidget *emu8086_app_window_get_revealer(Emu8086AppWindow *win)
{
    PRIV;
    return emu8086_app_side_pane_get_box(EMU8086_APP_SIDE_PANE(priv->revealer));
}

/**
 * emu8086_app_window_get_bottom_pane:
 * @win: a #Emu8086AppWindow
 *
 * Gets the bottom notebook of the @win.
 *
 * Returns: (transfer none): the bottom #GtkWidget.
 */
GtkWidget *emu8086_app_window_get_bottom_pane(Emu8086AppWindow *win)
{
    PRIV;
    return priv->bottom_notebook;
}

/**
 * emu8086_app_window_get_left_box:
 * @win: a #Emu8086AppWindow
 *
 * Gets the bottom left box  of the @win.
 *
 * Returns: (transfer none): the bottom #GtkWidget.
 */
GtkWidget *emu8086_app_window_get_left_box(Emu8086AppWindow *win)
{
    PRIV;
    return priv->left_box;
}

/**
 * emu8086_app_window_get_bottom_bar:
 * @win: a #Emu8086AppWindow
 *
 * Gets the bottom bar  of the @win.
 *
 * Returns: (transfer none): the bottom #GtkWidget.
 */
GtkWidget *emu8086_app_window_get_bottom_bar(Emu8086AppWindow *win)
{
    PRIV;
    return priv->bottom_bar;
}

/**
 * emu8086_app_window_get_code:
 * @win: a #Emu8086AppWindow
 *
 * Gets the code editor  of the @win.
 *
 * Returns: (transfer none): the bottom #Emu8086AppCode.
 */
Emu8086AppCode *emu8086_app_window_get_code(Emu8086AppWindow *win)
{
    return win->priv->code;
}