#include <gtk/gtk.h>
#include <code_gutter.h>
#include <code_buffer.h>

static void emu_8086_app_code_gutter_init(Emu8086AppCodeGutter *gutter);
static void set_code(Emu8086AppCodeGutter *gutter, Emu8086AppCode *code);

static void emu_8086_app_code_gutter_class_init(Emu8086AppCodeGutterClass *klass);

static void emu_8086_app_code_gutter_set_property(GObject *object,
                                                  guint property_id,
                                                  const GValue *value,
                                                  GParamSpec *pspec);

static void
emu_8086_app_code_gutter_get_property(GObject *object,
                                      guint property_id,
                                      GValue *value,
                                      GParamSpec *pspec);

typedef enum
{
    PROP_0,
    PROP_GUTTER_FONT,
    PROP_GUTTER_THEME,
    PROP_GUTTER_CODE,
    PROP_GUTTER_WINDOW_TYPE
} Emu8086AppCodeGutterProperty;

typedef struct _Emu8086AppCodeGutterPrivate Emu8086AppCodeGutterPrivate;

struct _Emu8086AppCodeGutterPrivate
{
    Emu8086AppCode *code;
    GtkTextWindowType window_type;
    gint num_line_digits;
    gint num_lines;
    gint size;
    gint height;
    Emu8086AppCodeBuffer *buffer;
    gchar *text;
    gint cl;
    PangoLayout *cached_layout;
};

struct _Emu8086AppCodeGutter
{
    GObject parent;
    gchar *font;
    gchar *theme;
    GtkTextWindowType window_type;
    Emu8086AppCodeGutterPrivate *priv;
};

G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppCodeGutter, emu_8086_app_code_gutter, G_TYPE_OBJECT);

Emu8086AppCodeGutter *emu_8086_app_code_gutter_new(Emu8086AppCode *code, GtkTextWindowType type)
{
    return g_object_new(EMU_8086_APP_CODE_GUTTER_TYPE,
                        "code", code,
                        "window_type", type,
                        NULL);
};

static void set_code(Emu8086AppCodeGutter *gutter, Emu8086AppCode *code)
{

    PRIV_CODE_GUTTER;

    priv->code = code;
    priv->buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(code));
}

