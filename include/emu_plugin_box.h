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
 * emu_8086_app_runner.h
 * Runner class
 */

#ifndef _EMU_PLUGIN_BOX_C
#define _EMU_PLUGIN_BOX_C
#include <gtk/gtk.h>
#include <emu8086win.h>
#include <emu_8086_app_runner.h>


G_BEGIN_DECLS
struct emu8086;
#define PRIV_BOX Emu8086AppPluginBoxPrivate *priv = box->priv

#define EMU_8086_APP_PLUGIN_BOX_TYPE (emu_8086_app_plugin_box_get_type())
#define EMU_8086_PLUGIN_BOX_IS_RUNNER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), EMU_8086_APP_PLUGIN_BOX_TYPE))
#define PRIV_PLUGIN_BOX Emu8086AppPluginBoxPrivate *priv = runner->priv
#define EMU_8086_APP_PLUGIN_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), EMU_8086_APP_PLUGIN_BOX_TYPE, Emu8086AppPluginBox))
#define EMU_8086_APP_PLUGIN_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), EMU_8086_APP_PLUGIN_BOX_TYPE, Emu8086AppPluginBoxClass))
#define EMU_8086_PLUGIN_BOX_IS_RUNNER_CLASS(klass)(G_TYPE_CHECK_CLASS_TYPE((klass), EMU_8086_APP_PLUGIN_BOX_TYPE)

typedef struct _Emu8086AppPluginBoxPrivate Emu8086AppPluginBoxPrivate;

typedef struct _Emu8086AppPluginBox Emu8086AppPluginBox;
struct _Emu8086AppPluginBox
{
    GtkBox parent;
    Emu8086AppPluginBoxPrivate *priv;
};

typedef struct _Emu8086AppPluginBoxClass Emu8086AppPluginBoxClass;

struct _Emu8086AppPluginBoxClass
{
    GtkBoxClass parent_class;
    GtkWidget *(*get_stack)(Emu8086AppPluginBox *box);
};

typedef enum
{
    DUMMY,
    PROP_WIN,
    PROP_MY_RUNNER,
    PROP_VBOX

} Emu8086AppPluginBoxProperty;

GType emu_8086_app_plugin_box_get_type(void) G_GNUC_CONST;

Emu8086AppPluginBox *emu_8086_app_plugin_box_new(GtkApplicationWindow *win,
                                                 Emu8086AppCodeRunner *runner);

GtkWidget *emu8086_app_window_get_stack(Emu8086AppPluginBox *box);

G_END_DECLS
#endif