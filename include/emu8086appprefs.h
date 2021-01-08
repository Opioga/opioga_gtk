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
 * emu8086appprefs.h
 * Runner class
 */




#ifndef __EMU8086APPPREFS_H
#define __EMU8086APPPREFS_H

#include <gtk/gtk.h>
#include "emu8086win.h"

#define EMU8086_APP_PREFS_TYPE (emu8086_app_prefs_get_type())
G_DECLARE_FINAL_TYPE(Emu8086AppPrefs, emu8086_app_prefs, EMU8086, APP_PREFS, GtkDialog)

Emu8086AppPrefs *emu8086_app_prefs_new(Emu8086AppWindow *win);

#endif /* __EMU8086APPPREFS_H */
