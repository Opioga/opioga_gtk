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
 * emu8086.h
 * Runner class
 */





#ifndef EMU_APP_C
#define EMU_APP_C
#include <gtk/gtk.h>

#define EMU8086_APP_TYPE (emu8086_app_get_type())
#define _PRIV Emu8086AppPrivate *priv = app->priv

G_DECLARE_FINAL_TYPE(Emu8086App, emu8086_app, EMU8086, APP, GtkApplication)
Emu8086App *emu8086_app_new(void);
void emu8086_app_open_file(Emu8086App *app, GFile *file);
Emu8086App *
emu8086_app_get_default (void);
void emu8086_app_quick_message(GtkWindow *parent, gchar *message, gchar *title);
// void quit(Emu8086AppWindow *app);




#endif