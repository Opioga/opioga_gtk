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
#define EMU8086_APP_WINDOW_TYPE (emu8086_app_window_get_type())
G_DECLARE_FINAL_TYPE(Emu8086AppWindow, emu8086_app_window, EMU8086, APP_WINDOW, GtkApplicationWindow)

G_BEGIN_DECLS


typedef struct _Emu8086AppWindowState Emu8086AppWindowState;

struct _Emu8086AppWindowState
{
    char file_name[256];
    gboolean isSaved;
    char file_path[256];
    gboolean file_path_set;
    gboolean Open;
    gint fontSize;
};



void emu8086_app_window_open(Emu8086AppWindow *win,
                              GFile *file);
void emu8086_app_window_up(Emu8086AppWindow *win);
void emu8086_app_window_set_app(Emu8086AppWindow *win, Emu8086App *app);
Emu8086AppWindow *emu8086_app_window_new(Emu8086App *app);

void emu8086_app_window_set_open(Emu8086AppWindow *win);
void emu8086_app_window_arr_sum_activate_cb(Emu8086AppWindow *win);
void emu8086_app_window_rev_str_activate_cb(Emu8086AppWindow *win);
void emu8086_app_window_save_activate_cb(Emu8086AppWindow *win);
void emu8086_app_window_save_as_activate_cb(Emu8086AppWindow *win);
void emu8086_app_window_open_activate_cb(Emu8086AppWindow *win);
void emu8086_app_window_stop_win(Emu8086AppWindow *win);
void emu8086_app_window_open_drag_data(Emu8086AppWindow *win, GtkSelectionData *selection_data);
void emu8086_app_window_upd(Emu8086AppWindow *win);
gchar *emu8086_app_window_write_to_file(gchar *filename, gchar *buffer, char *buff);
gboolean emu8086_app_window_save_doc(Emu8086AppWindow *win);

GtkWidget *emu8086_app_window_get_revealer(Emu8086AppWindow *win);



GtkWidget *emu8086_app_window_get_stack(Emu8086AppWindow *win);
gboolean emu8086_app_window_open_egs(Emu8086AppWindow *win);

G_END_DECLS
#endif /* __EXAMPLEAPPWIN_H */