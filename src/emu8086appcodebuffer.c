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
 * code_buffer.c
 * Code Buffer class
 */
#include <emu8086appcode.h>
#include <emu8086appcodebuffer.h>
#include <emu8086apputils.h>
#include <string.h>
#include <emu8086stylescheme.h>
#include <emu8086appuredomanager.h>

typedef enum
{
    PROP_0,
    PROP_BUFFER_THEME,
    PROP_CAN_UNDO,
    PROP_CAN_REDO,
    N_PROPERTIES
} Emu8086AppCodeBufferProperty;
enum
{

    UNDO,
    REDO,

    N_SIGNALS
};

static guint buffer_signals[N_SIGNALS] = {0};
static void skip_space(GtkTextIter *iter)
{
    while (g_ascii_isspace(gtk_text_iter_get_char(iter)))
    {
        gtk_text_iter_forward_char(iter);
    }
}

static gboolean ends_line(GtkTextIter *iter)
{
    return gtk_text_iter_ends_line(iter);
}

static void _highlight(GtkTextBuffer *buffer, gint i)
{
    GtkTextIter iter, start, end, iter2;
    // GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(code));
    gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer),
                                     &iter,
                                     i);
    start = iter;
    g_return_if_fail(gtk_text_iter_get_buffer(&start) == buffer);
    g_return_if_fail(gtk_text_iter_get_buffer(&iter) == buffer);
    gtk_text_iter_forward_to_line_end(&iter);
    g_return_if_fail(gtk_text_iter_get_buffer(&start) == buffer);
    g_return_if_fail(gtk_text_iter_get_buffer(&iter) == buffer);
    gchar *line = gtk_text_buffer_get_text(buffer, &start, &iter, FALSE), *p;
    gtk_text_buffer_remove_tag_by_name(buffer, "string", &start, &iter);

    gint v = 0, t = 0, offset = 0;
    end = iter;
    if (gtk_text_iter_get_buffer(&iter) == buffer)
    {

        iter = start;
        p = line;
        iter = start;
        while (*p)
        {
            v = 0;
            gchar *p2, buf[256];
            int sep = 0, o = 0;
            p2 = buf;
            iter = start;
            iter2 = start;
            while (*p && isspace(*p) || *p == ',' || *p == '/' ||
                   *p == '*' || *p == '(' || *p == ')' || *p == '[' || *p == ']' || *p == '+' ||
                   *p == '-')
            {
                p++;
                v++;
                offset++;
            }
            if (!*p)
                return;
            sep = v;
            // offset = v;
            if (*p == ';')
            {

                gtk_text_iter_forward_chars(&iter, offset);
                gtk_text_iter_forward_to_line_end(&iter2);
                gtk_text_buffer_apply_tag_by_name(buffer, "comment", &iter, &iter2);

                break;
            }
            else
                gtk_text_buffer_remove_tag_by_name(buffer, "comment", &start, &end);

            int len = 0;
            gboolean isString = FALSE;
            gboolean isString2 = FALSE;

            isString = (*p == '"');
            isString2 = (*p == '\'');

            if (isString || isString2)
            {

                v++;
                *p2++ = *p++;
            }
            while (*p && *p != ';')
            {
                //
                //if ()
                if ((!isString && !isString2) && (isspace(*p) || *p == '+' || *p == '/' ||
                                                  *p == '*' || *p == '-' || *p == '"' || *p == ',' ||
                                                  *p == '(' || *p == ')' || *p == '[' || *p == ']' || *p == '\''))
                {
                    break;
                }
                else if (isString && *p == '"')
                {

                    v++;
                    *p2++ = *p++;
                    len += 2;
                    break;
                }
                else if (isString2 && *p == '\'')
                {

                    v++;
                    *p2++ = *p++;
                    len += 2;
                    break;
                }

                v++;
                *p2++ = *p++;
                len++;
            }
            // gtk_text_buffer_remove_tag_by_name(buffer, "string", &start, &end);
            o = v;
            *p2 = '\0';
            p2 = buf;
            // if (t == 1)
            //     g_print(gtk_text_buffer_get_text(buffer, &start, &iter, FALSE));
            // gtk_text_iter_forward_chars(&)
            gtk_text_iter_forward_chars(&iter, offset);
            iter2 = iter;
            gtk_text_iter_forward_chars(&iter2, len);
            offset += len;
            if (isString)
            {
                if (p2[len - 1] = '"')
                    gtk_text_buffer_apply_tag_by_name(buffer, "string", &iter, &iter2);
                else
                {

                    gtk_text_buffer_remove_tag_by_name(buffer, "num", &iter, &iter2);

                    gtk_text_buffer_remove_tag_by_name(buffer, "special", &iter, &iter2);

                    gtk_text_buffer_remove_tag_by_name(buffer, "string", &iter, &iter2);

                    gtk_text_buffer_remove_tag_by_name(buffer, "label_def", &iter, &end);

                    gtk_text_buffer_remove_tag_by_name(buffer, "reg", &iter, &end);

                    gtk_text_buffer_remove_tag_by_name(buffer, "keyword", &iter, &end);
                    gtk_text_buffer_apply_tag_by_name(buffer, "string", &iter, &end);
                    break;
                }
            }
            else
                gtk_text_buffer_remove_tag_by_name(buffer, "string", &iter, &iter2);
            gint length = strlen(buf);
            if (length > 0 && !isString)
            {
                // g_print(buf);
                if (length > 2)
                    gtk_text_buffer_remove_tag_by_name(buffer, "reg", &iter, &iter2);

                if (getnum_(buf))
                    gtk_text_buffer_apply_tag_by_name(buffer, "num", &iter, &iter2);
                else if (getsp_(buf))
                    gtk_text_buffer_apply_tag_by_name(buffer, "special", &iter, &iter2);
                else if (getlab_(buf))
                    gtk_text_buffer_apply_tag_by_name(buffer, "label_def", &iter, &iter2);
                else if (getkeyword(buf))
                    gtk_text_buffer_apply_tag_by_name(buffer, "keyword", &iter, &iter2);
                else if (getreg(buf))
                    gtk_text_buffer_apply_tag_by_name(buffer, "reg", &iter, &iter2);

                else if (getstr_(buf))

                {
                    gtk_text_buffer_apply_tag_by_name(buffer, "string", &iter, &iter2);
                }

                else if (length > 0)
                {

                    gtk_text_buffer_remove_tag_by_name(buffer, "num", &iter, &iter2);

                    gtk_text_buffer_remove_tag_by_name(buffer, "special", &iter, &iter2);

                    gtk_text_buffer_remove_tag_by_name(buffer, "string", &iter, &iter2);

                    gtk_text_buffer_remove_tag_by_name(buffer, "label_def", &iter, &iter2);

                    gtk_text_buffer_remove_tag_by_name(buffer, "reg", &iter, &iter2);

                    gtk_text_buffer_remove_tag_by_name(buffer, "keyword", &iter, &iter2);
                }
            }
            t++;
        }
        g_free(line);
    }
}

