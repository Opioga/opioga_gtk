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
#include <code.h>
#include <code_buffer.h>
#include <pango_css.h>

struct _Emu8086AppCode
{
    GtkTextView parent;
    gchar *font;
};

typedef enum
{
    PROP_0,
    PROP_FONT
} Emu8086AppCodeProperty;

typedef struct _Emu8086AppCodePrivate Emu8086AppCodePrivate;

struct _Emu8086AppCodePrivate
{

    GtkTextBuffer *lines;
    GtkWidget *code;
    Emu8086AppWindow *win;
    gboolean isOpen;
    GtkStyleProvider *provider;
    Emu8086AppCodeBuffer *buffer;
    gint line;
    GSettings *settings;
    gint fontsize;
};

G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppCode, emu_8086_app_code, GTK_TYPE_TEXT_VIEW);

static void
emu_8086_app_code_set_property(GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec)
{
    Emu8086AppCode *code = EMU_8086_APP_CODE(object);
    PRIV_CODE;
    // g_print("l %d\n", *value);
    PangoFontDescription *desc;

    gchar *m;
    pango_font_description_free(desc);
    gchar *v;
    switch ((Emu8086AppCodeProperty)property_id)
    {
    case PROP_FONT:
        v = g_value_get_string(value);
        desc = pango_font_description_from_string(v);
        m = emu8086_pango_font_description_to_css(desc);
        gtk_css_provider_load_from_data(priv->provider, m, -1, NULL);

        code->font = g_strdup(v);
        // g_print("filename: %s\n", self->font);
        // g_free(v);
        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void
emu_8086_app_code_get_property(GObject *object,
                               guint property_id,
                               GValue *value,
                               GParamSpec *pspec)
{
    Emu8086AppCode *self = EMU_8086_APP_CODE(object);

    switch ((Emu8086AppCodeProperty)property_id)
    {
    case PROP_FONT:
        g_value_set_string(value, self->font);
        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void emu_8086_app_code_class_init(Emu8086AppCodeClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->set_property = emu_8086_app_code_set_property;
    object_class->get_property = emu_8086_app_code_get_property;

    g_object_class_install_property(object_class, PROP_FONT,
                                    g_param_spec_string("font", "Font", "Editor Font", "Monospace Regular 16",
                                                        G_PARAM_READWRITE));
}

static void emu_8086_app_code_init(Emu8086AppCode *code)
{
    PRIV_CODE;
    priv->settings = g_settings_new("com.krc.emu8086app");
    priv->provider = GTK_STYLE_PROVIDER(gtk_css_provider_new());
    gtk_style_context_add_provider(gtk_widget_get_style_context(code), priv->provider, G_MAXUINT);
    g_settings_bind(priv->settings, "font", code, "font", G_SETTINGS_BIND_GET);
}

void select_line(GtkWidget *co, gint line)
{
    Emu8086AppCode *code = EMU_8086_APP_CODE(co);
    GtkTextMark *mark;
    gboolean ret = TRUE;
    // line = line ? line : 1;
    GtkTextIter iter, start;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(code));
    gint lc = gtk_text_buffer_get_line_count(GTK_TEXT_BUFFER(buffer));
    PRIV_CODE;

    if (line == priv->line && line > 1)
        return;
    if (priv->line)
    {
        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer),
                                         &iter,
                                         priv->line);
        start = iter;
        gtk_text_iter_forward_to_line_end(&iter);

        gtk_text_buffer_remove_tag_by_name(buffer, "step", &start, &iter);
    }
    else
    {
        // g_print("lin: %d\n", priv->line);
        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer),
                                         &iter,
                                         0);
        start = iter;
        gtk_text_iter_forward_to_line_end(&iter);
        gtk_text_buffer_remove_tag_by_name(buffer, "step", &start, &iter);
    }

    if (line >= lc)
    {
        ret = FALSE;
        priv->line = lc;
        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer),
                                         &iter,
                                         lc);
    }
    else
    {
        priv->line = line;

        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer),
                                         &iter,
                                         line);
    }
    start = iter;
    // gtk_text_iter_forward_to_line_end(&iter);
    gtk_text_iter_forward_to_line_end(&iter);
    gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer), &iter);
    mark = gtk_text_buffer_get_mark(buffer, "insert");
    gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(code), mark);
    gtk_text_buffer_apply_tag_by_name(buffer, "step", &start, &iter);
}

