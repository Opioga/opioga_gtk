#include <stdio.h>
#include <emu8086stylescheme.h>

G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppStyleScheme, emu_8086_app_style_scheme, G_TYPE_OBJECT);

enum Emu8086StyleSchemeSignals
{
    DUMMY_E_S,
    EMU_THEME_CHANGED,
    // PORT_STATE_CHANGED,
    LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};
static void emu_8086_app_style_scheme_init(Emu8086AppStyleScheme *scheme);
static void emu_8086_app_style_scheme_load(Emu8086AppStyleScheme *scheme);
static void emu_8086_app_style_scheme_class_init(Emu8086AppStyleSchemeClass *klass);

static void emu_8086_app_style_scheme_set_property(GObject *object,
                                                   guint property_id,
                                                   const GValue *value,
                                                   GParamSpec *pspec)
{
    Emu8086AppStyleScheme *self = EMU_8086_APP_STYLE_SCHEME(object);
    // g_print("l %d\n", *value);
    gchar *m;
    switch ((Emu8086AppStyleSchemeProperty)property_id)
    {

    case PROP_SCHEME_THEME:
        // *v = (gboolean *)value;

        m = g_value_get_string(value);
        if (self->priv->theme != NULL)
        {
               if (strcmp(m, self->priv->theme) == 0)
            {
            
            
                return;
            }
            g_free(self->priv->theme);
        }
     
        self->priv->theme = g_strdup(m);
        emu_8086_app_style_scheme_load(self);

        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
emu_8086_app_style_scheme_get_property(GObject *object,
                                       guint property_id,
                                       GValue *value,
                                       GParamSpec *pspec)
{
    Emu8086AppStyleScheme *self = EMU_8086_APP_STYLE_SCHEME(object);

    switch ((Emu8086AppStyleSchemeProperty)property_id)
    {
    case PROP_SCHEME_THEME:
        g_value_set_boolean(value, self->priv->theme);
        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void emu_8086_app_style_scheme_theme_changed(Emu8086AppStyleScheme *scheme)
{
   return;
}

static void emu_8086_app_style_scheme_class_init(Emu8086AppStyleSchemeClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->set_property = emu_8086_app_style_scheme_set_property;
    object_class->get_property = emu_8086_app_style_scheme_get_property;
    klass->theme_changed = emu_8086_app_style_scheme_theme_changed;

    g_object_class_install_property(object_class,
                                    PROP_SCHEME_THEME,
                                    g_param_spec_string("theme",
                                                        "Theme",
                                                        "The editor theme",
                                                        NULL,
                                                        G_PARAM_READWRITE));

    signals[EMU_THEME_CHANGED] = g_signal_new("theme_changed", G_TYPE_FROM_CLASS(klass),
                                              G_SIGNAL_RUN_FIRST,
                                              G_STRUCT_OFFSET(Emu8086AppStyleSchemeClass, theme_changed),
                                              NULL, NULL, NULL,
                                              G_TYPE_NONE, 0);
}

static void emu_8086_app_style_scheme_init(Emu8086AppStyleScheme *scheme)
{
 
 

    scheme->priv = emu_8086_app_style_scheme_get_instance_private(scheme);
    scheme->priv->color_comment = NULL;
    scheme->priv->color_keyword = NULL;
    scheme->priv->color_special = NULL;
    scheme->priv->color_string = NULL;
    scheme->priv->color_num = NULL;
    scheme->priv->color_reg = NULL;
    scheme->priv->color_highlight = NULL;
    scheme->priv->color_label_def = NULL;

    GSettings *settings;
    settings = g_settings_new("com.krc.emu8086app");
    g_settings_bind(settings, "theme", scheme, "theme", G_SETTINGS_BIND_GET);
    g_object_unref(settings);
}
static void
emu_8086_app_scheme_weak_notify(gpointer data,
                                GObject *where_the_scheme_was)
{
    g_print("destroyed\n");
}

static Emu8086AppStyleScheme *emu_8086_app_style_scheme_new()
{
    return g_object_new(EMU_8086_APP_STYLE_SCHEME_TYPE,
                        NULL);
};
Emu8086AppStyleScheme *emu_8086_app_style_scheme_get_default()
{
    static Emu8086AppStyleScheme *scheme = NULL;
    if (scheme != NULL)
        return scheme;
    scheme = emu_8086_app_style_scheme_new();
    g_object_add_weak_pointer(G_OBJECT(scheme),
                              (gpointer)&scheme);
    g_object_weak_ref(G_OBJECT(scheme),
                      emu_8086_app_scheme_weak_notify,
                      NULL);
    return scheme;
}

static gchar *emu_8086_app_style_scheme_get_col(Emu8086AppStyleScheme *scheme, gboolean ret, gint index, gchar *val)
{
    gchar **col;
    switch (index)
    {
    case 0:
        /* code */
        col = &scheme->priv->color_keyword;
        break;
    case 1:
        /* code */
        col = &scheme->priv->color_reg;
        break;
    case 2:
        /* code */
        col = &scheme->priv->color_string;
        break;
    case 3:
        /* code */
        col = &scheme->priv->color_label_def;
        break;
    case 4:
        /* code */
        col = &scheme->priv->color_num;
        break;
    case 5:
        /* code */
        col = &scheme->priv->color_special;
        break;
    case 6:
        /* code */
        col = &scheme->priv->color_comment;
        break;
    case 8:
        /* code */
        col = &scheme->priv->color_background;
        break;
    case 9:
        /* code */
        col = &scheme->priv->color_cursor;
        break;
    case 10:
        /* code */
        col = &scheme->priv->color_selection;
        break;
    case 11:
        /* code */
        col = &scheme->priv->color_selectionbg;
        break;
    case 12:
        /* code */
        col = &scheme->priv->color_text;
        break;
    case 13:
        /* code */
        col = &scheme->priv->color_linecolor;
        break;

    default:
        /* code */
        col = &scheme->priv->color_highlight;
        break;
    }
    if (ret)
    {
        return *col;
    }
    if (*col != NULL)
        g_free(*col);

    *col = g_strdup(val);
    return NULL;
}

static void emu_8086_app_style_scheme_load(Emu8086AppStyleScheme *scheme)
{
    PRIV_STYLE_SCHEME;
    if (priv->theme == NULL)
    {
       
       
        return;
    }
    FILE *fil;
    gchar *path;

    gchar *fname = g_strconcat(priv->theme, ".theme", NULL);

    gchar *keystr[] = {
        "keyword",
        "reg",
        "string",
        "label",
        "num",
        "special",
        "comment",

        "highlight",
        "background",
        "cursor",
        "selection",
        "selectionbg", "text",
        "linecolor",
        "#96CBFE",
        "#B5CAE8",
        "#CE9178",
        "#ebeb8d",
        "#B5CEA8",
        "#C586C0",
        "#6A9955",
        "rgba(56,56,56,1)",
        "#c4c4c4",
        "#ffffff",
        "#ffffff",
        "#001b33",
        "#fcfcfc",
        "#ffffff"};

    path = g_build_filename("/home/lion/Desktop/kosyWork/Desktop/gtkemu8086/themes", fname, NULL);
    g_print("%s \n", path);

    g_free(fname);
    g_autoptr(GError) error = NULL;
    g_autoptr(GKeyFile) key_file = g_key_file_new();
    if (!g_key_file_load_from_file(key_file, path, G_KEY_FILE_NONE, &error))
    {
        if (!g_error_matches(error, G_FILE_ERROR, G_FILE_ERROR_NOENT))
            g_warning("Error loading key file: %s", error->message);
        return;
    }
    int i = 0;
    for (i; i < 14; i++)
    {
        gchar *val = NULL;
        val = g_key_file_get_string(key_file, "Theme", keystr[i], &error);
        if (val == NULL &&
            !g_error_matches(error, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND))
        {
            g_warning("Error finding key in key file: %s", error->message);
            return;
        }

        else if (val == NULL)
        {
            // Fall back to a default value.
            val = g_strdup(keystr[i + 14]);
        }

        emu_8086_app_style_scheme_get_col(scheme, FALSE, i, val);
        g_free(val);
    }
 
    g_free(path);
    g_signal_emit(scheme, signals[EMU_THEME_CHANGED], 0);
    //   priv
}

gchar *emu_8086_app_style_scheme_get_color_by_index(Emu8086AppStyleScheme *scheme, const gint index)
{

    return emu_8086_app_style_scheme_get_col(scheme, TRUE, index, NULL);
}