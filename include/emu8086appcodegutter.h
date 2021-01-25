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
 * code_gutter.h
 * Code Gutter class
 */




#ifndef _EMU_CODE_GUTTER_C
#define _EMU_CODE_GUTTER_C
#include <gtk/gtk.h>


#define EMU8086_APP_CODE_GUTTER_TYPE (emu8086_app_code_gutter_get_type())
#define EMU8086_CODE_GUTTER_IS_GUTTER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), EMU8086_APP_CODE_GUTTER_TYPE))
// #define PRIV_CODE_GUTTER Emu8086AppCodeGutterPrivate *priv = emu8086_app_code_gutter_get_instance_private(gutter)

#define PRIV_CODE_GUTTER Emu8086AppCodeGutterPrivate *priv = gutter->priv

G_DECLARE_FINAL_TYPE(Emu8086AppCodeGutter, emu8086_app_code_gutter, EMU8086, APP_CODE_GUTTER, GObject)
typedef struct _Emu8086AppCode Emu8086AppCode;
Emu8086AppCodeGutter *emu8086_app_code_gutter_new(Emu8086AppCode *view,
                                                   GtkTextWindowType type);
void draw(Emu8086AppCodeGutter *gutter, cairo_t *cr);
void recalculate_size(Emu8086AppCodeGutter *gutter);
#endif