struct _Emu8086AppCodeBufferPrivate
{

    gint lc;
    gint line;
    GtkTextTagTable *table;
    GSettings *settings;
    Emu8086AppCode *code;
    gint timeout;
    Emu8086AppStyleScheme *scheme;
    GtkTextMark *tmp_insert_mark;
    GtkTextMark *tmp_selection_bound_mark;
    GList *search_contexts;
    gint max_undo_levels;
    gboolean can_undo;
    gboolean can_redo;
    Emu8086AppURdoManager *manager;
};

G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppCodeBuffer, emu8086_app_code_buffer, GTK_TYPE_TEXT_BUFFER);

static void emu8086_app_code_buffer_init(Emu8086AppCodeBuffer *buffer)
{

    GtkTextTagTable *tag;
    GtkTextBuffer *buf;

    //  gtk_text_buffer
    buffer->priv = emu8086_app_code_buffer_get_instance_private(buffer);
    PRIV_CODE_BUFFER;
   
    buf = GTK_TEXT_BUFFER(buffer);
    priv->settings = g_settings_new("com.krc.emu8086app");
    priv->scheme = emu8086_app_style_scheme_get_default();
    priv->manager = emu8086_app_urdo_manager_new(buf, 100);
    gtk_text_buffer_create_tag(buf, "step", "background", "#B7B73B", "foreground", "#FF0000", NULL);
    gtk_text_buffer_create_tag(buf, "find-text", "background", "#ffffff", "foreground", "#5e91f2", NULL);
    gtk_text_buffer_create_tag(buf, "keyword", NULL);
    gtk_text_buffer_create_tag(buf, "reg", "weight", PANGO_WEIGHT_BOLD, NULL);
    gtk_text_buffer_create_tag(buf, "string", NULL);
    gtk_text_buffer_create_tag(buf, "label_def", NULL);
    gtk_text_buffer_create_tag(buf, "num", NULL);
    gtk_text_buffer_create_tag(buf, "special", "weight", PANGO_WEIGHT_BOLD, NULL);

    gtk_text_buffer_create_tag(buf, "comment", "style", PANGO_STYLE_ITALIC, NULL);
    g_settings_bind(priv->settings, "theme", buffer, "theme", G_SETTINGS_BIND_GET);
    priv->lc = 0;
    priv->line = 0;
    priv->timeout = 0;
}

