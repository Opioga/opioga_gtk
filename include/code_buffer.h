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

G_BEGIN_DECLS

#define EMU_8086_APP_CODE_BUFFER_TYPE (emu_8086_app_code_buffer_get_type())

#define EMU_8086_APP_CODE_BUFFER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), EMU_8086_APP_CODE_BUFFER_TYPE, Emu8086AppCodeBuffer))
#define EMU_8086_APP_CODE_BUFFER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), EMU_8086_APP_CODE_BUFFER_TYPE, Emu8086AppCodeBufferClass))
#define EMU_8086_IS_APP_CODE_BUFFER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), EMU_8086_APP_CODE_BUFFER_TYPE))
#define EMU_8086_IS_APP_CODE_BUFFER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), EMU_8086_APP_CODE_BUFFER_TYPE))
#define EMU_8086_APP_CODE_BUFFER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), EMU_8086_APP_CODE_BUFFER_TYPE, Emu8086AppCodeBufferClass))
#define PRIV_CODE_BUFFER Emu8086AppCodeBufferPrivate *priv = buffer->priv

// #define PRIV_CODE_BUFFER Emu8086AppCodeBufferPrivate *priv = buffer->priv

// G_DECLARE_FINAL_TYPE(Emu8086AppCodeBuffer, emu_8086_app_code_buffer, EMU_8086, APP_CODE_BUFFER, GtkTextBuffer)
typedef struct _Emu8086AppCodeBuffer Emu8086AppCodeBuffer;
typedef struct _Emu8086AppCodeBufferClass Emu8086AppCodeBufferClass;
typedef struct _Emu8086AppCodeBufferPrivate Emu8086AppCodeBufferPrivate;

struct _Emu8086AppCodeBuffer
{
    GtkTextBuffer parent;
    gchar *theme;
    Emu8086AppCodeBufferPrivate *priv;
};

struct _Emu8086AppCodeBufferClass
{
    GtkTextBufferClass parent_class;
    /* Signals */
    void (*undo)(Emu8086AppCodeBuffer *buffer);
    void (*redo)(Emu8086AppCodeBuffer *buffer);
    gpointer padding[20];
};
GType emu_8086_app_code_buffer_get_type(void) G_GNUC_CONST;
Emu8086AppCodeBuffer *
emu_8086_app_code_buffer_new(GtkTextTagTable *table);
void refreshLines(Emu8086AppCodeBuffer *buffer);
void emu_8086_app_code_buffer_indent(Emu8086AppCodeBuffer *buffer);

gboolean emu_8086_app_code_buffer_get_can_redo(Emu8086AppCodeBuffer *buffer);
gboolean emu_8086_app_code_buffer_get_can_undo(Emu8086AppCodeBuffer *buffer);


// TODO implement redo and undo 
void emu_8086_app_code_buffer_redo(Emu8086AppCodeBuffer *buffer);
void emu_8086_app_code_buffer_undo(Emu8086AppCodeBuffer *buffer);
G_END_DECLS

#endif