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
#include <code_gutter.h>

struct _Emu8086AppCode
{
    GtkTextView parent;
    gchar *font;
    gchar *theme;
};

typedef enum
{
    PROP_0,
    PROP_FONT,
    PROP_THEME
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
    GtkWidget *scrolled;
    gint mh;
    gint sh;
    GtkAdjustment *vadjustment;
    GtkTextMark *mark;
    Emu8086AppCodeGutter *gutter;
};

G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppCode, emu_8086_app_code, GTK_TYPE_TEXT_VIEW);

static void changeTheme(Emu8086AppCode *code)
{
    gint a;
    a = g_strcmp0("dark+", code->theme);
    g_print("lion: %s %d \n", code->theme, a);
}

static void
emu_8086_app_code_set_property(GObject *object,
                               guint property_id,
                               const GValue *value,
                               GParamSpec *pspec)
{
    Emu8086AppCode *code = EMU_8086_APP_CODE(object);
    PRIV_CODE;

    PangoFontDescription *desc;

    gchar *m;

    gchar *v;
    switch ((Emu8086AppCodeProperty)property_id)
    {
    case PROP_FONT:
        v = g_value_get_string(value);
        desc = pango_font_description_from_string(v);
        m = emu8086_pango_font_description_to_css(desc);
        gtk_css_provider_load_from_data(priv->provider, m, -1, NULL);

        code->font = g_strdup(v);

        pango_font_description_free(desc);
        // g_print("filename: %s\n", self->font);
        // g_free(v);
        break;

    case PROP_THEME:
        v = g_value_get_string(value);
        code->theme = g_strdup(v);
        // changeTheme(code);
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
    // g_print("lion\n");
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

static void
emu_8086_app_code_drag_data_received(GtkWidget *widget,
                                     GdkDragContext *context,
                                     gint x,
                                     gint y,
                                     GtkSelectionData *selection_data,
                                     guint info,
                                     guint timestamp)
{
    Emu8086AppCode *code;
    code = EMU_8086_APP_CODE(widget);
    PRIV_CODE;
    // ;
    open_drag_data(priv->win, selection_data);
    gtk_drag_finish(context, TRUE, FALSE, timestamp);
}
static gboolean
emu_8086_app_code_draw(GtkWidget *widget,
                       cairo_t *cr)
{
    Emu8086AppCode *code = EMU_8086_APP_CODE(widget);
    PRIV_CODE;
    gboolean event_handled;
    event_handled = GTK_WIDGET_CLASS(emu_8086_app_code_parent_class)->draw(widget, cr);

    if (priv->gutter != NULL)
    {
        draw(priv->gutter, cr);
    }
}
static void emu_8086_app_code_class_init(Emu8086AppCodeClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    object_class->set_property = emu_8086_app_code_set_property;
    object_class->get_property = emu_8086_app_code_get_property;

    widget_class->draw = emu_8086_app_code_draw;
    widget_class->drag_data_received = emu_8086_app_code_drag_data_received;
    g_object_class_install_property(object_class, PROP_FONT,
                                    g_param_spec_string("font", "Font", "Editor Font", "Monospace Regular 16",
                                                        G_PARAM_READWRITE));
    g_object_class_install_property(object_class, PROP_THEME,
                                    g_param_spec_string("theme", "Theme", "Editor Theme", "dark+",
                                                        G_PARAM_READWRITE));
}

static void emu_8086_app_code_init(Emu8086AppCode *code)
{
    PRIV_CODE;
    priv->settings = g_settings_new("com.krc.emu8086app");
    priv->provider = GTK_STYLE_PROVIDER(gtk_css_provider_new());
    priv->mark = NULL;
    priv->gutter = NULL;
    gtk_text_view_set_border_window_size(code,
                                         GTK_TEXT_WINDOW_LEFT,
                                         20);
    gtk_style_context_add_provider(gtk_widget_get_style_context(code), priv->provider, G_MAXUINT);
    g_settings_bind(priv->settings, "font", code, "font", G_SETTINGS_BIND_GET);
    g_settings_bind(priv->settings, "theme", code, "theme", G_SETTINGS_BIND_GET);
}

void select_line(GtkWidget *co, gint line)
{
    Emu8086AppCode *code = EMU_8086_APP_CODE(co);
    GtkTextMark *mark;
    gboolean ret = TRUE;
    // line = line ? line : 1;
    GtkTextIter iter, start;

    PRIV_CODE;
    Emu8086AppCodeBuffer *buffer = priv->buffer;
    // g_print("l = %d priv = %d\n", line, priv->line);
    if (priv->mark == NULL)
    {
        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &iter, line);

        start = iter;
        gtk_text_iter_forward_to_line_end(&iter);

        priv->mark = gtk_text_buffer_create_mark(GTK_TEXT_BUFFER(buffer),
                                                 "sel_line",
                                                 &iter,
                                                 FALSE);
        priv->line = line;

        gtk_text_buffer_apply_tag_by_name(buffer, "step", &start, &iter);
    }
    else if (GTK_IS_TEXT_MARK(priv->mark))
    {
        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &iter, priv->line);
        start = iter;
        gtk_text_iter_forward_to_line_end(&iter);
        // start = iter;
        // gtk_text_iter_forward_to_line_end(&iter);
        gtk_text_buffer_remove_tag_by_name(buffer, "step", &start, &iter);
        // gtk_text_buffer_get_iter_at_line(buffer, &iter, line);
        gtk_text_buffer_get_iter_at_line(GTK_TEXT_BUFFER(buffer), &iter, line);

        start = iter;
        gtk_text_iter_forward_to_line_end(&iter);

        priv->line = line;

        gtk_text_buffer_apply_tag_by_name(buffer, "step", &start, &iter);
        // gtk_text_buffer_move_mark(buffer, m, &iter);
        // start = iter;
        // gtk_text_iter_forward_to_line_end(&iter);
        // start = iter;
        // gtk_text_iter_forward_to_line_end(&iter);
        // gtk_text_buffer_apply_tag_by_name(buffer, "step", &start, &iter);
    }

    // start = iter;
    // gtk_text_iter_forward_to_line_end(&iter);
    gtk_text_buffer_place_cursor(GTK_TEXT_BUFFER(buffer), &iter);
    //     mark = gtk_text_buffer_get_mark(buffer, "insert");
    //     gtk_text_buffer_move_mark(buffer, mark, &iter);

    //     // gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(code), mark);
    //     // gtk_text_buffer_apply_tag_by_name(buffer, "step", &start, &iter);
    emu_8086_app_code_scroll_to_view(code);
    //
}

