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

#define EMU_8086_APP_TYPE (emu_8086_app_get_type())
#define _PRIV Emu8086AppPrivate *priv = emu_8086_app_get_instance_private(app)

G_DECLARE_FINAL_TYPE(Emu8086App, emu_8086_app, EMU_8086, APP, GtkApplication)

Emu8086App *emu_8086_app_new(void);
void emu_8086_open_file(Emu8086App *app, GFile *file);

// void quit(Emu8086AppWindow *app);

typedef struct _theme them;
struct _theme
{
    gchar *id;
    gchar *text;
};

#endif