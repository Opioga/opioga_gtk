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

#ifndef _EMU_CODE_BUFFER_C
#define _EMU_CODE_BUFFER_C
#include <gtk/gtk.h>

#define EMU_8086_APP_CODE_BUFFER_TYPE (emu_8086_app_code_buffer_get_type())
#define EMU_8086_CODE_BUFFER_IS_BUFFER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), EMU_8086_APP_CODE_BUFFER_TYPE))
#define PRIV_CODE_BUFFER Emu8086AppCodeBufferPrivate *priv = emu_8086_app_code_buffer_get_instance_private(buffer)


// #define PRIV_CODE_BUFFER Emu8086AppCodeBufferPrivate *priv = buffer->priv

G_DECLARE_FINAL_TYPE(Emu8086AppCodeBuffer, emu_8086_app_code_buffer, EMU_8086, APP_CODE_BUFFER, GtkTextBuffer)


Emu8086AppCodeBuffer *emu_8086_app_code_buffer_new(GtkTextTagTable *table);
void refreshLines(Emu8086AppCodeBuffer *buffer);


#endif