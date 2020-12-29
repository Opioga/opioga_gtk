#include <stdio.h>
#include <gtk/gtk.h>
#include <emu8086app.h>
#include <emu8086aboutwin.h>

struct _Emu8086AboutWindow
{
    GtkAboutDialog parent;
};

Emu8086AboutWindow *emu_8086_about_window_new(const gchar *app)
{
    return g_object_new(EMU_8086_ABOUT_WINDOW_TYPE, "title", app, NULL);
};
G_DEFINE_TYPE(Emu8086AboutWindow, emu_8086_about_window, GTK_TYPE_ABOUT_DIALOG);

static void data_win_close(GtkDialog *button, Emu8086AboutWindow *win)
{
    gtk_window_close(GTK_WINDOW(win));
}

void static clicked_close_button(GtkDialog *dialog,
                                 gint response_id,
                                 Emu8086AboutWindow *win)
{
    gtk_window_close(GTK_WINDOW(win));
}

static void emu_8086_about_window_class_init(Emu8086AboutWindowClass *class)
{
    gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                                                "/com/krc/emu8086app/ui/about.ui");
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), data_win_close);
    gtk_widget_class_bind_template_callback(GTK_WIDGET_CLASS(class), clicked_close_button);
}

static void emu_8086_about_window_init(Emu8086AboutWindow *win)
{
    gtk_widget_init_template(GTK_WIDGET(win));
}
