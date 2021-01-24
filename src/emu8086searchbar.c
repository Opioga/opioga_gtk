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
 * emu8086searchbar.c
 * App class
 */


#include <emu8086searchbar.h>

enum
{
    PROP_M0,
    PROP_TEXT_VIEW,
    PROP_MBUFFER,
    PROP_CASE_SENSITIVE

}; //gtk_text_buffer_apply_tag_by_name(buffer, "num", &iter, &iter2);

struct _Emu8086AppSearchBarPrivate
{
    GtkTextBuffer *buffer;
    GtkEntryBuffer *entry_buffer;
    GtkEntryBuffer *go_to_line_buffer;
    guint current_index;
    guint count;
    GtkWidget *search_entry_box;
    GtkWidget *search_entry;
    GtkWidget *button_box;
    GtkWidget *go_to_line_entry;
    GtkWidget *button_next;
    gboolean case_sensitive;
    GtkWidget *check_button;
    GtkWidget *label;
    GtkTextView *text_view;
};

G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppSearchBar, emu_8086_app_search_bar, GTK_TYPE_GRID);
static void next_clicked_cb(GtkButton *button, Emu8086AppSearchBar *search_bar);
static void prev_clicked_cb(GtkButton *button, Emu8086AppSearchBar *search_bar);
static void text_inserted(GtkEntryBuffer *buffer,
                          guint position,
                          gchar *chars,
                          guint n_chars,
                          gpointer user_data);

static void line_text_inserted(GtkEntryBuffer *buffer,
                               guint position,
                               gchar *chars,
                               guint n_chars,
                               gpointer user_data);
static void text_deleted(GtkEntryBuffer *buffer,
                         guint position,
                         guint n_chars,
                         gpointer user_data);

static void line_text_deleted(GtkEntryBuffer *buffer,
                              guint position,
                              guint n_chars,
                              gpointer user_data);

static void emu_8086_app_search_bar_init(Emu8086AppSearchBar *search_bar)
{
    search_bar->priv = emu_8086_app_search_bar_get_instance_private(search_bar);
    PRIV_EMU_8086_APP_SEARCH_BAR;
    priv->count = 0;
    priv->case_sensitive = TRUE;
    priv->current_index = 0;
    priv->case_sensitive = FALSE;
    priv->buffer = NULL;
    priv->button_box = gtk_button_box_new(GTK_ORIENTATION_HORIZONTAL);
    priv->button_next = gtk_button_new_from_icon_name("go-next", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_label(priv->button_next, "Next");


    gtk_box_set_spacing(GTK_BOX(priv->button_box), 6);

    gtk_container_add(GTK_CONTAINER(priv->button_box), priv->button_next);
    gtk_button_set_relief(GTK_BUTTON(priv->button_next), GTK_RELIEF_NONE);

    priv->check_button = gtk_check_button_new_with_label("Case Sensitive");

    priv->label = gtk_label_new("Search:");
    priv->search_entry = gtk_entry_new();
    priv->search_entry_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_container_add(GTK_CONTAINER(priv->search_entry_box), priv->label);
    gtk_container_add(GTK_CONTAINER(priv->search_entry_box), priv->search_entry);

    priv->go_to_line_entry = gtk_entry_new();

    // GTK_INPUT_PURPOSE_NUMBER_object_bind_property(win, "search-show", priv->revealer_search, "reveal-child", G_BINDING_DEFAULT);
    gtk_grid_set_column_spacing(GTK_GRID(search_bar), 8);
    gtk_grid_attach(GTK_GRID(search_bar), priv->search_entry_box, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(search_bar), priv->button_box, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(search_bar), priv->check_button, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(search_bar), priv->go_to_line_entry, 3, 0, 1, 1);
    gtk_entry_set_input_purpose(GTK_ENTRY(priv->go_to_line_entry), GTK_INPUT_PURPOSE_NUMBER);
    gtk_widget_set_margin_start(GTK_WIDGET(search_bar), 10);
    gtk_widget_set_margin_top(GTK_WIDGET(search_bar), 10);

    gtk_widget_set_margin_bottom(GTK_WIDGET(search_bar), 5);
    g_object_bind_property(priv->check_button, "active", search_bar, "case-sensitive", G_BINDING_DEFAULT);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(priv->check_button), TRUE);
    priv->entry_buffer = gtk_entry_get_buffer(GTK_ENTRY(priv->search_entry));
    priv->go_to_line_buffer = gtk_entry_get_buffer(GTK_ENTRY(priv->go_to_line_entry));
    gtk_entry_set_placeholder_text(GTK_ENTRY(priv->go_to_line_entry), "Go to line...");
    g_signal_connect(priv->go_to_line_buffer, "inserted-text", G_CALLBACK(line_text_inserted), search_bar);
    g_signal_connect(priv->go_to_line_buffer, "deleted-text", G_CALLBACK(line_text_deleted), search_bar);

    g_signal_connect(priv->entry_buffer, "inserted-text", G_CALLBACK(text_inserted), search_bar);
    g_signal_connect(priv->entry_buffer, "deleted-text", G_CALLBACK(text_deleted), search_bar);
    g_signal_connect(priv->button_next, "clicked", G_CALLBACK(next_clicked_cb), search_bar);
}

