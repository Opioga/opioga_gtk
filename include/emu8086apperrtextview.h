/* gtkemu8086
 * Copyright 2020 KRC
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source errtextview must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * errtextview.h
 * ErrTextView class
 */

#ifndef _EMU_ERR_TEXT_VIEW_C
#define _EMU_ERR_TEXT_VIEW_C
#include <gtk/gtk.h>
#include "emu8086apptypes.h"

G_BEGIN_DECLS
#define EMU8086_APP_ERR_TEXT_VIEW_TYPE (emu8086_app_err_text_view_get_type())
#define EMU8086_APP_ERR_TEXT_VIEW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), EMU8086_APP_ERR_TEXT_VIEW_TYPE, Emu8086AppErrTextView))
#define EMU8086_APP_ERR_TEXT_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), EMU8086_APP_ERR_TEXT_VIEW_TYPE, Emu8086AppErrTextViewClass))
#define EMU8086_IS_APP_ERR_TEXT_VIEW(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), EMU8086_APP_ERR_TEXT_VIEW_TYPE))
#define EMU8086_IS_APP_ERR_TEXT_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), EMU8086_APP_ERR_TEXT_VIEW_TYPE))
#define EMU8086_APP_ERR_TEXT_VIEW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), EMU8086_APP_ERR_TEXT_VIEW_TYPE, Emu8086AppErrTextViewClass))
#define PRIV_ERR_TEXT_VIEW Emu8086AppErrTextViewPrivate *priv = view->priv

typedef struct _Emu8086AppErrTextViewClass Emu8086AppErrTextViewClass;
typedef struct _Emu8086AppErrTextViewPrivate Emu8086AppErrTextViewPrivate;

struct _Emu8086AppErrTextView
{
    GtkTextView parent;

    Emu8086AppErrTextViewPrivate *priv;
};

struct _Emu8086AppErrTextViewClass
{
    GtkTextViewClass parent_class;

    gpointer padding[2];
};

// G_DECLARE_FINAL_TYPE(Emu8086AppErrTextView, emu8086_app_err_text_view, EMU8086, APP_ERR_TEXT_VIEW, GtkTextView)
GType emu8086_app_err_text_view_get_type(void) G_GNUC_CONST;
Emu8086AppErrTextView *emu8086_app_err_text_view_new(void);
void emu8086_app_err_set_win(Emu8086AppErrTextView *view, Emu8086AppWindow *win);
G_END_DECLS
#endif // MACRO
