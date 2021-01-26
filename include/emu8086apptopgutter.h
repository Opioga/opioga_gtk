/* gtkemu8086
 * Copyright 2020 KRC
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source top must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * top_gutter.h
 * Top Gutter class
 */




#ifndef _EMU_TOP_GUTTER_C
#define _EMU_TOP_GUTTER_C
#include <gtk/gtk.h>


#define EMU8086_APP_TOP_GUTTER_TYPE (emu8086_app_top_gutter_get_type())
#define EMU8086_TOP_GUTTER_IS_GUTTER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), EMU8086_APP_TOP_GUTTER_TYPE))
// #define PRIV_TOP_GUTTER Emu8086AppTopGutterPrivate *priv = emu8086_app_top_gutter_get_instance_private(gutter)

#define PRIV_TOP_GUTTER Emu8086AppTopGutterPrivate *priv = gutter->priv

G_DECLARE_FINAL_TYPE(Emu8086AppTopGutter, emu8086_app_top_gutter, EMU8086, APP_TOP_GUTTER, GObject)

Emu8086AppTopGutter *emu8086_app_top_gutter_new(GtkTextView *view,
                                                   GtkTextWindowType type);
void top_draw(Emu8086AppTopGutter *gutter, cairo_t *cr);

#endif