static void highlight(Emu8086AppCodeBuffer *buffer, gint line)
{
    GtkTextBuffer *buf;
    buf = GTK_TEXT_BUFFER(buffer);
    PRIV_CODE_BUFFER;
    gint i = 0, t = 0;
    priv->lc = gtk_text_buffer_get_line_count(buf);

    while (i < (line + 1))
    {
        t = 0;
        //
        _highlight(buf, i);
        i++;
        // g_free(line);
    }
}

gboolean hl(gpointer user_data)
{
    Emu8086AppCodeBuffer *buffer;
    GtkTextBuffer *buf;
    buf = GTK_TEXT_BUFFER(user_data);
    buffer = EMU8086_APP_CODE_BUFFER(buf);
    GtkTextMark *mark;
    GtkTextIter iter2;
    PRIV_CODE_BUFFER;

    mark = gtk_text_buffer_get_mark(buf, "insert");
    gtk_text_buffer_get_iter_at_mark(buf, &iter2, mark);
    gint i = gtk_text_iter_get_line(&iter2);
    _highlight(buf, i);
    priv->timeout = 0;
    return G_SOURCE_REMOVE;
}

void queue_highlight(Emu8086AppCodeBuffer *buffer)
{
    GtkTextBuffer *buf;
    buf = GTK_TEXT_BUFFER(buffer);
    PRIV_CODE_BUFFER;
    priv->lc = gtk_text_buffer_get_line_count(buf);
    if (priv->timeout != 0)
    {
        g_source_remove(priv->timeout);
        
    }
    hl((gpointer)(buffer));

}

static void emu8086_app_code_buffer_insert_text_real(GtkTextBuffer *buf,
                                                     GtkTextIter *iter,
                                                     const gchar *text,
                                                     gint len)
{
    gint start_offset;
    // return;
    Emu8086AppCodeBuffer *buffer;
    buffer = EMU8086_APP_CODE_BUFFER(buf);
    g_return_if_fail(EMU8086_IS_APP_CODE_BUFFER(buffer));

    g_return_if_fail(iter != NULL);
    g_return_if_fail(text != NULL);
    g_return_if_fail(gtk_text_iter_get_buffer(iter) == buf);

    GTK_TEXT_BUFFER_CLASS(emu8086_app_code_buffer_parent_class)->insert_text(buf, iter, text, len);
    PRIV_CODE_BUFFER;
    GtkTextMark *mark;
    GtkTextIter iter2;
    mark = gtk_text_buffer_get_mark(buf, "insert");
    gtk_text_buffer_get_iter_at_mark(buf, &iter2, mark);
    gint i = gtk_text_iter_get_line(&iter2);
    start_offset = i - (priv->lc);

    if (start_offset > 0 || start_offset < -1)
    {
        // g_print("lion\n");
        highlight(buffer, i);
    }

    else
    {
        queue_highlight(buffer);
    }
}

