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
 * code.h
 * Code class
 */

#ifndef _EMU_CODE_C
#define _EMU_CODE_C
#include <gtk/gtk.h>
#include "emu8086apptypes.h"

G_BEGIN_DECLS
#define EMU8086_APP_CODE_TYPE (emu8086_app_code_get_type())
#define EMU8086_APP_CODE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), EMU8086_APP_CODE_TYPE, Emu8086AppCode))
#define EMU8086_APP_CODE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), EMU8086_APP_CODE_TYPE, Emu8086AppCodeClass))
#define EMU8086_IS_APP_CODE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), EMU8086_APP_CODE_TYPE))
#define EMU8086_IS_APP_CODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), EMU8086_APP_CODE_TYPE))
#define EMU8086_APP_CODE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), EMU8086_APP_CODE_TYPE, Emu8086AppCodeClass))
#define PRIV_CODE Emu8086AppCodePrivate *priv = code->priv

typedef struct _Emu8086AppCodeClass Emu8086AppCodeClass;
typedef struct _Emu8086AppCodePrivate Emu8086AppCodePrivate;

struct _Emu8086AppCode
{
    GtkTextView parent;
    gchar *font;
    gchar *color;
    Emu8086AppCodePrivate *priv;
};

struct _Emu8086AppCodeClass
{
    GtkTextViewClass parent_class;
    /* Signals */
    void (*undo)(Emu8086AppCode *);
    void (*redo)(Emu8086AppCode *);
    void (*move_lines)(Emu8086AppCode *code,
                       gboolean down);
    gpointer padding[2];
};

// G_DECLARE_FINAL_TYPE(Emu8086AppCode, emu8086_app_code, EMU8086, APP_CODE, GtkTextView)
GType emu8086_app_code_get_type(void) G_GNUC_CONST;
Emu8086AppCode *emu8086_app_code_new(void);
gboolean check_for_break_points(Emu8086AppCode *code, gint line_num,
                                gboolean toggle);
void select_line(GtkWidget *code, gint line);
void reset_code(GtkWidget *co);
void editFontSize(Emu8086AppCode *code, gint size);
void set_win(Emu8086AppCode *code, GtkWidget *win);
void get_break_points(Emu8086AppCode *code, gint *bps, gint *len);
Emu8086AppCode *create_new();
void emu8086_app_code_undo(Emu8086AppCode *code);
void emu8086_app_code_redo(Emu8086AppCode *code);
void emu8086_app_code_set_show_lines(Emu8086AppCode *code, gboolean show);
G_END_DECLS
#endif // MACRO
