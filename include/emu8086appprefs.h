#ifndef __EMU8086APPPREFS_H
#define __EMU8086APPPREFS_H

#include <gtk/gtk.h>
#include "emu8086win.h"

#define EMU8086_APP_PREFS_TYPE (emu8086_app_prefs_get_type())
G_DECLARE_FINAL_TYPE(Emu8086AppPrefs, emu8086_app_prefs, EMU8086, APP_PREFS, GtkDialog)

Emu8086AppPrefs *emu8086_app_prefs_new(Emu8086AppWindow *win);

#endif /* __EMU8086APPPREFS_H */
