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
#include <emu8086app.h>
#include <emu8086win.h>
#include <code.h>
struct _Emu8086AppWindow
{
    GtkApplicationWindow parent;
    Emu8086AppWindowState state;
};

typedef struct _Emu8086AppWindowPrivate Emu8086AppWindowPrivate;
struct _Emu8086AppWindowPrivate
{
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
    GtkWidget *messages; //GtkWidget *spinner;
    gchar *fname;
    GtkWidget *head_bar;
    GtkWidget *window_m;
    GtkWidget *scrolled;
    Emu8086App *app;
};
G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppWindow, emu_8086_app_window, GTK_TYPE_APPLICATION_WINDOW);
Emu8086AppWindow *emu_8086_app_window_new(Emu8086App *app)
{
    //  emu_8086_app_get_type()
    return g_object_new(EMU_8086_APP_WINDOW_TYPE, "application", app, NULL);
};

static void emu_8086_app_window_init(Emu8086AppWindow *win)
{

    gtk_widget_init_template(GTK_WIDGET(win));

    PRIV;
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
    gtk_widget_set_margin_left(content_area, 20);
    gtk_widget_set_margin_right(content_area, 20);
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
static void _open(Emu8086AppWindow *win)
{
    GtkWidget *dialog;
    GtkFileChooser *chooser;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;
    gboolean ret = FALSE;
    PRIV;
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
            quick_message(win, err, "Error");
            g_free(base);
            free(filename);
            gtk_widget_destroy(dialog);
            return;
        }
        strcpy(win->state.file_name, base);
        //win->state.file_name[strlen(base) - 1] = '\0';
        gtk_window_set_title(win, win->state.file_name);

        strcpy(win->state.file_path, filename);

        //g_file_get_contents(filename, )
        if (g_file_load_contents(file, NULL, &contents, &length, NULL, NULL))
        {
            PRIV;
            GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));

            setOpen(win);
            gtk_text_buffer_set_text(buffer, contents, length);
            update(buffer, EMU_8086_APP_CODE(priv->code));

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
        else if ((event->keyval == GDK_KEY_s))
        {
            handled = TRUE;
            save_doc(EMU_8086_APP_WINDOW(widget));
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
static void open_activate_cb(GtkMenuItem *item, gpointer user_data)
{

    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(user_data);
    _open(win);
}

static void quit_activate_cb(GtkMenuItem *item, gpointer user_data)
{

    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(user_data);
    PRIV;
    // gtk_widget_destroy(GTK_WIDGET(priv->code));
    quit(priv->app);
    // gtk_application_ex
}

static void save_activate_cb(GtkMenuItem *item, gpointer user_data)
{

    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(user_data);
    save_doc(win);
    // gtk_application_ex
}
static void save_as_activate_cb(GtkMenuItem *item, gpointer user_data)
{
    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(user_data);

    if (save_new(win, "lol", win->state.file_path))
    {
        PRIV;
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
        // GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
        GtkTextIter start_iter, end_iter;
        gtk_text_buffer_get_start_iter(buffer, &start_iter);
        gtk_text_buffer_get_end_iter(buffer, &end_iter);
        gchar *con = gtk_text_buffer_get_text(buffer, &start_iter, &end_iter, FALSE);
        g_print(win->state.file_path);

        win->state.file_path_set = TRUE;
        write_to_file(win->state.file_path, con, win->state.file_name);
        gtk_window_set_title(GTK_WINDOW(win), win->state.file_name);
        win->state.isSaved = TRUE;
        return TRUE;
    };
}

static void about_activate_cb(GtkMenuItem *item, gpointer user_data)
{
    open_help();
}

static void arr_sum_activate_cb(GtkMenuItem *item, gpointer user_data)
{
    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(user_data);
    PRIV;
    gchar *con;
    gsize len;
    GFile *file = g_file_new_for_path("/usr/local/share/emu8086/egs/ArraySum.asm");
    g_return_if_fail(file != NULL);
    strcpy(win->state.file_path, "/usr/local/share/emu8086/egs/ArraySum.asm");
    strcpy(win->state.file_name, "ArraySum.asm");

    if (g_file_load_contents(file, NULL, &con, &len, NULL, NULL))
    {

        // memcpy
        GtkTextBuffer *buf;
        buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
        setOpen(win);
        gtk_text_buffer_set_text(buf, con, len);
        update(buf, EMU_8086_APP_CODE(priv->code));

        // g_free(contents);
        win->state.isSaved = TRUE;
        // win->state.file_path_set = TRUE;
        g_free(con);
    }
}

static void rev_str_activate_cb(GtkMenuItem *item, gpointer user_data)
{
    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(user_data);
    PRIV;
    gchar *con;
    gsize len;
    GFile *file = g_file_new_for_path("/usr/local/share/emu8086/egs/RevStr.asm");
    g_return_if_fail(file != NULL);
    strcpy(win->state.file_path, "/usr/local/share/emu8086/egs/RevStr.asm");
    strcpy(win->state.file_name, "RevStr.asm");

    if (g_file_load_contents(file, NULL, &con, &len, NULL, NULL))
    {

        // memcpy
        GtkTextBuffer *buf;
        buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
        setOpen(win);
        gtk_text_buffer_set_text(buf, con, len);
        update(buf, EMU_8086_APP_CODE(priv->code));

        // g_free(contents);
        win->state.isSaved = TRUE;
        // win->state.file_path_set = TRUE;
        g_free(con);
    }
}

static void emu_8086_app_window_class_init(Emu8086AppWindowClass *class)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(class);
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                                                "/com/krc/emu8086app/emu8086.ui");

    widget_class->key_press_event = emu_8086_window_key_press_event;
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), about_activate_cb);
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), open_activate_cb);

    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), save_activate_cb);
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), save_as_activate_cb);
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), quit_activate_cb);

    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), arr_sum_activate_cb);
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), rev_str_activate_cb);

    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, tool_bar);
    gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, stack);
    // gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppWindow, code);
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
};