void reset_code(GtkWidget *co)
{
    Emu8086AppCode *code = EMU_8086_APP_CODE(co);

    PRIV_CODE;
    priv->mark = NULL;
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

void emu_8086_app_code_scroll_to_view(Emu8086AppCode *code)
{

    GtkTextIter mstart, mend;
    GtkTextMark *cursor;
    Emu8086AppCodeBuffer *buffer;
    PRIV_CODE;
    buffer = priv->buffer;
    cursor = gtk_text_buffer_get_insert(buffer);
    gtk_text_view_scroll_mark_onscreen(code, cursor);
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
    emu_8086_app_code_scroll_to_view(code);
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
    //(code);
    emu_8086_app_code_scroll_to_view(code);
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
    GdkRGBA _color;
    _color.alpha = 1.0;
    _color.blue = 0.20;
    _color.red = 0.20;
    _color.green = 0.20;
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
    gtk_widget_override_background_color(lines, GTK_STATE_NORMAL, &_color);
    gtk_widget_override_background_color(box2, GTK_STATE_NORMAL, &_color);

    // getCss(size, priv->provider);
    gtk_widget_set_margin_top(GTK_WIDGET(lines), 10);
    gtk_widget_set_margin_right(GTK_WIDGET(lines), 5);
    gtk_widget_set_margin_left(GTK_WIDGET(lines), 5);

    // gtk_widget_override_symbolic_color
    gtk_widget_set_margin_top(GTK_WIDGET(code), 10);

    gtk_container_add(GTK_CONTAINER(box2), lines);
    gtk_container_add(GTK_CONTAINER(box), box2);

    // gtk_container_add(GTK_CONTAINER(box), GTK_WIDGET(code));

    // GtkTextBuffer *buff = gtk_text_view_get_buffer();
    Emu8086AppCodeBuffer *buffer = emu_8086_app_code_buffer_new(NULL);
    gtk_text_view_set_buffer(GTK_TEXT_VIEW(code), GTK_TEXT_BUFFER(buffer));

    Emu8086AppCodeGutter *gutter;
    gutter = emu_8086_app_code_gutter_new(code, GTK_TEXT_WINDOW_LEFT);

    gtk_text_buffer_create_tag(buffer, "step", "background", "#B7B73B", "foreground", "#FF0000", NULL);
    gtk_text_buffer_create_tag(buffer, "keyword", "foreground", "#96CBFE", NULL);
    gtk_text_buffer_create_tag(buffer, "reg", "foreground", "#B5CAE8", "weight", PANGO_WEIGHT_BOLD, NULL);
    gtk_text_buffer_create_tag(buffer, "string", "foreground", "#CE9178", NULL);
    gtk_text_buffer_create_tag(buffer, "label_def", "foreground", "#ebeb8d", NULL);
    gtk_text_buffer_create_tag(buffer, "num", "foreground", "#B5CEA8", NULL);
    gtk_text_buffer_create_tag(buffer, "special", "foreground", "#C586C0", "weight", PANGO_WEIGHT_BOLD, NULL);
    // gtk_text_tag_    // #b5cea8

    gtk_text_view_set_editable(GTK_TEXT_VIEW(lines), FALSE);
    gtk_text_buffer_create_tag(buffer, "comment", "foreground", "#6A9955", "style", PANGO_STYLE_ITALIC, NULL);
    // gtk_text_buffer_set_text(buffer, "1 ", 1);
    // priv->code = code;
    priv->isOpen = FALSE;
    priv->lines = gtk_text_view_get_buffer(lines);
    setCode(buffer, code);
    priv->buffer = buffer;
    priv->gutter = gutter;
    priv->line = 0;
    priv->win = win;
    // priv->provider = provider;
    g_signal_connect(GTK_TEXT_BUFFER(buffer), "insert-text", G_CALLBACK(user_function), code);
    g_signal_connect(GTK_TEXT_BUFFER(buffer), "delete-range", G_CALLBACK(user_function2), code);
    //

    return EMU_8086_APP_CODE(code);

    //gtk_widget_show(lines);
}

void resized(GtkWidget *widget,
             GdkRectangle *allocation,
             gpointer user_data)
{

    Emu8086AppCode *code = EMU_8086_APP_CODE(user_data);
    PRIV_CODE;
    priv->sh = allocation->height;
    priv->mh = priv->sh - 10;
    priv->vadjustment = gtk_scrolled_window_get_vadjustment(GTK_SCROLLED_WINDOW(widget));
}
void set_win(Emu8086AppCode *code, GtkWidget *win)
{
    PRIV_CODE;
    priv->scrolled = win;
    priv->mh = 0;
    priv->sh = 0;
    priv->vadjustment = NULL;
    g_signal_connect(win, "size-allocate", G_CALLBACK(resized), code);
};

Emu8086AppCode *emu_8086_app_code_new(void)
{
    return g_object_new(EMU_8086_APP_CODE_TYPE, NULL);
}
