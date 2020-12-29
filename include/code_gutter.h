#ifndef _EMU_CODE_GUTTER_C
#define _EMU_CODE_GUTTER_C
#include <gtk/gtk.h>
#include <code.h>

#define EMU_8086_APP_CODE_GUTTER_TYPE (emu_8086_app_code_gutter_get_type())
#define EMU_8086_CODE_GUTTER_IS_GUTTER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), EMU_8086_APP_CODE_GUTTER_TYPE))
// #define PRIV_CODE_GUTTER Emu8086AppCodeGutterPrivate *priv = emu_8086_app_code_gutter_get_instance_private(gutter)

#define PRIV_CODE_GUTTER Emu8086AppCodeGutterPrivate *priv = gutter->priv

G_DECLARE_FINAL_TYPE(Emu8086AppCodeGutter, emu_8086_app_code_gutter, EMU_8086, APP_CODE_GUTTER, GObject)

Emu8086AppCodeGutter *emu_8086_app_code_gutter_new(Emu8086AppCode *view,
                                                   GtkTextWindowType type);
void draw(Emu8086AppCodeGutter *gutter, cairo_t *cr);
void recalculate_size(Emu8086AppCodeGutter *gutter);
#endif