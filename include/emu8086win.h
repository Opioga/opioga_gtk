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