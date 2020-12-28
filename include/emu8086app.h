/* gtkemu8086
 * Copyright 2020 KRC
 *
 * Permission is hereby granted, free of charge, to any person obtaining 
 * a copy of this software and associated documentation files (the 
 * "Software"), to deal in the Software without restriction, including 
 * without limitation the rights to use, copy, modify, merge, publish, 
 * distribute, sublicense, and/or sell copies of the Software, and to 
 * permit persons to whom the Software is furnished to do so, subject 
 * to the following conditions: 
 *
 * The above copyright notice and this permission notice shall be included 
 * in all copies or substantial portions of the Software. 
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS 
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING 
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS 
 * IN THE SOFTWARE. 
 *
 * (i.e. the MIT License)
 *
 * code.c
 * General emulation functions
 */
#ifndef EMU_APP_C
#define EMU_APP_C
#include <gtk/gtk.h>

#define EMU_8086_APP_TYPE (emu_8086_app_get_type())
#define _PRIV Emu8086AppPrivate *priv = emu_8086_app_get_instance_private(app)

G_DECLARE_FINAL_TYPE(Emu8086App, emu_8086_app, EMU_8086, APP, GtkApplication)

Emu8086App *emu_8086_app_new(void);
void run_clicked_app(Emu8086App *app, gchar *fname);
void step_clicked_app(Emu8086App *app, gchar *fname);
void stop_clicked_app(Emu8086App *app);
void open_full_window(Emu8086App *app);
int emu_8086_app_update_usage(Emu8086App *app);
void init_start_stats(Emu8086App *app);
void notify_end(Emu8086App *app, uint64_t flag);
void update_max(Emu8086App *app, gpointer pointer, uint64_t max);
void emu_8086_app_reset_stats(Emu8086App *app);
void set_app_state(Emu8086App *app, gint state);
void step_over_clicked_app(Emu8086App *app, gchar *fname);
void open_help();
void _step(Emu8086App *app);
void quit(Emu8086App *app);

typedef struct _theme them;
struct _theme
{
    gchar *id;
    gchar *text;
};

enum app_state
{
    PLAYING,
    STOPPED,

    STEP
};
#endif