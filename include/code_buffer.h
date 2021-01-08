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
 * code_buffer.h
 * Code Buffer class
 */


#ifndef _EMU_CODE_BUFFER_C
#define _EMU_CODE_BUFFER_C
#include <gtk/gtk.h>
#include <code.h>

#define EMU_8086_APP_CODE_BUFFER_TYPE (emu_8086_app_code_buffer_get_type())
#define EMU_8086_CODE_BUFFER_IS_BUFFER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), EMU_8086_APP_CODE_BUFFER_TYPE))
#define PRIV_CODE_BUFFER Emu8086AppCodeBufferPrivate *priv = emu_8086_app_code_buffer_get_instance_private(buffer)

// #define PRIV_CODE_BUFFER Emu8086AppCodeBufferPrivate *priv = buffer->priv

G_DECLARE_FINAL_TYPE(Emu8086AppCodeBuffer, emu_8086_app_code_buffer, EMU_8086, APP_CODE_BUFFER, GtkTextBuffer)

Emu8086AppCodeBuffer *emu_8086_app_code_buffer_new(GtkTextTagTable *table);
void refreshLines(Emu8086AppCodeBuffer *buffer);
void emu_8086_app_code_buffer_indent(Emu8086AppCode *buffer);
#endif