static void emu8086_app_code_buffer_delete_range(GtkTextBuffer *buffer,
                                                 GtkTextIter *start,
                                                 GtkTextIter *end)
{
    Emu8086AppCodeBuffer *buf;
    Emu8086AppCodeBufferPrivate *priv;
    gint start_offset;
    buf = EMU8086_APP_CODE_BUFFER(buffer);
    g_return_if_fail(EMU8086_IS_APP_CODE_BUFFER(buf));
    g_return_if_fail(start != NULL);
    g_return_if_fail(end != NULL);
    g_return_if_fail(gtk_text_iter_get_buffer(start) == buffer);
    g_return_if_fail(gtk_text_iter_get_buffer(end) == buffer);
    priv = buf->priv;
    GtkTextMark *mark;
    GtkTextIter iter2;
    GTK_TEXT_BUFFER_CLASS(emu8086_app_code_buffer_parent_class)->delete_range(buffer, start, end);
    mark = gtk_text_buffer_get_mark(buffer, "insert");
    gtk_text_buffer_get_iter_at_mark(buffer, &iter2, mark);
    gint i = gtk_text_iter_get_line(&iter2);
    queue_highlight(buf);
}

void emu8086_app_code_buffer_change_theme(Emu8086AppCodeBuffer *buffer)
{
    // TODO
    PRIV_CODE_BUFFER;
    GtkTextTag *tag;
    gchar *tag_name;
    GtkTextTagTable *tag_table = gtk_text_buffer_get_tag_table(GTK_TEXT_BUFFER(buffer));

    for (int i = 0; i < 7; i++)
    {
        tag_name = tag_names[i];
        tag = gtk_text_tag_table_lookup(tag_table, tag_name);

        if (G_IS_OBJECT(tag))
            g_object_set(G_OBJECT(tag), "foreground", emu8086_app_style_scheme_get_color_by_index(priv->scheme, i), NULL);
    }
}

