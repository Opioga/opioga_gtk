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
    gboolean is_drawing;
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
    // priv->num_lines = num_lines;
    num_digits = count_num_digits(num_lines);

    if (num_digits != priv->num_line_digits)
    {
        gchar markup[27];
        gint size;
        priv->num_line_digits = num_digits;

        num_lines = MAX(num_lines, 99);

        g_snprintf(markup, sizeof markup, "<span>%d</span>", num_lines);

        measure_text(gutter, markup, &size);
        priv->size = size;
        gtk_text_view_set_border_window_size(priv->code,
                                             GTK_TEXT_WINDOW_LEFT,
                                             size + 10);
    }
}

static void
gutter_renderer_query_data(Emu8086AppCodeGutter *gutter)
{
    PRIV_CODE_GUTTER;
    gint line;
    line = priv->num_lines;
    gchar text[27];
    const gchar *textptr = text;
    // gint line;
    gint len;
    if (priv->text != NULL)
    {
        g_free(priv->text);
        priv->text = NULL;
    }
    line = line + 1;

    len = g_snprintf(text, sizeof text, "<span>%d</span>", line);
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

static gboolean
get_clip_rectangle(Emu8086AppCodeGutter *gutter,
                   Emu8086AppCode *view,
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

static LinesInfo *
get_lines_info(Emu8086AppCode *text_view,
               gint first_y_buffer_coord,
               gint last_y_buffer_coord)
{
    LinesInfo *info;
    GtkTextIter iter;
    gint last_line_num = -1;

    info = lines_info_new();

    /* Get iter at first y */
    gtk_text_view_get_line_at_y(text_view, &iter, first_y_buffer_coord, NULL);

    info->start = iter;

    /* For each iter, get its location and add it to the arrays.
	 * Stop when we pass last_y_buffer_coord.
	 */
    while (!gtk_text_iter_is_end(&iter))
    {
        gint y;
        gint height;
        gint line_num;

        gtk_text_view_get_line_yrange(text_view, &iter, &y, &height);

        g_array_append_val(info->buffer_coords, y);
        g_array_append_val(info->line_heights, height);

        info->total_height += height;

        line_num = gtk_text_iter_get_line(&iter);
        g_array_append_val(info->line_numbers, line_num);

        last_line_num = line_num;

        info->lines_count++;

        if (last_y_buffer_coord <= (y + height))
        {
            break;
        }

        gtk_text_iter_forward_line(&iter);
    }

    if (gtk_text_iter_is_end(&iter))
    {
        gint y;
        gint height;
        gint line_num;

        gtk_text_view_get_line_yrange(text_view, &iter, &y, &height);

        line_num = gtk_text_iter_get_line(&iter);

        if (line_num != last_line_num)
        {
            g_array_append_val(info->buffer_coords, y);
            g_array_append_val(info->line_heights, height);

            info->total_height += height;

            g_array_append_val(info->line_numbers, line_num);
            info->lines_count++;
        }
    }

    if (info->lines_count == 0)
    {
        gint y = 0;
        gint n = 0;
        gint height;

        info->lines_count = 1;

        g_array_append_val(info->buffer_coords, y);
        g_array_append_val(info->line_numbers, n);

        gtk_text_view_get_line_yrange(text_view, &iter, &y, &height);
        g_array_append_val(info->line_heights, height);

        info->total_height += height;
    }

    info->end = iter;

    return info;
}

static void
draw_cells(Emu8086AppCodeGutter *gutter,
           GtkTextView *view,
           gint renderer_widths,
           LinesInfo *info,
           cairo_t *cr)
{
    PRIV_CODE_GUTTER;
    GtkTextBuffer *buffer;
    GtkTextIter insert_iter;
    gint cur_line;
    GtkTextIter selection_start;
    GtkTextIter selection_end;
    gint selection_start_line = 0;
    gint selection_end_line = 0;
    gboolean has_selection;
    GtkTextIter start;
    gint i;

    // color.red
    buffer = priv->buffer;

    gtk_text_buffer_get_iter_at_mark(buffer,
                                     &insert_iter,
                                     gtk_text_buffer_get_insert(buffer));

    cur_line = gtk_text_iter_get_line(&insert_iter);

    start = info->start;
    i = 0;
    while (i < info->lines_count)
    {
        GtkTextIter end;
        GdkRectangle background_area;
        gint pos;
        gint line_to_paint;
        gint renderer_num;
        GList *l;

        end = start;

        if (!gtk_text_iter_ends_line(&end))
        {

            if (gtk_text_iter_forward_line(&end))
            {
                gtk_text_iter_backward_char(&end);
            }
        }
        gtk_text_view_buffer_to_window_coords(view,
                                              gutter->priv->window_type,
                                              0,
                                              g_array_index(info->buffer_coords, gint, i),
                                              NULL,
                                              &pos);

        line_to_paint = g_array_index(info->line_numbers, gint, i);
        priv->num_lines = line_to_paint;
        background_area.y = pos;
        background_area.height = g_array_index(info->line_heights, gint, i);
        background_area.x = 0;
        recalculate_size(gutter);
        GdkRectangle cell_area;
        gint width, height, x, y;
        gint xpad = 5;
        gint ypad = 0;

        background_area.width = priv->size + 10;
        gtk_text_view_set_border_window_size(priv->code,
                                             GTK_TEXT_WINDOW_LEFT,
                                             background_area.width + 10);
        // g_print("%s %d", "lion", priv->size);
        cell_area.y = background_area.y + ypad;
        cell_area.height = background_area.height - 2 * ypad;

        cell_area.x = background_area.x + xpad;
        cell_area.width = background_area.width - 2 * xpad;
        gutter_renderer_query_data(gutter);

        cairo_save(cr);

        gdk_cairo_rectangle(cr, &background_area);
        
        cairo_clip(cr);

        if (priv->cached_layout == NULL)
            priv->cached_layout = gtk_widget_create_pango_layout(GTK_WIDGET(priv->code), NULL);

        if (check_for_break_points(priv->code, line_to_paint, FALSE))

        {
            pango_layout_set_markup(priv->cached_layout, "<span foreground=\"red\">•</span>", -1);
        }

        else
            pango_layout_set_markup(priv->cached_layout, priv->text, -1);

        pango_layout_get_pixel_size(priv->cached_layout, &width, &height);

        x = cell_area.x; // + (cell_area.width - width)*0
        y = cell_area.y; // + (cell_area->height - height) * 0;
        GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(view));

        gtk_render_layout(context, cr, x, y, priv->cached_layout);
        cairo_restore(cr);

        // gtk_text_iter_forward_line(&start);
        i++;
    }
}

static void
lines_info_free(LinesInfo *info)
{
    if (info != NULL)
    {
        g_array_free(info->buffer_coords, TRUE);
        g_array_free(info->line_heights, TRUE);
        g_array_free(info->line_numbers, TRUE);

        g_slice_free(LinesInfo, info);
    }
}

void draw(Emu8086AppCodeGutter *gutter, cairo_t *cr)
{
    PRIV_CODE_GUTTER;
    Emu8086AppCode *view = priv->code;
    GtkTextIter start;
    GdkRectangle clip;
    LinesInfo *info;
    gint first_y_window_coord;
    gint last_y_window_coord;
    gint first_y_buffer_coord;
    gint last_y_buffer_coord;
    gint width;

    if (!get_clip_rectangle(gutter, view, cr, &clip))
    {
        return;
    }
    priv->is_drawing = TRUE;

    first_y_window_coord = clip.y;
    last_y_window_coord = first_y_window_coord + clip.height;

    gtk_text_view_window_to_buffer_coords(view,
                                          gutter->priv->window_type,
                                          0,
                                          first_y_window_coord,
                                          NULL,
                                          &first_y_buffer_coord);

    gtk_text_view_window_to_buffer_coords(view,
                                          gutter->priv->window_type,
                                          0,
                                          last_y_window_coord,
                                          NULL,
                                          &last_y_buffer_coord);

    info = get_lines_info(view,
                          first_y_buffer_coord,
                          last_y_buffer_coord);

    draw_cells(gutter,
               view,
               20,
               info,
               cr);

    lines_info_free(info);

    // // GtkTextView *text_view;
}