void char_ins(GtkTextView *text_view,
              gchar *string,
              gpointer user_data)
{
    exit(1);
}

void upd(Emu8086AppWindow *win)
{

    if (win->state.isSaved && (win->state.Open == 0))
    {
        char buf[20];
        sprintf(buf, "%s*", win->state.file_name);
        gtk_window_set_title(GTK_WINDOW(win), buf);
        win->state.isSaved = FALSE;
    }
    if (win->state.Open == 1)
    {
        win->state.Open = 0;
    }
}

gchar *write_to_file(gchar *filename, gchar *buffer, char *buff)
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
        char *filename;

        filename = gtk_file_chooser_get_filename(chooser);
        //  save_to_file(filename);
        strcpy(buf, filename);
        g_free(filename);
        ret = TRUE;
    }

    gtk_widget_destroy(dialog);
    return ret;
}

gboolean save_doc(Emu8086AppWindow *win)
{
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
            write_to_file(win->state.file_path, con, win->state.file_name);
            gtk_window_set_title(GTK_WINDOW(win), win->state.file_name);
            win->state.isSaved = TRUE;
            return TRUE;
        }
        else
        {
            if (save_new(win, "lol", win->state.file_path))
            {

                win->state.file_path_set = TRUE;
                write_to_file(win->state.file_path, con, win->state.file_name);
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
    if (!save_doc(win))
    {
        return;
    }
    if (c > 0)
    {
        gtk_text_view_set_editable(GTK_TEXT_VIEW(priv->code), FALSE);
        run_clicked_app(priv->app, win->state.file_path);
    }
    else
        emu_8086_app_window_flash(win, "Nothing to run");
}

void pause_clicked(GtkToolButton *toolbutton,
                   gpointer user_data)
{
    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(user_data);
    PRIV;
    set_app_state(priv->app, STEP);
    //
}

void step_clicked(GtkToolButton *toolbutton,
                  gpointer user_data)
{
    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(user_data);

    if (!save_doc(win))
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
        step_clicked_app(priv->app, win->state.file_path);
    }

    else
        emu_8086_app_window_flash(win, "Nothing to run");
}
void step_over_clicked(GtkToolButton *toolbutton,
                       gpointer user_data)
{
    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(user_data);
    if (!save_doc(win))
    {
        return;
    }
    PRIV;
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(priv->code));
    gint c = gtk_text_buffer_get_char_count(buffer);

    if (c > 0)
    {
        gtk_text_view_set_editable(GTK_TEXT_VIEW(priv->code), FALSE);

        step_over_clicked_app(priv->app, win->state.file_path);
    }
    else
        emu_8086_app_window_flash(win, "Nothing to run");
}
void save_clicked(GtkToolButton *toolbutton,
                  gpointer user_data)
{
    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(user_data);
    save_doc(win);
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
    stop_clicked_app(priv->app);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(priv->code), TRUE);

    //step_over_clicked_app
}

