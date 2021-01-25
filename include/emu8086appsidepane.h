#ifndef _EMU_SIDE_PANE_C
#define _EMU_SIDE_PANE_C
#include <gtk/gtk.h>
#include "emu8086apptypes.h"
G_BEGIN_DECLS


#define PRIV_SP Emu8086AppSidePanePrivate *priv = pane->priv
#define EMU8086_APP_SIDE_PANE_TYPE (emu8086_app_side_pane_get_type())

#define EMU8086_APP_SIDE_PANE(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), EMU8086_APP_SIDE_PANE_TYPE, Emu8086AppSidePane))
#define EMU8086_APP_SIDE_PANE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), EMU8086_APP_SIDE_PANE_TYPE, Emu8086AppSidePaneClass))
#define EMU8086_IS_APP_SIDE_PANE(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), EMU8086_APP_SIDE_PANE_TYPE))
#define EMU8086_IS_APP_SIDE_PANE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), EMU8086_APP_SIDE_PANE_TYPE))
#define EMU8086_APP_SIDE_PANE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), EMU8086_APP_SIDE_PANE_TYPE, Emu8086AppSidePaneClass))


typedef struct _Emu8086AppSidePaneClass Emu8086AppSidePaneClass;
typedef struct _Emu8086AppSidePanePrivate Emu8086AppSidePanePrivate;

struct _Emu8086AppSidePane
{
    GtkRevealer parent;


    Emu8086AppSidePanePrivate *priv;
};

struct _Emu8086AppSidePaneClass
{
    GtkRevealerClass parent_class;


    gpointer padding[2];
};

GType emu8086_app_side_pane_get_type(void) G_GNUC_CONST;


Emu8086AppSidePane *emu8086_app_side_pane_new(GtkApplicationWindow *window);
gboolean *emu8086_app_side_pane_use_default(Emu8086AppSidePane *pane,gboolean use_default);
void emu8086_app_side_pane_update_view(Emu8086AppSidePane *pane, unsigned short *values);

G_END_DECLS
#endif