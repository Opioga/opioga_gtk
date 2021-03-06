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
 * treeview.c
 * TreeView class
 */

#include <emu8086appwin.h>
#include <emu8086apperrtreeview.h>
#include <emu8086appcode.h>
#include <emu8086appcodebuffer.h>
#include <emu8086apprunner.h>
#include <emu8086stylescheme.h>

enum _Emu8086AppErrTreeViewSignals
{
    ERRORS_CLEARED
};

enum _Emu8086AppErrTreeViewProperty
{
    PROP_0,
    PROP_WINDOW,
    PROP_ERR_COUNT

};

enum _Emu8086AppErrTreeViewCols
{
    SOURCE_COLUMN,
    MESSAGE_COLUMN,
    TYPE_COLUMN,
    OBJECT_COLUMN,
    ICON_COLUMN,
    ICON_NAME_COLUMN,
    LINE_NUMBER_COLUMN,

    NUM_COLUMNS
};

typedef enum _Emu8086AppErrTreeViewProperty Emu8086AppErrTreeViewProperty;

struct _Emu8086AppErrTreeViewPrivate
{
    gboolean has_errors;
    Emu8086AppWindow *win;

    GtkTreeStore *tree_store;
    Emu8086AppStyleScheme *scheme;
    gint err_count;
    GHashTable *errors;
    gchar *text_color;
};

G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppErrTreeView, emu8086_app_err_tree_view, GTK_TYPE_TREE_VIEW);

static void tree_selection_changed_cb(GtkTreeSelection *selection, gpointer data);

static void emu8086_app_err_tree_view_set_property(GObject *object,
                                                   guint property_id,
                                                   const GValue *value,
                                                   GParamSpec *pspec);

static void emu8086_app_err_tree_view_get_property(GObject *object,
                                                   guint property_id,
                                                   GValue *value,
                                                   GParamSpec *pspec);
static void emu8086_app_err_tree_view_dispose(GObject *object);
static void
renderer_text_description_cell_data_func(GtkTreeViewColumn *column,
                                         GtkCellRenderer *renderer,
                                         GtkTreeModel *model,
                                         GtkTreeIter *iter,
                                         gpointer data);

static void
renderer_pb_description_cell_data_func(GtkTreeViewColumn *column,
                                       GtkCellRenderer *renderer,
                                       GtkTreeModel *model,
                                       GtkTreeIter *iter,
                                       gpointer data);
static void for_each(gpointer key,
                     gpointer value,
                     gpointer user_data);

 
static void emu8086_app_err_tree_view_class_init(Emu8086AppErrTreeViewClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->get_property = emu8086_app_err_tree_view_get_property;
    object_class->set_property = emu8086_app_err_tree_view_set_property;
    // object_class->dispose = emu8086_app_err_tree_view_dispose;
    g_object_class_install_property(object_class, PROP_WINDOW,
                                    g_param_spec_object("window",
                                                        "Window",
                                                        "The tree view's window",
                                                        EMU8086_APP_WINDOW_TYPE,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
    g_object_class_install_property(object_class, PROP_ERR_COUNT,
                                    g_param_spec_int("err_count", "ErrCount",
                                                     "The number of errors", 0, 1000, CODE_VIEW, G_PARAM_READABLE));
}

static void emu8086_app_err_tree_view_init(Emu8086AppErrTreeView *tree_view)
{

    GtkTreeViewColumn *column;
    GtkTreeView *_tree_view;
    GtkCellRenderer *renderer_text;
    GtkCellRenderer *renderer_pb;
    tree_view->priv = emu8086_app_err_tree_view_get_instance_private(tree_view);
    PRIV_ERR_TREE_VIEW;
    priv->err_count = 0;
    priv->errors = g_hash_table_new(g_direct_hash, NULL);

    _tree_view = GTK_TREE_VIEW(tree_view);
    priv->tree_store = gtk_tree_store_new(NUM_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, EMU8086_APP_ERROR_TYPE, G_TYPE_POINTER,
                                          G_TYPE_ICON, G_TYPE_STRING, G_TYPE_INT);

    gtk_tree_view_set_model(_tree_view, GTK_TREE_MODEL(priv->tree_store));
    renderer_text = gtk_cell_renderer_text_new();
    renderer_pb = gtk_cell_renderer_pixbuf_new();
    column = gtk_tree_view_column_new();

    gtk_tree_view_column_pack_end(column, renderer_text, TRUE);
    gtk_tree_view_column_pack_start(column, renderer_pb, FALSE);
    gtk_tree_view_column_set_cell_data_func(column,
                                            renderer_text,
                                            renderer_text_description_cell_data_func,
                                            tree_view,
                                            NULL);

    gtk_tree_view_column_set_cell_data_func(column,
                                            renderer_pb,
                                            renderer_pb_description_cell_data_func,
                                            tree_view,
                                            NULL);

    gtk_tree_view_column_set_spacing(column, 3);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view),
                                column);

    GtkTreeSelection *select;

    select = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree_view));
    gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
    g_signal_connect(G_OBJECT(select), "changed",
                     G_CALLBACK(tree_selection_changed_cb),
                     NULL);

    gtk_widget_set_vexpand(GTK_WIDGET(tree_view), TRUE);
    gtk_widget_set_name(tree_view, "err-tree-view");


}