void populate_tools(Emu8086AppWindow *win)
{
    PRIV;
    GtkToolItem *play, *step, *stop, *pause, *save, *open, *step_over;
    play = gtk_tool_button_new(NULL, NULL);
    // gtk_tool_button_set_label(GTK_TOOL_BUTTON(play), ("Run"));
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(play), "media-playback-start");
    gtk_tool_button_set_use_underline(GTK_TOOL_BUTTON(play), TRUE);
    gtk_tool_item_set_is_important(play, TRUE);
    gtk_widget_show(play);

    stop = gtk_tool_button_new(NULL, NULL);

    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(stop), "media-playback-stop");
    gtk_widget_show(stop);
    gtk_tool_button_set_label(stop, "Stop");

    pause = gtk_tool_button_new(NULL, NULL);
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(pause), "media-playback-pause");
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(pause), ("Halt"));
    gtk_tool_button_set_use_underline(GTK_TOOL_BUTTON(pause), TRUE);
    gtk_tool_item_set_is_important(pause, TRUE);

    gtk_widget_show(pause);

    step = gtk_tool_button_new(NULL, NULL);
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(step), "media-skip-forward");
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(step), ("Step"));
    gtk_tool_button_set_use_underline(GTK_TOOL_BUTTON(step), TRUE);
    gtk_tool_item_set_is_important(step, TRUE);
    gtk_widget_show(step);

    save = gtk_tool_button_new(NULL, NULL);
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(save), "document-save");
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(save), ("Save"));
    gtk_tool_button_set_use_underline(GTK_TOOL_BUTTON(save), TRUE);
    gtk_tool_item_set_is_important(save, TRUE);
    gtk_widget_show(save);

    open = gtk_tool_button_new(NULL, NULL);
    gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(open), "document-open");
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(open), ("Open"));
    gtk_tool_button_set_use_underline(GTK_TOOL_BUTTON(open), TRUE);
    gtk_tool_item_set_is_important(open, TRUE);
    gtk_widget_show(open);
    //

    step_over = gtk_tool_button_new(NULL, NULL);
    // gtk_tool_button_set_icon_name(GTK_TOOL_BUTTON(step_over), "media-skip-forward");
    gtk_tool_button_set_label(GTK_TOOL_BUTTON(step_over), ("Step Over"));
    gtk_tool_item_set_is_important(step_over, TRUE);
    gtk_widget_show(step_over);

    // gtk_tool_button_set_label(step, "Step");
    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, save, 0);
    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, open, -1);
    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, play, -1);
    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, pause, -1);
    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, step, -1);
    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, step_over, -1);
    gtk_toolbar_insert((GtkToolbar *)priv->tool_bar, stop, -1);

    g_signal_connect(play, "clicked", G_CALLBACK(play_clicked), win);
    g_signal_connect(pause, "clicked", G_CALLBACK(pause_clicked), win);
    g_signal_connect(step, "clicked", G_CALLBACK(step_clicked), win);
    g_signal_connect(open, "clicked", G_CALLBACK(open_clicked), win);
    g_signal_connect(stop, "clicked", G_CALLBACK(stop_clicked), win);
    g_signal_connect(save, "clicked", G_CALLBACK(save_clicked), win);
    g_signal_connect(step_over, "clicked", G_CALLBACK(step_over_clicked), win);
}

