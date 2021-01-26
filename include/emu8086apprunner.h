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

#ifndef _EMU_CODE_RUNNER_C
#define _EMU_CODE_RUNNER_C
#include <gtk/gtk.h>
#include "emu8086apptypes.h"


G_BEGIN_DECLS
struct emu8086;
#define EMU8086_APP_CODE_RUNNER_TYPE (emu8086_app_code_runner_get_type())
#define EMU8086_CODE_RUNNER_IS_RUNNER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), EMU8086_APP_CODE_RUNNER_TYPE))
#define PRIV_CODE_RUNNER Emu8086AppCodeRunnerPrivate *priv = runner->priv
#define EMU8086_APP_CODE_RUNNER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), EMU8086_APP_CODE_RUNNER_TYPE, Emu8086AppCodeRunner))
#define EMU8086_APP_CODE_RUNNER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), EMU8086_APP_CODE_RUNNER_TYPE, Emu8086AppCodeRunnerClass))
#define EMU8086_CODE_RUNNER_IS_RUNNER_CLASS(klass)(G_TYPE_CHECK_CLASS_TYPE((klass), EMU8086_APP_CODE_RUNNER_TYPE)

typedef struct _Emu8086AppCodeRunnerPrivate Emu8086AppCodeRunnerPrivate;
struct _Emu8086AppCodeRunnerPrivate
{
    struct emu8086 *aCPU;
    char *fname;
    guint to;
    gint state;
    gboolean can_run;
    gint ie;
    gchar *em;
    gint f;
    Emu8086AppCode *code;
  
    gint m_ins;
    gint update_frequency;
};

struct _Emu8086AppCodeRunner
{
    GObject parent;
    Emu8086AppCodeRunnerPrivate *priv;
};

typedef struct _Emu8086AppCodeRunnerClass Emu8086AppCodeRunnerClass;

struct _Emu8086AppCodeRunnerClass
{
    GObjectClass parent_class;
    void (*exec_ins)(Emu8086AppCodeRunner *runner);
    void (*error_occurred)(Emu8086AppCodeRunner *runner);
    void (*interrupt)(Emu8086AppCodeRunner *runner);
    void (*exec_stopped)(Emu8086AppCodeRunner *runner);
};

typedef enum
{
    PROP_RUNNER_0,
    PROP_RUNNER_FNAME,
    PROP_RUNNER_CODE,
    PROP_RUNNER_CAN_RUN,
    PROP_ACPU,
    PROP_U_F

} Emu8086AppCodeRunnerProperty;

typedef enum
{
    PLAYING,
    STOPPED,

    STEP,
    STEP_OVER
} Emu8086AppCodeRunnerState;

GType emu8086_app_code_runner_get_type(void) G_GNUC_CONST;
gboolean emu8086_app_code_runner_get_can_run(Emu8086AppCodeRunner *runner);
gchar *emu8086_app_code_runner_get_fname(Emu8086AppCodeRunner *runner);
Emu8086AppCodeRunner *emu8086_app_code_runner_new(gchar *fname, gboolean can_run);
void set_fname(Emu8086AppCodeRunner *runner, gchar *fname);
void run_clicked_app(Emu8086AppCodeRunner *runner);
void step_clicked_app(Emu8086AppCodeRunner *runner);
void stop_clicked_app(Emu8086AppCodeRunner *runner);
void set_app_state(Emu8086AppCodeRunner *runner, gint state);
void step_over_clicked_app(Emu8086AppCodeRunner *runner, Emu8086AppCode *code);
void open_help();
void _step(Emu8086AppCodeRunner *runner);
void stop(Emu8086AppCodeRunner *runner, gboolean reset);
struct emu8086 *getCPU(Emu8086AppCodeRunner *runner);
gchar *emu8086_app_code_runner_get_errors(Emu8086AppCodeRunner *runner);
G_END_DECLS
#endif