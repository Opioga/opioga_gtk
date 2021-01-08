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
#include "emu8086win.h"

#define EMU_8086_APP_CODE_TYPE (emu_8086_app_code_get_type())
#define PRIV_CODE Emu8086AppCodePrivate *priv = code->priv

G_DECLARE_FINAL_TYPE(Emu8086AppCode, emu_8086_app_code, EMU_8086, APP_CODE, GtkTextView)

Emu8086AppCode *emu_8086_app_code_new(void);
gboolean check_for_break_points(Emu8086AppCode *code, gint line_num,
                                gboolean toggle);
void select_line(GtkWidget *code, gint line);
void reset_code(GtkWidget *co);
void editFontSize(Emu8086AppCode *code, gint size);
void set_win(Emu8086AppCode *code, GtkWidget *win);
void get_break_points(Emu8086AppCode *code, gint *bps, gint *len);
Emu8086AppCode *create_new();

#endif // MACRO
