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






#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "emu8086app.h"

int main(int argc, char *argv[])
{
    guint interval = 1000;
    Emu8086App *a = emu_8086_app_new();

    // gint l = g_timeout_add(interval, (GSourceFunc)emu_8086_app_update_usage, a);

    return g_application_run(G_APPLICATION(a), argc, argv);
}
