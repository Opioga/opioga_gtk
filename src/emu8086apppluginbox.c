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
#include <emu8086apprunner.h>
#include <emu8086appwin.h>

#include <emu8086apppluginbox.h>
#include <emu8086apppluginsengine.h>
#include <libpeas/peas-activatable.h>
#include <libpeas/peas-extension-set.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

struct _Emu8086AppPluginBoxPrivate
{
    Emu8086AppWindow *win;
    PeasExtensionSet *extensions;
    Emu8086AppCodeRunner *runner;
    GtkBox *v_box;
};
G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppPluginBox, emu8086_app_plugin_box, GTK_TYPE_GRID);

static void emu8086_app_plugin_box_init(Emu8086AppPluginBox *runner);

static void emu8086_app_plugin_box_class_init(Emu8086AppPluginBoxClass *klass);
static void
on_extension_removed(PeasExtensionSet *extensions,
                     PeasPluginInfo *info,
                     PeasExtension *exten,
                     Emu8086AppPluginBox *window);

static void
on_extension_added(PeasExtensionSet *extensions,
                   PeasPluginInfo *info,
                   PeasExtension *exten,
                   Emu8086AppPluginBox *window);

static void emu8086_app_plugin_box_set_property(GObject *object,
                                                guint property_id,
                                                const GValue *value,
                                                GParamSpec *pspec);
static void
emu8086_app_plugin_box_get_property(GObject *object,
                                    guint property_id,
                                    GValue *value,
                                    GParamSpec *pspec);

static void emu8086_app_plugin_box_set_property(GObject *object,
                                                guint property_id,
                                                const GValue *value,
                                                GParamSpec *pspec)
{
    Emu8086AppPluginBox *self = EMU8086_APP_PLUGIN_BOX(object);
 
    switch ((Emu8086AppPluginBoxProperty)property_id)
    {

    case PROP_WIN:

        self->priv->win = EMU8086_APP_WINDOW(g_value_get_object(value));
        break;
    case PROP_MY_RUNNER:

        self->priv->runner = EMU8086_APP_CODE_RUNNER(g_value_get_object(value));
        break;
    case PROP_VBOX:
        break;
    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
emu8086_app_plugin_box_get_property(GObject *object,
                                    guint property_id,
                                    GValue *value,
                                    GParamSpec *pspec)
{
    Emu8086AppPluginBox *self = EMU8086_APP_PLUGIN_BOX(object);

    switch ((Emu8086AppPluginBoxProperty)property_id)
    {
    case PROP_WIN:
        g_value_set_object(value, self->priv->win);
        break;

    case PROP_MY_RUNNER:
        g_value_set_object(value, self->priv->runner);
        break;

    case PROP_VBOX:
        g_value_set_object(value, self);
        break;
    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

Emu8086AppPluginBox *emu8086_app_plugin_box_new(GtkApplicationWindow *win, Emu8086AppCodeRunner *runner)
{
    return g_object_new(EMU8086_APP_PLUGIN_BOX_TYPE,
                        "window", win,
                        "runner", runner,
                        "row-spacing", 30,
                        NULL);
};

static void emu8086_app_plugin_box_class_init(Emu8086AppPluginBoxClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->set_property = emu8086_app_plugin_box_set_property;
    object_class->get_property = emu8086_app_plugin_box_get_property;
    // klass->get_stack = emu8086_app_window_get_stack;
    // PROP_MY_RUNNER;

    g_object_class_install_property(object_class,
                                    PROP_MY_RUNNER,
                                    g_param_spec_object("runner",
                                                        "Runner",
                                                        "",

                                                        EMU8086_APP_CODE_RUNNER_TYPE,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));

    g_object_class_install_property(object_class,
                                    PROP_WIN,
                                    g_param_spec_object("window",
                                                        "Window",
                                                        "",

                                                        EMU8086_APP_WINDOW_TYPE,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

static void emu8086_app_plugin_box_init(Emu8086AppPluginBox *box)
{
    box->priv = emu8086_app_plugin_box_get_instance_private(box);
   
}

static void
on_extension_removed(PeasExtensionSet *extensions,
                     PeasPluginInfo *info,
                     PeasExtension *exten,
                     Emu8086AppPluginBox *window)
{
    peas_extension_call(exten, "deactivate", window);
}

static void
on_extension_added(PeasExtensionSet *extensions,
                   PeasPluginInfo *info,
                   PeasExtension *exten,
                   Emu8086AppPluginBox *box)
{
    PRIV_BOX;
    gtk_widget_show_all(box);
    //  g_print("0x%x", priv->runner);
    peas_activatable_activate(PEAS_ACTIVATABLE(exten));
    // peas_extension_call (exten, "kon",box);
    // peas_
    // peas_extension_set_call_valist
}

void emu8086_app_plugin_box_start_plugins(Emu8086AppPluginBox *box)
{
     PRIV_BOX;

    priv->extensions = peas_extension_set_new(PEAS_ENGINE(emu8086_plugins_engine_get_default()),
                                              PEAS_TYPE_ACTIVATABLE, "object", box, NULL);




    peas_extension_set_foreach(priv->extensions,
                               (PeasExtensionSetForeachFunc)on_extension_added,
                               box);

    g_signal_connect(priv->extensions,
                     "extension-added",
                     G_CALLBACK(on_extension_added),
                     box);

    g_signal_connect(priv->extensions,
                     "extension-removed",
                     G_CALLBACK(on_extension_removed),
                     box);
}

Emu8086AppWindow * emu8086_app_plugin_box_get_mwindow(Emu8086AppPluginBox *box){
    return EMU8086_APP_WINDOW(box->priv->win);
}

void emu8086_app_plugin_box_set_window(Emu8086AppPluginBox *box, Emu8086AppWindow *win){
   box->priv->win= GTK_APPLICATION_WINDOW(win);
};