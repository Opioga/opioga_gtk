/* gtkemu8086
 * Copyright 2020 KRC
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source errtextview must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * errtextview.h
 * ErrTreeView class
 */

#ifndef _EMU_ERR_TREE_VIEW_C
#define _EMU_ERR_TREE_VIEW_C
#include <gtk/gtk.h>
#include "emu8086apptypes.h"

G_BEGIN_DECLS
#define EMU8086_APP_ERR_TREE_VIEW_TYPE (emu8086_app_err_tree_view_get_type())
#define EMU8086_APP_ERR_TREE_VIEW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), EMU8086_APP_ERR_TREE_VIEW_TYPE, Emu8086AppErrTreeView))
#define EMU8086_APP_ERR_TREE_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), EMU8086_APP_ERR_TREE_VIEW_TYPE, Emu8086AppErrTreeViewClass))
#define EMU8086_IS_APP_ERR_TREE_VIEW(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), EMU8086_APP_ERR_TREE_VIEW_TYPE))
#define EMU8086_IS_APP_ERR_TREE_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), EMU8086_APP_ERR_TREE_VIEW_TYPE))
#define EMU8086_APP_ERR_TREE_VIEW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), EMU8086_APP_ERR_TREE_VIEW_TYPE, Emu8086AppErrTreeViewClass))
#define PRIV_ERR_TREE_VIEW Emu8086AppErrTreeViewPrivate *priv = tree_view->priv
#define EMU8086_APP_ERROR_TYPE (emu8086_error_type_get_type())
enum _Emu8086AppErrorType
{

    EMU8086_APP_ERROR_TYPE_WARNING,
    EMU8086_APP_ERROR_TYPE_FATAL
};


typedef struct _Emu8086AppErrTreeViewClass Emu8086AppErrTreeViewClass;
typedef struct _Emu8086AppErrTreeViewPrivate Emu8086AppErrTreeViewPrivate;


struct _Emu8086ErrorInfo
{gchar *icon_name;
GdkPixbuf  *icon;
    gchar *message;
    gchar *source;
    gpointer object;
    Emu8086ErrorInfo *next;
    gint line;
    gint index;
    Emu8086AppErrorType err_type;
};

struct _Emu8086AppErrTreeView
{
    GtkTreeView parent;

    Emu8086AppErrTreeViewPrivate *priv;
};

struct _Emu8086AppErrTreeViewClass
{
    GtkTreeViewClass parent_class;

    gpointer padding[2];
};

// G_DECLARE_FINAL_TYPE(Emu8086AppErrTreeView, emu8086_app_err_tree_view, EMU8086, APP_ERR_TREE_VIEW, GtkTreeView)
GType emu8086_app_err_tree_view_get_type(void) G_GNUC_CONST;
Emu8086AppErrTreeView *emu8086_app_err_tree_view_new(Emu8086AppWindow *win);
void emu8086_app_err_set_win(Emu8086AppErrTreeView *view, Emu8086AppWindow *win);
Emu8086ErrorInfo *emu8086_app_err_tree_view_new_error(Emu8086AppErrTreeView *view, gchar *message,
                                                      gchar *source,
                                                      gpointer object);
GType emu8086_error_type_get_type(void) G_GNUC_CONST;
void emu8086_app_err_tree_view_refresh(Emu8086AppErrTreeView *tree_view);
Emu8086ErrorInfo *emu8086_app_err_tree_view_new_error_full1(Emu8086AppErrTreeView *view, gchar *message,
                                                           gchar *source,
                                                           gpointer object, gchar *icon_name, Emu8086AppErrorType type);

Emu8086ErrorInfo *emu8086_app_err_tree_view_new_error_full2(Emu8086AppErrTreeView *view, gchar *message,
                                                           gchar *source,

                                                           gpointer object, GdkPixbuf  *icon, Emu8086AppErrorType type);


void *emu8086_app_err_tree_view_free_error(Emu8086AppErrTreeView *view, gchar *source);
G_END_DECLS
#endif // MACRO
