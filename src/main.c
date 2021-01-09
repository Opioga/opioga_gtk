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
 * main.c
 * CPU class
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <locale.h>
#include <stdlib.h>
#include <string.h>

#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>

#ifdef HAVE_INTROSPECTION
#include <girepository.h>
#endif

#include "emu8086app.h"
#include <emu8086win.h>
#include <emu_8086_plugins_engine.h>

static gboolean new_window_option = FALSE;
static gboolean new_document_option = FALSE;
static gchar **remaining_args = NULL;
static GSList *file_list = NULL;

static void
show_version_and_quit(void)
{
    g_print("%s - Version %s\n", g_get_application_name(), VERSION);

    exit(0);
}

static const GOptionEntry options[] =
    {
        {"version", 'V', G_OPTION_FLAG_NO_ARG, G_OPTION_ARG_CALLBACK,
         show_version_and_quit, ("Show the application's version"), NULL},

        {"new-window", '\0', 0, G_OPTION_ARG_NONE, &new_window_option,
         N_("Create a new top-level window in an existing instance of emu8086"), NULL},

        {"new-document", '\0', 0, G_OPTION_ARG_NONE, &new_document_option,
         N_("Create a new document in an existing instance of emu8086"), NULL},

        {G_OPTION_REMAINING, '\0', 0, G_OPTION_ARG_FILENAME_ARRAY, &remaining_args,
         NULL, N_("[FILE...]")}, /* collects file arguments */

        {NULL}};

static void
emu8086_get_command_line_data(void)
{
    if (remaining_args)
    {
        gint i;

        for (i = 0; remaining_args[i]; i++)
        {

            GFile *file;

            file = g_file_new_for_commandline_arg(remaining_args[i]);
            file_list = g_slist_prepend(file_list, file);
        }

        file_list = g_slist_reverse(file_list);
    }
}

int main(int argc, char *argv[])
{
    GOptionContext *context;
    Emu8086PluginsEngine *engine;
    Emu8086App *a;
    GError *error = NULL;
    Emu8086AppWindow *window;
    context = g_option_context_new(_("- Edit text files"));
    g_option_context_add_main_entries(context, options, NULL);
    g_option_context_add_group(context, gtk_get_option_group(TRUE));

#ifdef HAVE_INTROSPECTION
    g_option_context_add_group(context, g_irepository_get_option_group());
#endif

    if (!g_option_context_parse(context, &argc, &argv, &error))
    {
        g_print(_("%s\nRun '%s --help' to see a full list of available command line options.\n"),
                error->message, argv[0]);
        g_error_free(error);
        g_option_context_free(context);
        return 1;
    }
    g_option_context_free(context);

    engine = emu8086_plugins_engine_get_default();
    a = emu_8086_app_get_default();

    // if (file_list != NULL)
    // {
    //     while (file_list != NULL)
    //     {
    //         window = emu_8086_app_window_new(a);
    //         emu_8086_app_window_open(window, file_list->data);
    //     }
    // }
    // else
    // {
    //     window = emu_8086_app_window_new(a);
    //     emu_8086_app_window_up(window);
    // }
    // gtk_main();
    // Emu8086App *a = emu_8086_app_new();
    // gint l = g_timeout_add(interval, (GSourceFunc)emu_8086_app_update_usage, a);

    return g_application_run(G_APPLICATION(a), argc, argv);
}