static void populate_win(Emu8086AppWindow *win)
{

    PRIV;
    gtk_window_set_default_size(GTK_WINDOW(win), 800, 600);

    GtkWidget *scrolled, *scrolled_2, *mem, *header, *b;
    GtkWidget *box, *code, *box2;
    int be = 0;
    gtk_window_set_icon_name(GTK_WINDOW(win), "emu8086");
    scrolled = gtk_scrolled_window_new(NULL, NULL);
    scrolled_2 = gtk_scrolled_window_new(NULL, NULL);
    b = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    header = gtk_label_new("MEMORY");
    gtk_container_add(GTK_CONTAINER(b), header);
    mem = gtk_label_new("00000");
    gtk_container_add(GTK_CONTAINER(b), mem);
    gtk_container_add(GTK_CONTAINER(scrolled_2), b);
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 20);
    box2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_show(scrolled_2);
    gtk_widget_show(mem);
    gtk_widget_show(header);
    gtk_widget_show(b);
    gtk_widget_show(scrolled);
    gtk_widget_set_hexpand(scrolled, TRUE);
    gtk_widget_set_vexpand(scrolled, TRUE);

    code = create_new(box, box2, win);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(code), TRUE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(code), TRUE);

    gtk_widget_set_hexpand(box, TRUE);
    gtk_widget_set_vexpand(box, TRUE);
    gtk_widget_set_hexpand(code, TRUE);
    gtk_widget_set_vexpand(code, TRUE);
    GdkRGBA color;
    // lines = gtk_label_new("1");
    color.red = 0.22;
    color.green = 0.22;
    color.blue = 0.22;
    color.alpha = 1;
    gtk_widget_override_background_color(box, GTK_STATE_NORMAL, &color);
    gtk_widget_override_background_color(code, GTK_STATE_NORMAL, &color);

    gtk_container_add(GTK_CONTAINER(scrolled), box);
    gtk_widget_show_all(scrolled);
    gtk_stack_add_titled(GTK_STACK(priv->stack), scrolled, "code", "Code");
    gtk_stack_add_titled(GTK_STACK(priv->stack), scrolled_2, "memory", "Memory");

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(code));

    priv->code = code;
    priv->scrolled = scrolled;
    priv->tos = 0;
    strcpy(win->state.file_name, "Untitled.asm");
    win->state.isSaved = TRUE;
    win->state.file_path_set = FALSE;
    gtk_window_set_title(GTK_WINDOW(win), win->state.file_name);
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

void emu_8086_app_window_update_wids(Emu8086AppWindow *win, struct emu8086 *aCPU)
{
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
        GtkScrolledWindow *s = GTK_SCROLLED_WINDOW(priv->scrolled);
        GtkAdjustment *a = gtk_scrolled_window_get_vadjustment(s);
        gdouble ps = gtk_adjustment_get_page_size(a);
        gdouble incr = gtk_adjustment_get_step_increment(a);

        gdouble v = 2.5;
        v = _INSTRUCTIONS->line_number > 300 ? 2.4 : v;
        gtk_adjustment_set_value(a, (incr * ((_INSTRUCTIONS->line_number) / v)));
        gtk_scrolled_window_set_vadjustment(s, a);
    }
    else
    {

        select_line(priv->code, 1);
    }
    // sprintf(buf, "SS: %04x", _SS);
    // gtk_label_set_text(GTK_LABEL(priv->SS_), buf);
};

void emu_8086_app_window_set_app(Emu8086AppWindow *win, Emu8086App *app)
{
    PRIV;
    priv->app = app;
}
void clear_message(gpointer user_data)
{
    Emu8086AppWindow *win = EMU_8086_APP_WINDOW(user_data);
    PRIV;
    gtk_label_set_text(GTK_LABEL(priv->messages), "  ");
    //
    priv->tos = 0;
}
void emu_8086_app_window_flash(Emu8086AppWindow *win, char *_err)
{
    PRIV;

    //char buf[30];
    // strcpy(buf, err->message);
    //  g_print("err->message\n");
    // g_print();

    if (_err)
    {
        if (priv->tos != 0)
            return;
        gtk_label_set_text(GTK_LABEL(priv->messages), _err);
        priv->tos = g_timeout_add(2500,
                                  (GSourceFunc)clear_message,
                                  win);
    }
    //  if (err->next == NULL)

    //  errors--;
    // }
}

void reset_win(Emu8086AppWindow *win)
{
    PRIV;
    // gtk_label_set_text(GTK_LABEL(priv->messages), "ENDED");

    reset_code(priv->code);
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
        quick_message(win, err, "Error");
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
        setOpen(win);
        gtk_text_buffer_set_text(buf, con, len);
        update(buf, EMU_8086_APP_CODE(priv->code));

        // g_free(contents);
        win->state.isSaved = TRUE;
        win->state.file_path_set = TRUE;
        g_free(con);
    }

    else
    {
        char err[256];
        sprintf(err, "Cannot open file\n %s", win->state.file_path);
        quick_message(win, err, "Error");
    }
    // g_free(base);
}
void setOpen(Emu8086AppWindow *win)
{

    win->state.Open = TRUE;
}
