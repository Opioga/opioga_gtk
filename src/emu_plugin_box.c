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

#include <emu_plugin_box.h>
#include <emu_8086_plugins_engine.h>
#include <libpeas/peas-activatable.h>
#include <libpeas/peas-extension-set.h>

struct _Emu8086AppPluginBoxPrivate
{
    Emu8086AppWindow *win;
    PeasExtensionSet *extensions;
    Emu8086AppCodeRunner *runner;
};
G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppPluginBox, emu_8086_app_plugin_box, GTK_TYPE_BOX);

static void emu_8086_app_plugin_box_init(Emu8086AppPluginBox *runner);

static void emu_8086_app_plugin_box_class_init(Emu8086AppPluginBoxClass *klass);
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

static void emu_8086_app_plugin_box_set_property(GObject *object,
                                                 guint property_id,
                                                 const GValue *value,
                                                 GParamSpec *pspec);
static void
emu_8086_app_plugin_box_get_property(GObject *object,
                                     guint property_id,
                                     GValue *value,
                                     GParamSpec *pspec);

static void emu_8086_app_plugin_box_set_property(GObject *object,
                                                 guint property_id,
                                                 const GValue *value,
                                                 GParamSpec *pspec)
{
    Emu8086AppPluginBox *self = EMU_8086_APP_PLUGIN_BOX(object);
    // g_print("l %d\n", *value);

    switch ((Emu8086AppPluginBoxProperty)property_id)
    {

    case PROP_WIN:

        self->priv->win = EMU_8086_APP_WINDOW(g_value_get_object(value));
        // emu8086_win_change_theme(self);
        // g_print("filename: %s\n", self->filename);
        break;
    case PROP_MY_RUNNER:

        self->priv->runner = EMU_8086_APP_CODE_RUNNER(g_value_get_object(value));
        // emu8086_win_change_theme(self);
        // g_print("filename: %s\n", self->filename);
        break;
    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
emu_8086_app_plugin_box_get_property(GObject *object,
                                     guint property_id,
                                     GValue *value,
                                     GParamSpec *pspec)
{
    Emu8086AppPluginBox *self = EMU_8086_APP_PLUGIN_BOX(object);

    switch ((Emu8086AppPluginBoxProperty)property_id)
    {
    case PROP_WIN:
        g_value_set_object(value, self->priv->win);
        break;

    case PROP_MY_RUNNER:
        g_value_set_object(value, self->priv->runner);
        break;



    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

Emu8086AppPluginBox *emu_8086_app_plugin_box_new(GtkApplicationWindow *win, Emu8086AppCodeRunner *runner)
{
    return g_object_new(EMU_8086_APP_PLUGIN_BOX_TYPE,
                        "window", win,
                        "runner", runner,

                        NULL);
};

static void emu_8086_app_plugin_box_class_init(Emu8086AppPluginBoxClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    object_class->set_property = emu_8086_app_plugin_box_set_property;
    object_class->get_property = emu_8086_app_plugin_box_get_property;
    klass->get_stack = emu8086_app_window_get_stack;
// PROP_MY_RUNNER;


 g_object_class_install_property(object_class,
                                    PROP_MY_RUNNER,
                                    g_param_spec_object("runner",
                                                        "Runner",
                                                        "",

                                                        EMU_8086_APP_CODE_RUNNER_TYPE,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));


    g_object_class_install_property(object_class,
                                    PROP_WIN,
                                    g_param_spec_object("window",
                                                        "Window",
                                                        "",

                                                        EMU_8086_APP_WINDOW_TYPE,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

static void emu_8086_app_plugin_box_init(Emu8086AppPluginBox *box)
{
    box->priv = emu_8086_app_plugin_box_get_instance_private(box);
    PRIV_BOX;
    priv->extensions = peas_extension_set_new(PEAS_ENGINE(emu8086_plugins_engine_get_default()),
                                              PEAS_TYPE_ACTIVATABLE, "object", box, NULL);
   

    peas_extension_set_call(priv->extensions, "activate", box);
    g_signal_connect(priv->extensions,
                     "extension-added",
                     G_CALLBACK(on_extension_added),
                     box);

    g_signal_connect(priv->extensions,
                     "extension-removed",
                     G_CALLBACK(on_extension_removed),
                     box);
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

    //  g_print("0x%x", priv->runner);
    peas_activatable_activate(PEAS_ACTIVATABLE(exten));
    // peas_extension_call (exten, "kon",box);
    // peas_
    // peas_extension_set_call_valist
}

GtkWidget *emu8086_app_window_get_stack(Emu8086AppPluginBox *box)
{
    return GTK_WIDGET(box);
}
