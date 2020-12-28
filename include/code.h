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
#ifndef _EMU_CODE_C
#define _EMU_CODE_C
#include <gtk/gtk.h>
#include "emu8086win.h"

#define EMU_8086_APP_CODE_TYPE (emu_8086_app_code_get_type())
#define PRIV_CODE Emu8086AppCodePrivate *priv = emu_8086_app_code_get_instance_private(code)

G_DECLARE_FINAL_TYPE(Emu8086AppCode, emu_8086_app_code, EMU_8086, APP_CODE, GtkTextView)

Emu8086AppCode *emu_8086_app_code_new(void);
void update(Emu8086AppCode *code);

void select_line(GtkWidget *code, gint line);
void reset_code(GtkWidget *co);
void editFontSize(Emu8086AppCode *code, gint size);
void set_win(Emu8086AppCode *code, GtkWidget *win);
// GtkWidget *create();
Emu8086AppCode *create_new(GtkWidget *box, GtkWidget *box2, Emu8086AppWindow *win);

#endif // MACRO
