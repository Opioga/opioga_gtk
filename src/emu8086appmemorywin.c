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
 * emu8086win.h
 * Window class
 */
#include <emu8086appmemorywin.h>
#include <emu8086appcode.h>
#include <emu8086apprunner.h>
#include <emu8086stylescheme.h>

typedef struct _Emu8086AppMemoryWindowPrivate Emu8086AppMemoryWindowPrivate;

struct _Emu8086AppMemoryWindowPrivate
{
    Emu8086AppCode *text_view;
    Emu8086AppCodeRunner *runner;
    Emu8086AppStyleScheme *scheme;
    gint page;
    GtkTextBuffer *buffer;
    GtkWidget *scrolled;
    GtkWidget *box;
    GtkWidget *combo_box;
    gboolean is_visible;
    GtkAdjustment *adjustment
};

struct _Emu8086AppMemoryWindow
{
    GtkWindow parent;
    Emu8086AppMemoryWindowPrivate *priv;
};
static GtkWidget *emu8086_app_memory_window_create_combo_box(GtkWidget *header_bar);
static void emu8086_app_memory_window_mem_changed(GtkWidget *w, Emu8086AppMemoryWindow *win);
static void emu8086_app_memory_window_adjust_changed(GtkAdjustment *adj, Emu8086AppMemoryWindow *win);

static void emu8086_app_memory_window_populate_head(Emu8086AppMemoryWindow *win);
static void emu8086_app_memory_window_populate_body(Emu8086AppMemoryWindow *win);
static void emu8086_app_memory_win_change_theme(Emu8086AppStyleScheme *scheme, Emu8086AppMemoryWindow *win);
static void emu8086_app_window_memory_update(Emu8086AppCodeRunner *runner, gpointer data);

G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppMemoryWindow, emu8086_app_memory_window, GTK_TYPE_WINDOW);

static void emu8086_app_memory_window_init(Emu8086AppMemoryWindow *win)
{
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

    win->priv = emu8086_app_memory_window_get_instance_private(win);

    win->priv->scheme = emu8086_app_style_scheme_get_default();

    win->priv->adjustment = gtk_adjustment_new(0, 0, 0xfffff - 100, 100, 200, 2);

    emu8086_app_memory_window_populate_head(win);
    emu8086_app_memory_window_populate_body(win);
    g_signal_connect(win->priv->adjustment, "value-changed", G_CALLBACK(emu8086_app_memory_window_adjust_changed), win);
    gtk_window_set_title(GTK_WINDOW(win), "Memory");
    g_signal_connect(win->priv->scheme, "theme_changed", G_CALLBACK(emu8086_app_memory_win_change_theme), win);
    gtk_window_set_default_size(GTK_WINDOW(win), 600, 450);
    //  gtk_widget_show_all(win);
    gtk_widget_show_all(win->priv->box);
    emu8086_app_memory_win_change_theme(NULL, win);
    win->priv->is_visible = FALSE;
    win->priv->page = 1;
}

static void emu8086_app_memory_window_populate_head(Emu8086AppMemoryWindow *win)
{
    PRIV_AMW;
    GtkWidget *btn_close, *vbox, *sb;
    GtkWidget *header_bar;

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    header_bar = gtk_header_bar_new();
    sb = gtk_spin_button_new(priv->adjustment, 1, 0);

    btn_close = gtk_button_new_with_label("Close");
    priv->combo_box = emu8086_app_memory_window_create_combo_box(header_bar);
    gtk_header_bar_pack_start(GTK_HEADER_BAR(header_bar), sb);

    gtk_header_bar_pack_end(GTK_HEADER_BAR(header_bar), btn_close);
    gtk_widget_show_all(header_bar);

    g_signal_connect(btn_close, "clicked", G_CALLBACK(emu8086_app_memory_window_close), win);
    g_signal_connect(priv->combo_box, "changed", G_CALLBACK(emu8086_app_memory_window_mem_changed), win);
    gtk_window_set_titlebar(GTK_WINDOW(win), header_bar);
    gtk_container_add(GTK_CONTAINER(win), vbox);

    priv->box = vbox;
}

static GtkWidget *emu8086_app_memory_window_create_combo_box(GtkWidget *header_bar)
{
    GtkWidget *combo_box, *vbox, *label;
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);

    combo_box = gtk_combo_box_text_new();
    gtk_combo_box_text_prepend(GTK_COMBO_BOX_TEXT(combo_box),
                               "es",
                               "Extended Segment");
    gtk_combo_box_text_prepend(GTK_COMBO_BOX_TEXT(combo_box),
                               "ss",
                               "Stack Segment");

    gtk_combo_box_text_prepend(GTK_COMBO_BOX_TEXT(combo_box),

                               "cs",
                               "Code Segment");

    gtk_combo_box_text_prepend(GTK_COMBO_BOX_TEXT(combo_box),
                               "ds",
                               "Data Segment");
    gtk_combo_box_text_prepend(GTK_COMBO_BOX_TEXT(combo_box),
                               "ns",
                               "No Segment");
    gtk_combo_box_set_active_id(GTK_COMBO_BOX(combo_box), "ns");
    //  label = gtk_label_new("Memory View");

    gtk_container_add(GTK_CONTAINER(vbox), combo_box);
    //gtk_container_add(GTK_CONTAINER(vbox), label);
    gtk_header_bar_set_custom_title(GTK_HEADER_BAR(header_bar), vbox);
    return combo_box;
}

