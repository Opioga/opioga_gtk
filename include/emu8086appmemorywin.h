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

#ifndef MEM_WIN_C
#define MEM_WIN_C


#include <gtk/gtk.h>
#include "emu8086app.h"
#include <emu8086.h>
#include "emu8086apptypes.h"

G_BEGIN_DECLS

#define PRIV_AMW Emu8086AppMemoryWindowPrivate *priv = win->priv
#define EMU8086_APP_MEMORY_WINDOW_TYPE (emu8086_app_memory_window_get_type())
G_DECLARE_FINAL_TYPE(Emu8086AppMemoryWindow, emu8086_app_memory_window, EMU_8086, APP_MEMORY_WINDOW, GtkWindow)

Emu8086AppMemoryWindow * emu8086_app_memory_window_open(GtkWindow *parent_win, Emu8086AppCodeRunner *win);
void emu8086_app_memory_window_close(GtkButton *btn,Emu8086AppMemoryWindow *win);

G_END_DECLS
#endif /* __EXAMPLEAPPMEM_WIN_H */
