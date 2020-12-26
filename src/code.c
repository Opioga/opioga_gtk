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
struct _Emu8086AppCode
{
    GtkTextView parent;
};

typedef struct _Emu8086AppCodePrivate Emu8086AppCodePrivate;

struct _Emu8086AppCodePrivate
{

    GtkWidget *lines;
    GtkWidget *code;
    Emu8086AppWindow *win;
    gboolean isOpen;
    GtkStyleProvider *provider;
    Emu8086AppCodeBuffer *buffer;
    gint line;
};

G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppCode, emu_8086_app_code, GTK_TYPE_TEXT_VIEW);



static void emu_8086_app_code_class_init(Emu8086AppCodeClass *klass) {

}



static void emu_8086_app_code_init(Emu8086AppCode *code) {

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
void update(GtkTextBuffer *textbuffer, Emu8086AppCode *code){

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
}

static void *getCss(gint size, GtkStyleProvider *provider)
{
    gchar buf[85];
    if (size > 30)
        return;

    sprintf(buf, "* {font-family: Monospace;font-size: %dpx; color: #ffffff;"
                 "caret-color: #ffffff;}",
            size);
    gtk_css_provider_load_from_data(provider, buf, strlen(buf), NULL);
}

void editFontSize(Emu8086AppCode *code, gint size)
{
    PRIV_CODE;
    getCss(size, priv->provider);
}

Emu8086AppCode *create_new(GtkWidget *box, GtkWidget *box2, Emu8086AppWindow *win)
{
    GtkWidget *lines;
    Emu8086AppCode *code;
    GtkStyleProvider *provider;
    lines = gtk_label_new(" ");
    gtk_widget_show(lines);
    code = emu_8086_app_code_new();
    GdkRGBA cursor_color;
    cursor_color.alpha = 1.0;
    cursor_color.blue = 0.5;
    cursor_color.red = 0.5;
    cursor_color.green = 1.0;

    provider = GTK_STYLE_PROVIDER(gtk_css_provider_new());
    gtk_style_context_add_provider(gtk_widget_get_style_context(code), provider, G_MAXUINT);
    gtk_style_context_add_provider(gtk_widget_get_style_context(lines), provider, G_MAXUINT);
    gtk_css_provider_load_from_resource(provider, "/com/krc/emu8086app/css/text.css");
    // gtk_css_provider_load_from_data(provider, getCss(40), -1, NULL);
    gtk_widget_set_margin_top(GTK_WIDGET(code), 10);
    gtk_widget_override_cursor(GTK_WIDGET(code), &cursor_color, &cursor_color);
    gtk_widget_override_cursor(GTK_WIDGET(box), &cursor_color, &cursor_color);
    gtk_widget_override_cursor(GTK_WIDGET(box), &cursor_color, &cursor_color);

    gtk_container_add(GTK_CONTAINER(box2), lines);
    gtk_container_add(GTK_CONTAINER(box), box2);

    gtk_container_add(GTK_CONTAINER(box), GTK_WIDGET(code));
    PRIV_CODE;
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
    gtk_text_buffer_create_tag(buffer, "comment", "foreground", "#6A9955", "style", PANGO_STYLE_ITALIC, NULL);
    // gtk_text_buffer_set_text(buffer, "1 ", 1);
    // priv->code = code;
    priv->isOpen = FALSE;
    priv->lines = lines;
    priv->buffer = buffer;
    priv->line = 0;
    priv->win = win;
    priv->provider = provider;
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