void reset_code(GtkWidget *co)
{
    Emu8086AppCode *code = EMU_8086_APP_CODE(co);

    PRIV_CODE;
    if (priv->line)
    {
        GtkTextIter iter, start;
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(code));
        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer),
                                         &iter,
                                         priv->line);
        gtk_text_iter_forward_to_line_end(&iter);
        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer),
                                         &start,
                                         priv->line);
        gtk_text_buffer_remove_tag_by_name(buffer, "step", &start, &iter);
    }
}

void update(Emu8086AppCode *code)
{
    PRIV_CODE;

    GtkTextBuffer *textbuffer;
    textbuffer = priv->lines;
    Emu8086AppCodeBuffer *buffer;

    GString *s;
    buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(code));
    gint lc, i;
    i = 0;
    lc = gtk_text_buffer_get_line_count(buffer);
    // if (lc == gtk_text_buffer_get_line_count(textbuffer))
    //     return;
    s = g_string_new("");

    while (i < lc)
    {
        gchar buf[10];
        sprintf(buf, "%d\n", i + 1);

        g_string_append(s, buf);
        i++;
    }

    gtk_text_buffer_set_text(textbuffer, g_string_free(s, FALSE), -1);
}

void user_function(GtkTextBuffer *textbuffer,
                   GtkTextIter *location,
                   gchar *text,
                   gint len,
                   gpointer user_data)
{

    Emu8086AppCode *code = EMU_8086_APP_CODE(user_data);
    PRIV_CODE;

    if (!priv->isOpen)
        upd(priv->win);
    else
        priv->isOpen = FALSE;
}

void user_function2(GtkTextBuffer *textbuffer,
                    GtkTextIter *start,
                    GtkTextIter *end,
                    gpointer user_data)
{

    Emu8086AppCode *code = EMU_8086_APP_CODE(user_data);
    PRIV_CODE;

    if (!priv->isOpen)
        upd(priv->win);
    else
        priv->isOpen = FALSE;
    // update(priv->lines, code);
}

void editFontSize(Emu8086AppCode *code, gint size)
{
    PRIV_CODE;
    gchar *font = code->font, *tempsize;
    gint nsize;
    // font += strlen(font);
    tempsize = g_strdup(font);

    g_strreverse(tempsize);
    g_strcanon(tempsize, "1234567890", '\0');
    g_strreverse(tempsize);

    gchar tempfont[strlen(font)];
    strcpy(tempfont, font);
    tempfont[strlen(font) - strlen(tempsize)] = 0;
    sscanf(tempsize, "%d", &priv->fontsize);
    gchar new[strlen(tempsize) + 1];
    priv->fontsize += size;
    sprintf(new, "%d", priv->fontsize);

    gchar *tmp = strcat(tempfont, new);
    code->font = g_strdup(tmp);
    g_settings_set_string(priv->settings, "font", tmp);
    PangoFontDescription *desc;
    desc = pango_font_description_from_string(tmp);
    gtk_css_provider_load_from_data(priv->provider, (emu8086_pango_font_description_to_css(desc)), -1, NULL);
    // getCss(size, priv->provider);

    pango_font_description_free(desc);
    // g_free(tmp);
    return;
}