static void emu8086_app_memory_window_populate_body(Emu8086AppMemoryWindow *win)
{
    PRIV_AMW;
    GtkWidget *scrolled;
    GtkWidget *code, *vbox;

    scrolled = gtk_scrolled_window_new(NULL, NULL);
    code = create_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(code), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(code), FALSE);
    gtk_widget_set_hexpand(scrolled, TRUE);
    gtk_widget_set_vexpand(scrolled, TRUE);
    gtk_widget_set_hexpand(code, TRUE);
    gtk_widget_set_vexpand(code, TRUE);

    gtk_container_add(GTK_CONTAINER(scrolled), code);
    vbox = priv->box;
    gtk_widget_set_hexpand(vbox, TRUE);
    gtk_widget_set_vexpand(vbox, TRUE);

    gtk_container_add(GTK_CONTAINER(vbox), scrolled);

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(code));

    priv->text_view = code;
    priv->buffer = buffer;
    priv->scrolled = scrolled;
}

static void emu8086_app_memory_window_class_init(Emu8086AppMemoryWindowClass *klass)
{
}

static void emu8086_app_memory_win_change_theme(Emu8086AppStyleScheme *scheme, Emu8086AppMemoryWindow *win)
{
    PRIV_AMW;
    gchar *a;
    GdkRGBA color;
    g_return_if_fail(EMU_8086_IS_APP_MEMORY_WINDOW(win));

    a = emu8086_app_style_scheme_get_color_by_index(priv->scheme, 8);
    gdk_rgba_parse(&color, a);
    gtk_widget_override_background_color(priv->text_view, GTK_STATE_NORMAL, &color);

    //   gtk_widget_override_background_color(priv->err_text_view, GTK_STATE_NORMAL, &color);
}

static Emu8086AppMemoryWindow *emu8086_app_memory_window_new(GtkWindow *parent_win)
{

    //  emu8086_app_get_type()
    return g_object_new(EMU8086_APP_MEMORY_WINDOW_TYPE, "type", GTK_WINDOW_TOPLEVEL, "deletable", FALSE,
                        "transient-for", parent_win, NULL);
};

Emu8086AppMemoryWindow *emu8086_app_memory_window_open(GtkWindow *parent_win, Emu8086AppCodeRunner *runner)
{
    Emu8086AppMemoryWindow *win;
    win = emu8086_app_memory_window_new(parent_win);
    win->priv->runner = runner;
    g_signal_connect(win->priv->runner, "exec_ins", G_CALLBACK(emu8086_app_window_memory_update), win);
    gtk_text_buffer_set_text(win->priv->buffer, "; Run a program to \n; see memory ", -1);
    // gtk_widget_show(GTK_WIDGET(win));
    return win;
}

static void emu8086_app_window_memory_update(Emu8086AppCodeRunner *runner, gpointer data)
{

    Emu8086AppMemoryWindow *win;

    win = EMU_8086_APP_MEMORY_WINDOW(data);
    g_return_if_fail(EMU_8086_IS_APP_MEMORY_WINDOW(win));
    PRIV_AMW;

    //

    if (!priv->is_visible)
        return;

    struct emu8086 *aCPU;
    aCPU = getCPU(runner);
    g_return_if_fail(aCPU != NULL);
    gint start, end;

    start = (gint)gtk_adjustment_get_value(priv->adjustment);
    end = start + 100;
    end = end > 1048474 ? 1048474 : end;

    GString *s;
    s = g_string_new("; Memory\n");

    for (gint i = start; i < (end + 1); i++)
    {
        gchar buf[20];
        sprintf(buf, "0x%04x : 0x%02x\n", i, aCPU->mDataMem[i]);
        g_string_append(s, (buf));
    }
    gchar *str;
    str = g_string_free(s, FALSE);
    gtk_text_buffer_set_text(priv->buffer, str, -1);
    g_free(str);
}

static void emu8086_app_memory_window_mem_changed(GtkWidget *w, Emu8086AppMemoryWindow *win)
{
    struct emu8086 *aCPU;
    aCPU = getCPU(win->priv->runner);
    g_return_if_fail(aCPU != NULL);
    gint active = gtk_combo_box_get_active(GTK_COMBO_BOX(win->priv->combo_box));

    gint regs_[] = {0, REG_DS, REG_CS,
                    REG_SS,
                    REG_ES};
    gdouble value = aCPU->mSFR[regs_[active]] * 16;
    gtk_adjustment_set_value(win->priv->adjustment, value);

    emu8086_app_window_memory_update(win->priv->runner, win);
}

void emu8086_app_memory_window_close(GtkButton *btn, Emu8086AppMemoryWindow *win)
{
    g_return_if_fail(EMU_8086_IS_APP_MEMORY_WINDOW(win));
    win->priv->is_visible = !win->priv->is_visible;
    gtk_widget_set_visible(GTK_WIDGET(win), win->priv->is_visible);
}

static void emu8086_app_memory_window_adjust_changed(GtkAdjustment *adj, Emu8086AppMemoryWindow *win)
{
    emu8086_app_window_memory_update(win->priv->runner, win);
}