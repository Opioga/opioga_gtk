#ifndef _EMU_STYLE_SCHEME_C
#define _EMU_STYLE_SCHEME_C
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define EMU_8086_APP_STYLE_SCHEME_TYPE (emu_8086_app_style_scheme_get_type())
#define EMU_8086_STYLE_SCHEME_IS_scheme(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), EMU_8086_APP_STYLE_SCHEME_TYPE))
#define PRIV_STYLE_SCHEME Emu8086AppStyleSchemePrivate *priv = scheme->priv
#define EMU_8086_APP_STYLE_SCHEME(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), EMU_8086_APP_STYLE_SCHEME_TYPE, Emu8086AppStyleScheme))
#define EMU_8086_APP_STYLE_SCHEME_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), EMU_8086_APP_STYLE_SCHEME_TYPE, Emu8086AppStyleSchemeClass))
#define EMU_8086_STYLE_SCHEME_IS_scheme_CLASS(klass)(G_TYPE_CHECK_CLASS_TYPE((klass), EMU_8086_APP_STYLE_SCHEME_TYPE)

typedef struct _Emu8086AppStyleSchemePrivate Emu8086AppStyleSchemePrivate;

struct _Emu8086AppStyleSchemePrivate
{
    /* data */
    gchar *theme;

    gchar *color_keyword;
    gchar *color_reg;
    gchar *color_string;
    gchar *color_label_def;
    gchar *color_num;
    gchar *color_special;
    gchar *color_comment;
    gchar *color_highlight;

   gchar *color_background;
gchar *color_linecolor;
gchar *color_cursor;
 gchar *color_selection;
 gchar *color_selectionbg;
gchar *color_text;
};

typedef struct _Emu8086AppStyleScheme Emu8086AppStyleScheme;
struct _Emu8086AppStyleScheme
{
    GObject parent;
    Emu8086AppStyleSchemePrivate *priv;
};

typedef struct _Emu8086AppStyleSchemeClass Emu8086AppStyleSchemeClass;

struct _Emu8086AppStyleSchemeClass
{
    GObjectClass parent_class;
      void (*theme_changed)(Emu8086AppStyleScheme *scheme);

  
};
typedef enum
{
    PROP_SCHEME_0,
    PROP_SCHEME_THEME

} Emu8086AppStyleSchemeProperty;

GType emu_8086_app_style_scheme_get_type(void) G_GNUC_CONST;


Emu8086AppStyleScheme *emu_8086_app_style_scheme_get_default();
gchar *emu_8086_app_style_scheme_get_color_by_index(Emu8086AppStyleScheme *scheme,const gint index);
gchar *emu_8086_app_style_scheme_get_color(Emu8086AppStyleScheme *scheme,const gchar *key);
G_END_DECLS
#endif