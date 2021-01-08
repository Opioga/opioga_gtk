#ifndef EMU8086_PANGO_H
#define EMU8086_PANGO_H

#include <pango/pango.h>

G_BEGIN_DECLS

gchar *emu8086_pango_font_description_to_css(const PangoFontDescription *font_desc, gchar *color);

G_END_DECLS

#endif /* EMU8086_PANGO_H */