static void emu_8086_app_search_bar_set_property(GObject *object,
                                                 guint property_id,
                                                 const GValue *value,
                                                 GParamSpec *pspec)
{
    Emu8086AppSearchBar *self = EMU_8086_APP_SEARCH_BAR(object);
    switch (property_id)
    {
    case PROP_TEXT_VIEW:
        // *v = (gboolean *)value;

        self->priv->text_view = GTK_TEXT_VIEW(g_value_get_object(value));
        // g_print("filename: %s\n", self->filename);
        break;

    case PROP_MBUFFER:
        // *v = (gboolean *)value;

        self->priv->buffer = GTK_TEXT_BUFFER(g_value_get_object(value));
        // g_print("filename: %s\n", self->filename);
        break;

    case PROP_CASE_SENSITIVE:

        self->priv->case_sensitive = g_value_get_boolean(value);
        break;
    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void emu_8086_app_search_bar_get_property(GObject *object,
                                                 guint property_id,
                                                 const GValue *value,
                                                 GParamSpec *pspec)
{
    Emu8086AppSearchBar *self = EMU_8086_APP_SEARCH_BAR(object);
    switch (property_id)
    {
    case PROP_TEXT_VIEW:
        g_value_set_object(value, self->priv->text_view);
        break;
    case PROP_MBUFFER:
        g_value_set_object(value, self->priv->buffer);
        break;

    case PROP_CASE_SENSITIVE:

        g_value_set_boolean(value, self->priv->case_sensitive);
        break;
    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void emu_8086_app_search_bar_class_init(Emu8086AppSearchBarClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->set_property = emu_8086_app_search_bar_set_property;
    object_class->get_property = emu_8086_app_search_bar_set_property;
    g_object_class_install_property(object_class,
                                    PROP_TEXT_VIEW,
                                    g_param_spec_object("text-view",
                                                        "textView",
                                                        "The Editors text view",
                                                        GTK_TYPE_TEXT_VIEW,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property(object_class,
                                    PROP_MBUFFER,
                                    g_param_spec_object("buffer",
                                                        "Buffer",
                                                        "The Editors text buffer",
                                                        GTK_TYPE_TEXT_BUFFER,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property(object_class,
                                    PROP_CASE_SENSITIVE,
                                    g_param_spec_boolean("case-sensitive",
                                                         "caseSensitive",
                                                         "whether search is case sensitive",
                                                         TRUE,
                                                         G_PARAM_READWRITE));
    // g_object_class_install_properties
}

Emu8086AppSearchBar *emu_8086_app_search_bar_create(GtkTextView *view)
{

    return g_object_new(EMU_8086_APP_SEARCH_BAR_TYPE, "text-view", view, "buffer", gtk_text_view_get_buffer(view), NULL);
}

static gboolean find_text(Emu8086AppSearchBar *search_bar, const gchar *text, GtkTextIter *iter,
                          gboolean applyTag)
{
    PRIV_EMU_8086_APP_SEARCH_BAR;
    GtkTextIter mstart, mend;
    GtkTextBuffer *buffer;
    buffer = priv->buffer;
    gboolean found;
    g_return_val_if_fail(GTK_IS_TEXT_BUFFER(buffer), FALSE);
    
    GtkTextSearchFlags flag = !priv->case_sensitive ? (GTK_TEXT_SEARCH_TEXT_ONLY |GTK_TEXT_SEARCH_CASE_INSENSITIVE) 
                            :GTK_TEXT_SEARCH_TEXT_ONLY  ;


    found = gtk_text_iter_forward_search(iter, text, flag, &mstart, &mend, NULL);
    if (found)
    {
        if (applyTag)
            gtk_text_buffer_apply_tag_by_name(buffer, "find-text", &mstart, &mend);
        gtk_text_buffer_select_range(buffer, &mstart, &mend);
        gtk_text_buffer_create_mark(buffer, "last_pos", &mend, FALSE);
    }
    return found;
}

static gboolean find_text_backwards(Emu8086AppSearchBar *search_bar,
                                    const gchar *text,
                                    GtkTextIter *iter)
{
    PRIV_EMU_8086_APP_SEARCH_BAR;
    GtkTextIter mstart, mend;
    GtkTextBuffer *buffer;
    buffer = priv->buffer;
    gboolean found;
    g_return_val_if_fail(GTK_IS_TEXT_BUFFER(buffer), FALSE);
    found = gtk_text_iter_backward_search(iter, text, 0, &mstart, &mend, NULL);
    if (found)
    {
        gtk_text_buffer_select_range(buffer, &mstart, &mend);
        gtk_text_buffer_create_mark(buffer, "last_pos", &mend, FALSE);
    }
    return found;
}

static void find_all(Emu8086AppSearchBar *search_bar, const gchar *text)
{
    PRIV_EMU_8086_APP_SEARCH_BAR;

    GtkTextBuffer *buffer;
    GtkTextMark *last_pos;
    buffer = priv->buffer;
    GtkTextIter iter;
    gboolean found;
    gboolean is_first;
    is_first = TRUE;
    g_return_if_fail(GTK_IS_TEXT_BUFFER(buffer));
    g_return_if_fail(gtk_text_buffer_get_char_count(buffer) > 0);
    gtk_text_buffer_get_start_iter(buffer, &iter);
    while (find_text(search_bar, text, &iter, TRUE))
    {
        last_pos = gtk_text_buffer_get_mark(buffer, "last_pos");
        if (is_first)
        {
            gtk_text_view_scroll_mark_onscreen(priv->text_view, last_pos);
            is_first = FALSE;
        }
        if (last_pos == NULL)
            break;
        gtk_text_buffer_get_iter_at_mark(buffer, &iter, last_pos);
        if (last_pos != NULL)
            gtk_text_buffer_delete_mark(buffer, last_pos);
        /* code */
    }
    last_pos = gtk_text_buffer_get_mark(buffer, "last_pos");
    if (last_pos != NULL)
        gtk_text_buffer_delete_mark(buffer, last_pos);
}

static void find_once(Emu8086AppSearchBar *search_bar, const gchar *text, gboolean backward)
{
    PRIV_EMU_8086_APP_SEARCH_BAR;

    GtkTextBuffer *buffer;
    GtkTextMark *last_pos;
    buffer = priv->buffer;
    GtkTextIter iter;
    gboolean found;
    g_return_if_fail(GTK_IS_TEXT_BUFFER(buffer));
    g_return_if_fail(gtk_text_buffer_get_char_count(buffer) > 0);

    last_pos = gtk_text_buffer_get_mark(buffer, "last_pos");
    if (last_pos == NULL)
    {

        gtk_text_buffer_get_start_iter(buffer, &iter);
    }
    else
        gtk_text_buffer_get_iter_at_mark(buffer, &iter, last_pos);

    if (!backward && find_text(search_bar, text, &iter, FALSE))
    {
        last_pos = gtk_text_buffer_get_mark(buffer, "last_pos");
        if (last_pos == NULL)
            return;
        gtk_text_view_scroll_mark_onscreen(priv->text_view, last_pos);
    };
    if (backward)
    {
        if (last_pos == NULL)
            return;
        gtk_text_buffer_get_iter_at_mark(buffer, &iter, last_pos);
        if (find_text_backwards(search_bar, text, &iter))
        {
            last_pos = gtk_text_buffer_get_mark(buffer, "last_pos");
            if (last_pos == NULL)
                return;
            gtk_text_view_scroll_mark_onscreen(priv->text_view, last_pos);
        };
    }

}

static void clear_selections(Emu8086AppSearchBar *search_bar)

{
    PRIV_EMU_8086_APP_SEARCH_BAR;

    GtkTextIter start;
    GtkTextIter end;
    GtkTextBuffer *buffer;
    GtkTextMark *last_pos;
    buffer = priv->buffer;

    g_return_if_fail(GTK_IS_TEXT_BUFFER(priv->buffer));
    gtk_text_buffer_get_bounds(buffer, &start, &end);
    gtk_text_buffer_remove_tag_by_name(buffer, "find-text", &start, &end);
    last_pos = gtk_text_buffer_get_mark(buffer, "last_pos");
    if (last_pos != NULL)
        gtk_text_buffer_delete_mark(buffer, last_pos);
}

static void next_clicked_cb(GtkButton *button, Emu8086AppSearchBar *search_bar)
{
    PRIV_EMU_8086_APP_SEARCH_BAR;
    gchar *text;
    text = gtk_entry_buffer_get_text(priv->entry_buffer);
    if (strlen(text) == 0)
        return;

    find_once(search_bar, text, FALSE);
}

static void text_inserted(GtkEntryBuffer *buffer,
                          guint position,
                          gchar *chars,
                          guint n_chars,
                          gpointer user_data)
{
    Emu8086AppSearchBar *search_bar = EMU_8086_APP_SEARCH_BAR(user_data);

    gchar *text;

    gboolean found;
    text = gtk_entry_buffer_get_text(buffer);

    if (strlen(text) == 0)
        return;
    clear_selections(search_bar);
    find_all(search_bar, text);
}
static void text_deleted(GtkEntryBuffer *buffer,
                         guint position,
                         guint n_chars,
                         gpointer user_data)
{
    Emu8086AppSearchBar *search_bar = EMU_8086_APP_SEARCH_BAR(user_data);

    gchar *text;

    gboolean found;
    text = gtk_entry_buffer_get_text(buffer);

    if (strlen(text) == 0)
        return;
    clear_selections(search_bar);
    find_all(search_bar, text);
}

static void go_to_line(Emu8086AppSearchBar *search_bar, gchar *text)
{
    PRIV_EMU_8086_APP_SEARCH_BAR;
    GtkTextIter iter;
    gint line = g_ascii_strtoll(text, NULL, 10);
    g_return_if_fail(GTK_IS_TEXT_BUFFER(priv->buffer));

    gint lc = gtk_text_buffer_get_line_count(priv->buffer);
    g_return_if_fail((line > 0) && (lc > 0));
    g_return_if_fail((line <= lc));
    gtk_text_buffer_get_iter_at_line(priv->buffer, &iter, line - 1);
    gtk_text_iter_forward_to_line_end(&iter);
    gtk_text_buffer_place_cursor(priv->buffer, &iter);
    gtk_text_view_scroll_mark_onscreen(priv->text_view,
                                       gtk_text_buffer_get_insert(priv->buffer));
}

static void line_text_inserted(GtkEntryBuffer *buffer,
                               guint position,
                               gchar *chars,
                               guint n_chars,
                               gpointer user_data)
{
    Emu8086AppSearchBar *search_bar = EMU_8086_APP_SEARCH_BAR(user_data);

    gchar *text;

    text = gtk_entry_buffer_get_text(buffer);

    if (strlen(text) == 0)
        return;
    go_to_line(search_bar, text);
}

static void line_text_deleted(GtkEntryBuffer *buffer,
                              guint position,
                              guint n_chars,
                              gpointer user_data)
{
    Emu8086AppSearchBar *search_bar = EMU_8086_APP_SEARCH_BAR(user_data);

    gchar *text;

    text = gtk_entry_buffer_get_text(buffer);

    if (strlen(text) == 0)
        return;
    go_to_line(search_bar, text);
}

gboolean find(Emu8086AppSearchBar *search_bar, const gchar *text)
{
    PRIV_EMU_8086_APP_SEARCH_BAR;
    GtkTextIter mstart, mend;
    GtkTextBuffer *buffer;
    buffer = priv->buffer;
    gboolean found;
    g_return_if_fail(GTK_IS_TEXT_BUFFER(priv->buffer));

    if (found)
    {
        gtk_text_buffer_select_range(buffer, &mstart, &mend);
        gtk_text_buffer_create_mark(buffer, "last_pos", &mend, FALSE);
    }
}

void emu_8086_app_search_bar_clear_out(Emu8086AppSearchBar *search_bar)
{
    PRIV_EMU_8086_APP_SEARCH_BAR;
    gtk_entry_set_text(GTK_ENTRY(priv->search_entry), "");
    gtk_entry_set_text(GTK_ENTRY(priv->go_to_line_entry), "");
    clear_selections(search_bar);
}