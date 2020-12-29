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

#ifndef _EMU_CODE_RUNNER_C
#define _EMU_CODE_RUNNER_C
#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _Emu8086AppCodeRunnerClass Emu8086AppCodeRunnerClass;
#define EMU_8086_APP_CODE_RUNNER_TYPE (emu_8086_app_code_runner_get_type())
#define EMU_8086_CODE_RUNNER_IS_RUNNER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), EMU_8086_APP_CODE_RUNNER_TYPE))
#define PRIV_CODE_RUNNER Emu8086AppCodeRunnerPrivate *priv = runner->priv
#define PRIV_CODE_RUNNER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), PLUMA_TYPE_WINDOW, PlumaWindowClass))
Emu8086AppCodeRunner *emu_8086_app_code_runner_new(gchar *fname, gboolean can_run);

void run_clicked_app(Emu8086AppCodeRunner *runner);
void step_clicked_app(Emu8086AppCodeRunner *runner);
void stop_clicked_app(Emu8086AppCodeRunner *runner);
void set_app_state(Emu8086AppCodeRunner *runner, gint state);
void step_over_clicked_app(Emu8086AppCodeRunner *runner);
void open_help();
void _step(Emu8086AppCodeRunner *runner);

#endif