static void emu8086_app_err_tree_view_get_property(GObject *object,
                                                   guint property_id,
                                                   GValue *value,
                                                   GParamSpec *pspec)

{
    Emu8086AppErrTreeView *self = EMU8086_APP_ERR_TREE_VIEW(object);
    switch ((Emu8086AppErrTreeViewProperty)property_id)
    {
    case PROP_WINDOW:
        g_value_set_object(value, self->priv->win);
        break;
    case PROP_ERR_COUNT:
        g_value_set_int(value, self->priv->err_count);
        break;
    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void emu8086_app_err_tree_view_set_property(GObject *object,
                                                   guint property_id,
                                                   const GValue *value,
                                                   GParamSpec *pspec)
{
    Emu8086AppErrTreeView *self = EMU8086_APP_ERR_TREE_VIEW(object);
    switch ((Emu8086AppErrTreeViewProperty)property_id)
    {
    case PROP_WINDOW:
        self->priv->win = EMU8086_APP_WINDOW(g_value_get_object(value));
        break;

    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
renderer_text_description_cell_data_func(GtkTreeViewColumn *column,
                                         GtkCellRenderer *renderer,
                                         GtkTreeModel *model,
                                         GtkTreeIter *iter,
                                         gpointer data)
{
    gchar *src;
    gchar *msg;
    gchar *text;

    gtk_tree_model_get(model, iter,
                       SOURCE_COLUMN, &src,

                       MESSAGE_COLUMN, &msg,

                       -1);

    if (msg != NULL)
    {
        text = msg;
    }
    else
    {
        text = src;
    }

    g_object_set(G_OBJECT(renderer),
                 "text",
                 text,
                 NULL);

    g_free(src);
    if (msg != NULL)
        g_free(msg);
}

static void
renderer_pb_description_cell_data_func(GtkTreeViewColumn *column,
                                       GtkCellRenderer *renderer,
                                       GtkTreeModel *model,
                                       GtkTreeIter *iter,
                                       gpointer data)
{
    GdkPixbuf *icon;
    gchar *icon_name;
    gtk_tree_model_get(model, iter,
                       ICON_COLUMN, &icon,

                       ICON_NAME_COLUMN, &icon_name,

                       -1);
    if (icon)
    {
        g_object_set(G_OBJECT(renderer),
                     "pixbuf",
                     icon,
                     NULL);
        g_object_unref(icon);
    }
    else if (icon_name)
    {
        g_object_set(G_OBJECT(renderer),
                     "icon-name",
                     icon_name,
                     NULL);
        g_free(icon_name);
    }
    else
    {
        g_object_set(G_OBJECT(renderer),
                     "pixbuf",
                     NULL,
                     NULL);
        //g_object_unref(icon);
    }
}

static void for_each(gpointer key,
                     gpointer value,
                     gpointer user_data)

{
    Emu8086AppErrTreeView *tree_view;
    Emu8086ErrorInfo *err;
    GArray *array;
    tree_view = EMU8086_APP_ERR_TREE_VIEW(user_data);
    array = value;
    gint i = 0;
    GtkTreeIter iter1; /* Parent iter */
    GtkTreeIter iter2; /* Child iter  */
    GtkTreeStore *store = tree_view->priv->tree_store;
    gtk_tree_store_append(store, &iter1, NULL);
    while (err = g_array_index(array, Emu8086ErrorInfo *, i))
    {
        i++;

        g_print("src-> %s \n", err->source);
        g_print("err-> %x \n", err->object);
        if (err->message)
        {
            gtk_tree_store_append(store, &iter2, &iter1);
            gtk_tree_store_set(store, &iter2,
                               SOURCE_COLUMN, err->source,
                               MESSAGE_COLUMN, err->message,
                               TYPE_COLUMN, err->err_type,
                               OBJECT_COLUMN, err->object,
                               ICON_COLUMN, err->icon,
                               ICON_NAME_COLUMN, err->icon_name,
                               LINE_NUMBER_COLUMN, err->line,

                               -1);
        }
        else
        {
            gtk_tree_store_set(store, &iter1,
                               SOURCE_COLUMN, err->source,
                               MESSAGE_COLUMN, NULL,
                               TYPE_COLUMN, err->err_type,
                               OBJECT_COLUMN, err->object,
                               ICON_COLUMN, err->icon,
                               ICON_NAME_COLUMN, err->icon_name,
 LINE_NUMBER_COLUMN, err->line,
                               -1);
        }
    };
}

static void tree_selection_changed_cb(GtkTreeSelection *selection, gpointer data)
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    gpointer sel_obj;
    gint line;
    if (gtk_tree_selection_get_selected(selection, &model, &iter))
    {
        gtk_tree_model_get(model, &iter,
                           OBJECT_COLUMN, &sel_obj,
                           LINE_NUMBER_COLUMN, &line,


                           -1);
                           if(EMU8086_IS_APP_CODE(sel_obj)){
                               emu8086_app_code_mgo_to_line(EMU8086_APP_CODE(sel_obj), line);
                           }
        g_print("<Object %x> %d \n", sel_obj,  line);
    }
};

Emu8086ErrorInfo *emu8086_app_err_tree_view_new_error(Emu8086AppErrTreeView *view, gchar *message,
                                                      gchar *source,
                                                      gpointer object)
{
    Emu8086ErrorInfo *new_err;
    new_err = g_slice_new0(Emu8086ErrorInfo);
    new_err->index = view->priv->err_count++;
    new_err->source = g_strdup(source);
    new_err->err_type = EMU8086_APP_ERROR_TYPE_WARNING;
    new_err->object = object;
    new_err->icon_name = NULL;
    new_err->icon = NULL;
    new_err->line = 0;
    GArray *array = (g_hash_table_lookup(view->priv->errors, source));
    if (array == NULL)
    {
        array = g_array_new(TRUE, FALSE, sizeof(Emu8086ErrorInfo *));
        //Emu8086ErrorInfo *new_err_header;

        new_err->message = NULL;

        new_err->object = object;
        //   g_array_append_val(array, new_err_header);

        g_hash_table_insert(view->priv->errors, source, array);
    }
    else
    {
        new_err->message = g_strdup(message);
    }

    g_array_append_val(array, new_err);

    return new_err;
}

void *emu8086_app_err_tree_view_free_error(Emu8086AppErrTreeView *view, gchar *source)
{
    g_return_if_fail(EMU8086_IS_APP_ERR_TREE_VIEW(view));

    if (!g_hash_table_contains(view->priv->errors, source))
        return;

    GArray *array = (g_hash_table_lookup(view->priv->errors, source));
    if (!array)
        return;
    Emu8086ErrorInfo *err;
    gint i = 0;
    gtk_tree_store_clear(view->priv->tree_store);
    while (err = g_array_index(array, Emu8086ErrorInfo *, i))
    {
        i++;
        g_free(err->source);
        if (err->message)
            g_free(err->message);
        if (err->icon_name)
            g_free(err->icon_name);
        if (err->icon)
            g_object_unref(err->icon);
        g_slice_free(Emu8086ErrorInfo, err);
        /* code */
    }
    g_array_free(array, TRUE);
    g_hash_table_remove(view->priv->errors, source);
    emu8086_app_err_tree_view_refresh(view);
}

GType emu8086_error_type_get_type(void)
{
    static GType the_type = 0;
    if (the_type == 0)
    {
        static const GEnumValue values[] = {
            {EMU8086_APP_ERROR_TYPE_WARNING, "EMU8086_APP_ERROR_TYPE_WARNING", "warning"},
            {EMU8086_APP_ERROR_TYPE_FATAL, "EMU8086_APP_ERROR_TYPE_FATAL", "fatal"},
            {0, NULL, NULL},
        };

        the_type = g_enum_register_static(
            g_intern_static_string("Emu8086AppErrorType"),
            values);
    }

    return the_type;
}

Emu8086AppErrTreeView *emu8086_app_err_tree_view_new(Emu8086AppWindow *win)
{
    return g_object_new(EMU8086_APP_ERR_TREE_VIEW_TYPE, "window", win, NULL);
};

void emu8086_app_err_tree_view_refresh(Emu8086AppErrTreeView *tree_view)
{
    g_return_if_fail(EMU8086_IS_APP_ERR_TREE_VIEW(tree_view));
    PRIV_ERR_TREE_VIEW;
    gtk_tree_store_clear(priv->tree_store);
    g_hash_table_foreach(priv->errors, for_each, tree_view);
}

Emu8086ErrorInfo *emu8086_app_err_tree_view_new_error_full1(Emu8086AppErrTreeView *view, gchar *message,
                                                            gchar *source,
                                                            gpointer object, gchar *icon_name, Emu8086AppErrorType type)
{
    Emu8086ErrorInfo *new_err;
    new_err = emu8086_app_err_tree_view_new_error(view, message, source, object);
    new_err->err_type = type;
    if (icon_name)
        new_err->icon_name = g_strdup(icon_name);
    return new_err;
}

Emu8086ErrorInfo *emu8086_app_err_tree_view_new_error_full2(Emu8086AppErrTreeView *view, gchar *message,
                                                            gchar *source,
                                                            gpointer object, GdkPixbuf *icon, Emu8086AppErrorType type)
{
    Emu8086ErrorInfo *new_err;
    new_err = emu8086_app_err_tree_view_new_error(view, message, source, object);
    new_err->err_type = type;
    new_err->icon = icon;
    return new_err;
}