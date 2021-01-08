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
 * emu8086win.h
 * Window class
 */



#ifndef WIN_C
#define WIN_C

#include <gtk/gtk.h>
#include "emu8086app.h"
#include <emu8086.h>
#include <assembler.h>

#define PRIV Emu8086AppWindowPrivate *priv = win->priv
#define EMU_8086_APP_WINDOW_TYPE (emu_8086_app_window_get_type())
G_DECLARE_FINAL_TYPE(Emu8086AppWindow, emu_8086_app_window, EMU_8086, APP_WINDOW, GtkApplicationWindow)

Emu8086AppWindow *emu_8086_app_window_new(Emu8086App *app);

typedef struct _Emu8086AppWindowState Emu8086AppWindowState;

struct _Emu8086AppWindowState
{
    char file_name[20];
    gboolean isSaved;
    char file_path[100];
    gboolean file_path_set;
    gboolean Open;
    gint fontSize;
};

void emu_8086_app_window_open(Emu8086AppWindow *win,
                              GFile *file);
void emu_8086_app_window_up(Emu8086AppWindow *win);
void emu_8086_app_window_set_app(Emu8086AppWindow *win, Emu8086App *app);

void setOpen(Emu8086AppWindow *win);
void arr_sum_activate_cb(Emu8086AppWindow *win);
void rev_str_activate_cb(Emu8086AppWindow *win);

void save_activate_cb(Emu8086AppWindow *win);
void save_as_activate_cb(Emu8086AppWindow *win);
void open_activate_cb(Emu8086AppWindow *win);
void stop_win(Emu8086AppWindow *win);
void open_drag_data(Emu8086AppWindow *win, GtkSelectionData *selection_data);
void upd(Emu8086AppWindow *win);
gchar *write_to_file(gchar *filename, gchar *buffer, char *buff);
gboolean save_doc(Emu8086AppWindow *win);
#endif /* __EXAMPLEAPPWIN_H */