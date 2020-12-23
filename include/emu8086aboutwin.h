#ifndef ABOUT_WIN_C
#define ABOUT_WIN_C

#include <gtk/gtk.h>
#include "emu8086app.h"
#define EMU_8086_ABOUT_WINDOW_TYPE (emu_8086_about_window_get_type())

G_DECLARE_FINAL_TYPE(Emu8086AboutWindow, emu_8086_about_window, EMU_8086, ABOUT_DIALOG, GtkAboutDialog)
Emu8086AboutWindow *emu_8086_about_window_new(const gchar *app);
void emu_8086_about_window_open(Emu8086AboutWindow *appWindow);

#endif