static void emu_8086_app_code_gutter_set_property(GObject *object,
                                                  guint property_id,
                                                  const GValue *value,
                                                  GParamSpec *pspec)
{
    Emu8086AppCodeGutter *self = EMU_8086_APP_CODE_GUTTER(object);
    // g_print("l %d\n", *value);

    switch ((Emu8086AppCodeGutterProperty)property_id)
    {
    case PROP_GUTTER_THEME:
        // *v = (gboolean *)value;

        self->theme = g_value_get_string(value);
        // g_print("filename: %s\n", self->filename);
        break;

    case PROP_GUTTER_FONT:
        // *v = (gboolean *)value;

        self->font = g_value_get_string(value);
        // emu8086_win_change_theme(self);
        // g_print("filename: %s\n", self->filename);
        break;

    case PROP_GUTTER_CODE:
        set_code(self, EMU_8086_APP_CODE(g_value_get_object(value)));
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
emu_8086_app_code_gutter_get_property(GObject *object,
                                      guint property_id,
                                      GValue *value,
                                      GParamSpec *pspec)
{
    Emu8086AppCodeGutter *self = EMU_8086_APP_CODE_GUTTER(object);

    switch ((Emu8086AppCodeGutterProperty)property_id)
    {
    case PROP_GUTTER_CODE:
        g_value_set_object(value, self->priv->code);
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

static void emu_8086_app_code_gutter_class_init(Emu8086AppCodeGutterClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->set_property = emu_8086_app_code_gutter_set_property;
    object_class->get_property = emu_8086_app_code_gutter_get_property;

    g_object_class_install_property(object_class,
                                    PROP_GUTTER_CODE,
                                    g_param_spec_object("code",
                                                        "Code",
                                                        "",
                                                        EMU_8086_APP_CODE_TYPE,
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

static void emu_8086_app_code_gutter_init(Emu8086AppCodeGutter *gutter)
{
    gutter->priv = emu_8086_app_code_gutter_get_instance_private(gutter);
    gutter->priv->text = NULL;
    gutter->priv->cached_layout = NULL;
    gutter->priv->cl = -1;
}

static inline gint
count_num_digits(gint num_lines)
{
    if (num_lines < 100)
    {
        return 2;
    }
    else if (num_lines < 1000)
    {
        return 3;
    }
    else if (num_lines < 10000)
    {
        return 4;
    }
    else if (num_lines < 100000)
    {
        return 5;
    }
    else if (num_lines < 1000000)
    {
        return 6;
    }
    else
    {
        return 10;
    }
}

static void measure_text(Emu8086AppCodeGutter *gutter, const gchar *markup, gint *width)
{
    g_return_if_fail(markup != NULL);

    PRIV_CODE_GUTTER;

    Emu8086AppCode *code;
    PangoLayout *layout;
    code = priv->code;
    gint height = 0;
    // if(priv->cached_layout != NULL )pango_layfr
    layout = gtk_widget_create_pango_layout(GTK_WIDGET(code), NULL);

    if (markup != NULL)
    {
        pango_layout_set_markup(layout, markup, -1);
    }

    pango_layout_get_pixel_size(layout, width, height);
    priv->height = height;

    g_object_unref(layout);
}

void recalculate_size(Emu8086AppCodeGutter *gutter)
{
    PRIV_CODE_GUTTER;
    Emu8086AppCodeBuffer *buffer;
    gint num_lines;
    gint num_digits = 0;
    buffer = priv->buffer;
    num_lines = gtk_text_buffer_get_line_count(buffer);
    priv->num_lines = num_lines;
    num_digits = count_num_digits(num_lines);

    if (num_digits != priv->num_line_digits)
    {
        gchar markup[24];
        gint size;
        priv->num_line_digits = num_digits;

        num_lines = MAX(num_lines, 99);

        g_snprintf(markup, sizeof markup, "<b>%d</b>", num_lines);

        measure_text(gutter, markup, &size);
        priv->size = size;
    }
}

static void
gutter_renderer_query_data(Emu8086AppCodeGutter *gutter)
{
    PRIV_CODE_GUTTER;
    gint line;
    line = priv->num_lines;
    gchar text[24];
    const gchar *textptr = text;
    // gint line;
    gint len;
    if (priv->text != NULL)
    {
        g_free(priv->text);
        priv->text = NULL;
    }
    line = line + 1;
    len = g_snprintf(text, sizeof text, "<b>%d</b>", line);
    priv->text = len >= 0 ? g_strndup(text, len) : g_strdup(text);
}

static gint
get_last_visible_line_number(Emu8086AppCodeGutter *gutter)
{
    PRIV_CODE_GUTTER;

    Emu8086AppCode *view;
    GdkRectangle visible_rect;
    GtkTextIter iter;

    view = priv->code;

    gtk_text_view_get_visible_rect(view, &visible_rect);

    gtk_text_view_get_line_at_y(view,
                                &iter,
                                visible_rect.y + visible_rect.height,
                                NULL);

    gtk_text_iter_forward_line(&iter);

    return gtk_text_iter_get_line(&iter);
}

void draw(Emu8086AppCodeGutter *gutter, cairo_t *cr)
{
    PRIV_CODE_GUTTER;
    Emu8086AppCode *view;
    GtkTextIter start;
    GdkRectangle clip;
    GtkTextView *text_view;
    gint first_y_window_coord;
    gint last_y_window_coord;
    gint first_y_buffer_coord;
    gint last_y_buffer_coord;
    GArray *renderer_widths;
    gutter_renderer_query_data(gutter);
    g_return_if_fail(priv->text != NULL);
    g_return_if_fail(priv->cl != priv->num_lines);
    // g_print("%s\n", priv->text);
    // priv->cl = priv->num_lines;
}