Emu8086AppCode *create_new(GtkWidget *box, GtkWidget *box2, Emu8086AppWindow *win)
{
    GtkWidget *lines;
    Emu8086AppCode *code;
    GtkStyleProvider *provider;
    lines = gtk_text_view_new();
    gtk_widget_show(lines);
    code = emu_8086_app_code_new();
    GdkRGBA cursor_color;
    cursor_color.alpha = 1.0;
    cursor_color.blue = 0.5;
    cursor_color.red = 0.5;
    cursor_color.green = 1.0;
    PRIV_CODE;
    provider = priv->provider;
    //
    gtk_style_context_add_provider(gtk_widget_get_style_context(lines), provider, G_MAXUINT);
    GdkRGBA color;
    // lines = gtk_label_new("1");
    color.red = 0.22;
    color.green = 0.22;
    color.blue = 0.22;
    color.alpha = 1;
    gtk_widget_override_background_color(box, GTK_STATE_NORMAL, &color);
    gtk_widget_override_background_color(code, GTK_STATE_NORMAL, &color);
    gtk_widget_override_background_color(lines, GTK_STATE_NORMAL, &color); // getCss(size, priv->provider);
                                                                           // gtk_widget_override_symbolic_color
    gtk_widget_set_margin_top(GTK_WIDGET(code), 10);
    gtk_widget_override_cursor(GTK_WIDGET(code), &cursor_color, &cursor_color);
    gtk_widget_override_cursor(GTK_WIDGET(box), &cursor_color, &cursor_color);
    gtk_widget_override_cursor(GTK_WIDGET(box), &cursor_color, &cursor_color);

    gtk_container_add(GTK_CONTAINER(box2), lines);
    gtk_container_add(GTK_CONTAINER(box), box2);

    gtk_container_add(GTK_CONTAINER(box), GTK_WIDGET(code));

    // GtkTextBuffer *buff = gtk_text_view_get_buffer();
    Emu8086AppCodeBuffer *buffer = emu_8086_app_code_buffer_new(NULL);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(code), GTK_TEXT_BUFFER(buffer));

    gtk_text_buffer_create_tag(buffer, "step", "background", "#B7B73B", "foreground", "#FF0000", NULL);
    gtk_text_buffer_create_tag(buffer, "keyword", "foreground", "#96CBFE", NULL);
    gtk_text_buffer_create_tag(buffer, "reg", "foreground", "#B5CAE8", "weight", PANGO_WEIGHT_BOLD, NULL);
    gtk_text_buffer_create_tag(buffer, "string", "foreground", "#CE9178", NULL);
    gtk_text_buffer_create_tag(buffer, "label_def", "foreground", "#DCDCAA", NULL);
    gtk_text_buffer_create_tag(buffer, "num", "foreground", "#B5CEA8", NULL);
    gtk_text_buffer_create_tag(buffer, "special", "foreground", "#C586C0", "weight", PANGO_WEIGHT_BOLD, NULL);
    // #c586c0
    // #b5cea8

    gtk_text_view_set_editable(GTK_TEXT_VIEW(lines), FALSE);
    gtk_text_buffer_create_tag(buffer, "comment", "foreground", "#6A9955", "style", PANGO_STYLE_ITALIC, NULL);
    // gtk_text_buffer_set_text(buffer, "1 ", 1);
    // priv->code = code;
    priv->isOpen = FALSE;
    priv->lines = gtk_text_view_get_buffer(lines);
    setCode(buffer, code);
    priv->buffer = buffer;
    priv->line = 0;
    priv->win = win;
    // priv->provider = provider;
    g_signal_connect(GTK_TEXT_BUFFER(buffer), "insert-text", G_CALLBACK(user_function), code);
    g_signal_connect(GTK_TEXT_BUFFER(buffer), "delete-range", G_CALLBACK(user_function2), code);
    //

    return EMU_8086_APP_CODE(code);

    //gtk_widget_show(lines);
}

Emu8086AppCode *emu_8086_app_code_new(void)
{
    return g_object_new(EMU_8086_APP_CODE_TYPE, NULL);
}
