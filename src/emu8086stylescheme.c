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
 * emu8086stylescheme.c
 * App class
 */


#include <stdio.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#else
#define DATADIR "/home/lion/Desktop/kosyWork/Desktop/gtkemu8086"
#endif

#include <glib.h>

#include <emu8086app.h>

#include <emu8086stylescheme.h>

G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppStyleScheme, emu8086_app_style_scheme, G_TYPE_OBJECT);

enum Emu8086StyleSchemeSignals
{
    DUMMY_E_S,
    EMU_THEME_CHANGED,
    // PORT_STATE_CHANGED,
    LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = {0};
static void emu8086_app_style_scheme_init(Emu8086AppStyleScheme *scheme);
static void emu8086_app_style_scheme_load(Emu8086AppStyleScheme *scheme);
static void emu8086_app_style_scheme_class_init(Emu8086AppStyleSchemeClass *klass);

static void emu8086_app_style_scheme_set_property(GObject *object,
                                                   guint property_id,
                                                   const GValue *value,
                                                   GParamSpec *pspec)
{
    Emu8086AppStyleScheme *self = EMU8086_APP_STYLE_SCHEME(object);
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
        emu8086_app_style_scheme_load(self);

        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
emu8086_app_style_scheme_get_property(GObject *object,
                                       guint property_id,
                                       GValue *value,
                                       GParamSpec *pspec)
{
    Emu8086AppStyleScheme *self = EMU8086_APP_STYLE_SCHEME(object);

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
static void emu8086_app_style_scheme_theme_changed(Emu8086AppStyleScheme *scheme)
{
    return;
}

static void emu8086_app_style_scheme_class_init(Emu8086AppStyleSchemeClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->set_property = emu8086_app_style_scheme_set_property;
    object_class->get_property = emu8086_app_style_scheme_get_property;
    klass->theme_changed = emu8086_app_style_scheme_theme_changed;

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

static void emu8086_app_style_scheme_init(Emu8086AppStyleScheme *scheme)
{

    scheme->priv = emu8086_app_style_scheme_get_instance_private(scheme);
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
emu8086_app_scheme_weak_notify(gpointer data,
                                GObject *where_the_scheme_was)
{
    g_print("destroyed\n");
}

static Emu8086AppStyleScheme *emu8086_app_style_scheme_new()
{
    return g_object_new(EMU8086_APP_STYLE_SCHEME_TYPE,
                        NULL);
};
Emu8086AppStyleScheme *emu8086_app_style_scheme_get_default()
{
    static Emu8086AppStyleScheme *scheme = NULL;
    if (scheme != NULL)
        return scheme;
    scheme = emu8086_app_style_scheme_new();
    g_object_add_weak_pointer(G_OBJECT(scheme),
                              (gpointer)&scheme);
    g_object_weak_ref(G_OBJECT(scheme),
                      emu8086_app_scheme_weak_notify,
                      NULL);
    return scheme;
}

static gchar *emu8086_app_style_scheme_get_col(Emu8086AppStyleScheme *scheme, gboolean ret, gint index, gchar *val)
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

static void emu8086_app_style_scheme_load(Emu8086AppStyleScheme *scheme)
{
    PRIV_STYLE_SCHEME;
    if (priv->theme == NULL)
    {

        return;
    }
    FILE *fil;
    gchar *path;

    gchar *fname = g_strconcat(priv->theme, ".theme", NULL);
    gboolean usefallbacks = FALSE;
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

        "#96CBFE",               // 0
        "#B5CAE8",               // 1
        "#CE9178",               // 2
        "#ebeb8d",               // 3
        "#B5CEA8",               // 4
        "#C586C0",               // 5
        "#6A9955",               // 6
        "rgba(100,100,100,0.5)", // 7
        "rgba(56,56,56,1)",      // 8
        "#ffffff",
        "#ffffff",
        "#001b33",
        "#fcfcfc",
        "#ffffff"};

    path = g_build_filename(DATADIR, "emu8086/themes", fname, NULL);
    if (!file_exists(path))
    {
        g_free(path);
        path = NULL;
        path = g_build_filename(g_get_user_config_dir(), "emu8086/themes", fname, NULL);
        if (!file_exists(path))
        {

            g_print("not exist %s \n", path);
            g_free(path);
            path = g_build_filename(DATADIR, "emu8086/themes", "dark+.theme", NULL);
            usefallbacks = (!file_exists(path));
        }
    }
    g_print("%s \n", path);

    g_free(fname);
    if (usefallbacks)
    {
        int i = 0;
        for (i; i < 14; i++)
        {
            gchar *val = NULL;
            val = keystr[i + 14];

            emu8086_app_style_scheme_get_col(scheme, FALSE, i, val);
        }
    }

    else
    {
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

            emu8086_app_style_scheme_get_col(scheme, FALSE, i, val);
            g_free(val);
        }
    }
    g_free(path);

    g_signal_emit(scheme, signals[EMU_THEME_CHANGED], 0);
    //   priv
}

gchar *emu8086_app_style_scheme_get_color_by_index(Emu8086AppStyleScheme *scheme, const gint index)
{

    return emu8086_app_style_scheme_get_col(scheme, TRUE, index, NULL);
}

gboolean file_exists(const gchar *path)
{
    GFile *file;
    GError *error = NULL, *error2 = NULL;
    file = g_file_new_for_path(path);

    if (!g_file_query_exists(file, NULL))
    {

        return FALSE;
    }

    g_object_unref(file);

    return TRUE;
}

gboolean user_config_dir_exists()
{
    // g_file_make_directory
    GFile *file;
    GError *error = NULL, *error2 = NULL;
    gchar *path;
    gboolean ret;
    path = g_build_filename(g_get_user_config_dir(), "emu8086", NULL);
    if (file_exists(path))
    {
        g_free(path);
        return TRUE;
    }

    file = g_file_new_for_path(path);

    ret = g_file_make_directory(file, NULL, &error);
    if (error != NULL)
    {
        g_print("%s\n", error->message);
        g_error_free(error);
    }
    g_object_unref(file);
    g_free(path);
    return ret;
}

static gboolean emu_create_config_dir()
{
    GFile *file;
    GError *error = NULL;
    gchar *path;
    gboolean ret;
    user_config_dir_exists();
    path = g_build_filename(g_get_user_config_dir(), "emu8086/themes", NULL);
    file = g_file_new_for_path(path);
    ret = g_file_make_directory(file, NULL, &error);
    if (error != NULL)
    {
        g_print("%s\n", error->message);
        g_error_free(error);
    }
    g_object_unref(file);
    g_free(path);
    return ret;
}

gboolean user_config_themes_exists()
{
    GFile *file;
    GError *error = NULL, *error2 = NULL;
    gchar *path;
    path = g_build_filename(g_get_user_config_dir(), "emu8086/themes", NULL);
    file = g_file_new_for_path(path);
    if (!g_file_query_exists(file, NULL))
    {

        if (emu_create_config_dir())
        {
          g_file_make_directory(file, NULL, &error2);
            if (error2 != NULL)
            {
                g_error_free(error2);
                g_print("hhh %s\n", error2->message);
                return FALSE;
            }
        }
        else
        {
            g_print("failes \n");
            return FALSE;
        }
    }

    g_object_unref(file);
    g_free(path);

    return TRUE;
}

them *emu8086_app_style_scheme_get_theme( gchar *_name,gchar *path ){
       gchar *theme_path, *name;
     
       g_autoptr(GError) error4 = NULL;
    g_autoptr(GKeyFile) key_file = g_key_file_new();

    theme_path = g_build_filename(path, _name, NULL);
    

    if (!g_key_file_load_from_file(key_file, theme_path, G_KEY_FILE_NONE, &error4))
    {
        if (g_error_matches(error4, G_FILE_ERROR, G_FILE_ERROR_NOENT))
            g_print("Error loading key file: %s '%s'", error4->message,theme_path);
        return NULL;
    }
    gchar *des = g_key_file_get_string(key_file, "Description", "value",  &error4);
 
 
        if (des == NULL ||
        g_error_matches(error4, G_KEY_FILE_ERROR, G_KEY_FILE_ERROR_KEY_NOT_FOUND))
    {
        g_print("Error finding key in key file: %s", error4->message);
              return NULL;
    }
    _name[strlen(_name) - 6] = '\0';
 
    name = _name;
    them *theme = (them *)malloc(sizeof(them));
            theme->id = g_strdup(name);
        theme->text = des;
        g_free(theme_path);
        return theme;
}



them **emu8086_app_style_scheme_get_themes(Emu8086AppStyleScheme *scheme, gsize *len)
{

    gchar *path2;
  GDir *dir;

    path2 = g_build_filename(DATADIR, "emu8086/themes",  NULL);
  
    dir = g_dir_open (path2, 0, NULL);
    const gchar *_name;
    gsize size, l = 0;
    GSList      *filenames;
GSList *f;
filenames = g_slist_prepend (filenames, NULL);
    while ((_name = g_dir_read_name (dir)) != NULL)
	{
        gchar *name = g_strdup(_name);
       gchar *m = NULL;
       m= strstr(name, ".theme");
if(m != NULL) {
    filenames = g_slist_prepend (filenames, name);
    l= l+1;
    
    }
    else {
        g_free (name);
    }

    }

them **themes;
themes = (them **)malloc(sizeof(them *) * l);
 int o =0;
    for (f = filenames; f != NULL; f = f->next){
        gchar *filename;

		filename = f->data;

        them *_theme;
        _theme = emu8086_app_style_scheme_get_theme(filename, path2);
        if(_theme != NULL){ g_print("name -> %s Des -> %s\n", _theme->id, _theme->text);
        themes[o++] = _theme;}

    }


 
 



    g_free(path2);
   
   
    size =o;
g_slist_free_full (filenames, g_free);
    themes = add_local_themes(scheme, themes, size, len);

    return themes;
  
  
}

void emu8086_theme_free(them *themes)
{

    g_free(themes->id);
    g_free(themes->text);
    free(themes);
}

gboolean check_is_empty(GFile *file, GError *error)
{

    GFileInfo *info;
    info = g_file_query_info(file, "standard::*", G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS,
                             NULL, &error);
    if (error != NULL)
        return FALSE;
    goffset size = g_file_info_get_size(info);

    g_object_unref(info);
    return size == 0;
}

gchar *get_n(GFile *file)
{
    gchar *fname = g_file_get_basename(file);
    gint mylen = strlen(fname) - 6;
    gchar name[strlen(fname) - 5];
    strncpy(name, fname, mylen);
    g_free(fname);
    return g_strdup(name);
}

static gboolean emu_style_scheme_install_theme_to_dir(Emu8086AppStyleScheme *scheme,
                                                      const GFile *file, const gchar *dirname)
{
    GError *error2;
    error2 = NULL;
    gchar *path;
    gchar *fname;
    fname = g_file_get_basename(file);
    path = g_build_filename(dirname, fname, NULL);
    if (file_exists(path))
    {
        g_free(path);
        g_free(fname);
        return FALSE;
    }
    GFile *dest = g_file_new_for_path(path);

    g_file_copy(file, dest, G_FILE_COPY_OVERWRITE | G_FILE_COPY_NOFOLLOW_SYMLINKS,
                NULL, NULL, NULL, &error2);
    g_free(path);
    g_free(fname);
    g_object_unref(dest);
    if (error2 != NULL)
    {
        g_print(error2->message);
        g_error_free(error2);
        return FALSE;
    }

    return TRUE;
}

them **add_local_themes(Emu8086AppStyleScheme *scheme, them **themes, gsize size, gsize *len)
{

    gchar *new_file_name = NULL;
    gchar *dirname;
    gchar *styles_dir;
    gchar *hn = NULL;
    GError *error;
    GError *error2;
    error = error2 = NULL;
    gboolean isEmpty = FALSE;
    GFile *theme_file;
    gchar *path;
    *len = size;
    if (!user_config_themes_exists())
        return themes;
 path = g_build_filename(g_get_user_config_dir(), "emu8086/themes", NULL);
      GDir *dir;
          const gchar *_name;


    GSList      *filenames;
GSList *f;
gsize l = 0;
filenames = g_slist_prepend (filenames, NULL);
    dir = g_dir_open (path, 0, NULL);

    while ((_name = g_dir_read_name (dir)) != NULL)
	{
        gchar *name = g_strdup(_name);
       gchar *m = NULL;
       m= strstr(name, ".theme");
if(m != NULL) {
    filenames = g_slist_prepend (filenames, name);
    l= l+1;
    
    }
    else {
        g_free (name);
    }

    }
    if(l == 0) return themes;
    else {
        g_print("%d \n ", l);
    }
    gsize expanded = sizeof(them *) * (size + l);
    themes = g_realloc(themes, expanded);

    int o =0;
    for (f = filenames; f != NULL; f = f->next){
        gchar *filename;

		filename = f->data;

        them *_theme;
        _theme = emu8086_app_style_scheme_get_theme(filename, path);
        if(_theme != NULL){ g_print("name -> %s Des -> %s\n", _theme->id, _theme->text);
        themes[ o+ size] = _theme;}
o = o + 1;
    }

g_slist_free_full (filenames, g_free);
    *len = size + l;
    return themes;
}

const gchar *emu_style_scheme_install_theme(Emu8086AppStyleScheme *scheme,
                                            const GFile *file)
{
    gchar *new_file_name = NULL;
    gchar *dirname;
    gchar *styles_dir;
    gchar *hn = NULL;
    GError *error;
    GError *error2;
    error = error2 = NULL;
    gboolean copied = FALSE;
    GFile *theme_file;
    gchar *path;

    g_autoptr(GKeyFile) keyfile = g_key_file_new();
    if (!user_config_themes_exists())
        return NULL;
    dirname = g_build_filename(g_get_user_config_dir(), "emu8086", "themes", NULL);
    if (emu_style_scheme_install_theme_to_dir(scheme, file, dirname))
    {
        g_free(dirname);
    }
    else
    {
        g_free(dirname);
        g_print("exists");
        return NULL;
    }

 
 
  
    return get_n(file);
}