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
 * top_gutter.c
 * Top Gutter class
 */

#include <gtk/gtk.h>

#include <emu8086apptopgutter.h>
#include <emu8086stylescheme.h>

static void emu8086_app_top_gutter_init(Emu8086AppTopGutter *gutter);
static void set_top(Emu8086AppTopGutter *gutter, GtkTextView *top);

static void emu8086_app_top_gutter_class_init(Emu8086AppTopGutterClass *klass);

static void emu8086_app_top_gutter_set_property(GObject *object,
                                                guint property_id,
                                                const GValue *value,
                                                GParamSpec *pspec);

static void
emu8086_app_top_gutter_get_property(GObject *object,
                                    guint property_id,
                                    GValue *value,
                                    GParamSpec *pspec);

typedef enum
{
    PROP_0,
    PROP_GUTTER_FONT,
    PROP_GUTTER_THEME,
    PROP_GUTTER_TOP,
    PROP_GUTTER_WINDOW_TYPE
} Emu8086AppTopGutterProperty;

typedef struct _LinesInfo LinesInfo;

struct _LinesInfo
{
    gint total_height;
    gint lines_count;
    GArray *buffer_coords;
    GArray *line_heights;
    GArray *line_numbers;
    GtkTextIter start;
    GtkTextIter end;
};

static LinesInfo *
lines_info_new(void)
{
    LinesInfo *info;

    info = g_slice_new0(LinesInfo);

    info->buffer_coords = g_array_new(FALSE, FALSE, sizeof(gint));
    info->line_heights = g_array_new(FALSE, FALSE, sizeof(gint));
    info->line_numbers = g_array_new(FALSE, FALSE, sizeof(gint));

    return info;
}

typedef struct _Emu8086AppTopGutterPrivate Emu8086AppTopGutterPrivate;

struct _Emu8086AppTopGutterPrivate
{
    GtkTextView *view;
    GtkTextWindowType window_type;
    gint num_line_digits;
    gint num_lines;
    gint size;
    gint height;

    gchar *text;
    gchar *foreground;
    gint cl;
    PangoLayout *cached_layout;
    gboolean is_drawing;
    Emu8086AppStyleScheme *scheme;
};

struct _Emu8086AppTopGutter
{
    GObject parent;
    gchar *font;
    gchar *theme;
    GtkTextWindowType window_type;
    Emu8086AppTopGutterPrivate *priv;
};

G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppTopGutter, emu8086_app_top_gutter, G_TYPE_OBJECT);

Emu8086AppTopGutter *emu8086_app_top_gutter_new(GtkTextView *view, GtkTextWindowType type)
{
    return g_object_new(EMU8086_APP_TOP_GUTTER_TYPE,
                        "text-v", view,
                        "window_type", type,
                        NULL);
};

static void emu8086_app_top_gutter_set_property(GObject *object,
                                                guint property_id,
                                                const GValue *value,
                                                GParamSpec *pspec)
{
    Emu8086AppTopGutter *self = EMU8086_APP_TOP_GUTTER(object);
 
    switch ((Emu8086AppTopGutterProperty)property_id)
    {
    case PROP_GUTTER_THEME:
 
        self->theme = g_value_get_string(value);
         break;

    case PROP_GUTTER_FONT:
 
        self->font = g_value_get_string(value);
       
        break;

    case PROP_GUTTER_TOP:
        set_top(self, GTK_TEXT_VIEW(g_value_get_object(value)));
        break;

    case PROP_GUTTER_WINDOW_TYPE:
        self->priv->window_type = g_value_get_enum(value);
        break;
    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void
emu8086_app_top_gutter_get_property(GObject *object,
                                    guint property_id,
                                    GValue *value,
                                    GParamSpec *pspec)
{
    Emu8086AppTopGutter *self = EMU8086_APP_TOP_GUTTER(object);

    switch ((Emu8086AppTopGutterProperty)property_id)
    {
    case PROP_GUTTER_TOP:
        g_value_set_object(value, self->priv->view);
        break;
    case PROP_GUTTER_WINDOW_TYPE:
        g_value_set_enum(value, self->priv->window_type);
        break;
    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void emu8086_app_top_gutter_class_init(Emu8086AppTopGutterClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->set_property = emu8086_app_top_gutter_set_property;
    object_class->get_property = emu8086_app_top_gutter_get_property;

    g_object_class_install_property(object_class,
                                    PROP_GUTTER_TOP,
                                    g_param_spec_object("text-v",
                                                        "text-v",
                                                        "",
                                                        GTK_TYPE_TEXT_VIEW,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property(object_class,
                                    PROP_GUTTER_WINDOW_TYPE,
                                    g_param_spec_enum("window_type",
                                                      "Window Type",
                                                      "The gutters' text window type",
                                                      GTK_TYPE_TEXT_WINDOW_TYPE,
                                                      GTK_TEXT_WINDOW_LEFT,
                                                      G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

static void emu8086_top_gutter_refresh_fg(Emu8086AppStyleScheme *scheme, Emu8086AppTopGutter *gutter)
{
    PRIV_TOP_GUTTER;
    priv->foreground = emu8086_app_style_scheme_get_color_by_index(scheme,
                                                                   13);
}

static void emu8086_app_top_gutter_init(Emu8086AppTopGutter *gutter)
{
    gutter->priv = emu8086_app_top_gutter_get_instance_private(gutter);
    gutter->priv->text = NULL;
    gutter->priv->scheme = emu8086_app_style_scheme_get_default();
    gutter->priv->foreground = emu8086_app_style_scheme_get_color_by_index(gutter->priv->scheme,
                                                                           13);
    gutter->priv->cached_layout = NULL;
    gutter->priv->cl = -1;
    g_signal_connect(gutter->priv->scheme, "theme_changed", G_CALLBACK(emu8086_top_gutter_refresh_fg), gutter);
}

static gboolean
get_clip_rectangle(Emu8086AppTopGutter *gutter,
                   GtkTextView *view,
                   cairo_t *cr,
                   GdkRectangle *clip)
{
    GdkWindow *window = gtk_text_view_get_window(GTK_TEXT_VIEW(view),
                                                 gutter->priv->window_type);

    if (window == NULL || !gtk_cairo_should_draw_window(cr, window))
    {
        if (window == NULL)
            //  g_print("77lon\n");

            return FALSE;
    }

    gtk_cairo_transform_to_window(cr, GTK_WIDGET(view), window);

    return gdk_cairo_get_clip_rectangle(cr, clip);
}

static void set_top(Emu8086AppTopGutter *gutter, GtkTextView *top)
{
    gutter->priv->view = top;
}

void top_draw(Emu8086AppTopGutter *gutter, cairo_t *cr)
{
    PRIV_TOP_GUTTER; 
}