static void
emu8086_app_code_buffer_set_property(GObject *object,
                                     guint property_id,
                                     const GValue *value,
                                     GParamSpec *pspec)
{
    Emu8086AppCodeBuffer *self = EMU8086_APP_CODE_BUFFER(object);

    gchar *v;
    switch ((Emu8086AppCodeBufferProperty)property_id)
    {

    case PROP_BUFFER_THEME:
       
       // self->theme = v;
        emu8086_app_code_buffer_change_theme(self);
        // g_string_free(v, FALSE);
        break;

    case PROP_CAN_UNDO:

        self->priv->can_undo = g_value_get_boolean(value);

        // g_string_free(v, FALSE);
        break;
    case PROP_CAN_REDO:

        self->priv->can_redo = g_value_get_boolean(value);

        // g_string_free(v, FALSE);
        break;
    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void
emu8086_app_code_buffer_get_property(GObject *object,
                                     guint property_id,
                                     GValue *value,
                                     GParamSpec *pspec)
{
    Emu8086AppCodeBuffer *self = EMU8086_APP_CODE_BUFFER(object);
    switch ((Emu8086AppCodeBufferProperty)property_id)
    {
    case PROP_BUFFER_THEME:
        g_value_set_string(value, self->theme);
        break;
    case PROP_CAN_UNDO:
        g_value_set_boolean(value, self->priv->can_undo);
        break;
    case PROP_CAN_REDO:
        g_value_set_boolean(value, self->priv->can_redo);
        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void emu8086_app_code_buffer_class_init(Emu8086AppCodeBufferClass *klass)
{

    GtkTextBufferClass *text_buffer_class;
    GObjectClass *object_class;
    object_class = G_OBJECT_CLASS(klass);
    text_buffer_class = GTK_TEXT_BUFFER_CLASS(klass);
    text_buffer_class->insert_text = emu8086_app_code_buffer_insert_text_real;
    text_buffer_class->delete_range = emu8086_app_code_buffer_delete_range;
    object_class->set_property = emu8086_app_code_buffer_set_property;
    object_class->get_property = emu8086_app_code_buffer_get_property;

    g_object_class_install_property(object_class, PROP_BUFFER_THEME,
                                    g_param_spec_string("theme", "Theme", "Editor Theme", "dark+",
                                                        G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_CAN_UNDO,
                                    g_param_spec_boolean("can-undo", "CanUndo", "If we can undo", FALSE,
                                                         G_PARAM_READWRITE));

    g_object_class_install_property(object_class, PROP_CAN_REDO,
                                    g_param_spec_boolean("can-redo", "CanRedo", "If we can redo", FALSE,
                                                         G_PARAM_READWRITE));

}

Emu8086AppCodeBuffer *emu8086_app_code_buffer_new(GtkTextTagTable *table)
{
    return g_object_new(EMU8086_APP_CODE_BUFFER_TYPE,
                        "tag-table", table,
                        NULL);
}

void refreshLines(Emu8086AppCodeBuffer *buffer)
{
    PRIV_CODE_BUFFER;
    priv->lc = 0;
}

void setCode(Emu8086AppCodeBuffer *buffer, Emu8086AppCode *code)
{
    PRIV_CODE_BUFFER;
    priv->code = code;
}
gboolean check_for_indent(gchar *line)
{
    if (*line == ';')
        return TRUE;
    gchar *p = line;

    while (*p)
    {
        if (*p == ':')
        {

            return FALSE;
        }
        p++;
    }
    //  g_print("%s\n", line);
    return TRUE;
}

void emu8086_app_code_buffer_indent(Emu8086AppCodeBuffer *buffer)
{
    GtkTextBuffer *_buffer = GTK_TEXT_BUFFER(buffer);
    gint end = gtk_text_buffer_get_line_count(_buffer);
    if (end < 0)
        return;
    gtk_text_buffer_begin_user_action(_buffer);

    for (gint i = 0; i < end; i++)
    {

        GtkTextIter iter;
        GtkTextIter iter2, iter3;
        guint replaced_spaces = 0;
        gboolean lin = FALSE;
        gtk_text_buffer_get_iter_at_line(_buffer, &iter, i);
        if (gtk_text_iter_ends_line(&iter))
        {
            continue;
        }
        if (gtk_text_iter_get_char(&iter) == '\t')
        {
            // g_print("lines: %d\n", i);
            continue;
        }

        while (gtk_text_iter_get_char(&iter) == ' ')
        {
            replaced_spaces++;
            gtk_text_iter_forward_char(&iter);
        }

        if (gtk_text_iter_ends_line(&iter))
        {
            continue;
        }

        if (replaced_spaces > 0)
        {
            iter2 = iter;
            gtk_text_iter_backward_chars(&iter2, replaced_spaces);
            gtk_text_buffer_delete(_buffer, &iter2, &iter);
            gtk_text_buffer_get_iter_at_line(_buffer, &iter, i);
        }

        iter3 = iter;
        gtk_text_iter_forward_to_line_end(&iter3);
        gchar *line = gtk_text_buffer_get_text(_buffer, &iter, &iter3, FALSE);
        if (check_for_indent(line))
            gtk_text_buffer_insert(_buffer, &iter, "\t", 1);

        g_free(line);
    }
    gtk_text_buffer_end_user_action(_buffer);
}

gboolean emu8086_app_code_buffer_get_can_undo(Emu8086AppCodeBuffer *buffer)
{
    return buffer->priv->can_undo;
}
gboolean emu8086_app_code_buffer_get_can_redo(Emu8086AppCodeBuffer *buffer)
{
    return buffer->priv->can_redo;
}

void emu8086_app_code_buffer_undo(Emu8086AppCodeBuffer *buffer)
{
    emu8086_app_urdo_manager_undo_impl(buffer->priv->manager);
}
void emu8086_app_code_buffer_redo(Emu8086AppCodeBuffer *buffer)
{
    emu8086_app_urdo_manager_redo_impl(buffer->priv->manager);
}
