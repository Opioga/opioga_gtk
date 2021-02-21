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
 * emu8086_app_runner.h
 * Runner class
 */

#ifndef _EMU_PLUGIN_BOX_C
#define _EMU_PLUGIN_BOX_C
#include <gtk/gtk.h>
#include "emu8086apptypes.h"



G_BEGIN_DECLS
struct emu8086;
#define PRIV_BOX Emu8086AppPluginBoxPrivate *priv = box->priv

#define EMU8086_APP_PLUGIN_BOX_TYPE (emu8086_app_plugin_box_get_type())
#define EMU8086_PLUGIN_BOX_IS_RUNNER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), EMU8086_APP_PLUGIN_BOX_TYPE))


#define EMU8086_APP_PLUGIN_BOX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), EMU8086_APP_PLUGIN_BOX_TYPE, Emu8086AppPluginBox))
#define EMU8086_APP_PLUGIN_BOX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), EMU8086_APP_PLUGIN_BOX_TYPE, Emu8086AppPluginBoxClass))
#define EMU8086_PLUGIN_BOX_IS_RUNNER_CLASS(klass)(G_TYPE_CHECK_CLASS_TYPE((klass), EMU8086_APP_PLUGIN_BOX_TYPE)

typedef struct _Emu8086AppPluginBoxPrivate Emu8086AppPluginBoxPrivate;

struct _Emu8086AppPluginBox
{
    GtkGrid parent;
    Emu8086AppPluginBoxPrivate *priv;
};

typedef struct _Emu8086AppPluginBoxClass Emu8086AppPluginBoxClass;

struct _Emu8086AppPluginBoxClass
{
    GtkGridClass parent_class;
    GtkWidget *(*get_stack)(Emu8086AppPluginBox *box);
};

typedef enum
{
    DUMMY,
    PROP_WIN,
    PROP_MY_RUNNER,
    PROP_VBOX

} Emu8086AppPluginBoxProperty;

GType emu8086_app_plugin_box_get_type(void) G_GNUC_CONST;
typedef struct _Emu8086AppCodeRunner Emu8086AppCodeRunner;
Emu8086AppPluginBox *emu8086_app_plugin_box_new(GtkApplicationWindow *win,
                                                 Emu8086AppCodeRunner *runner);

void emu8086_app_plugin_box_start_plugins(Emu8086AppPluginBox *box);
Emu8086AppWindow * emu8086_app_plugin_box_get_mwindow(Emu8086AppPluginBox *box);
void emu8086_app_plugin_box_set_window(Emu8086AppPluginBox *box, Emu8086AppWindow *win);
G_END_DECLS
#endif