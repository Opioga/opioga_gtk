#include <emu8086appsidepane.h>
#include <emu8086appwin.h>
#include <emu8086.h>
typedef enum
{
    PROP_0,
    PROP_SIDE_PANE_WINDOW,
    PROP_USE_DEFAULT
} Emu8086AppSidePaneProperty;

typedef struct _Emu8086AppSidePanePrivate Emu8086AppSidePanePrivate;

struct _Emu8086AppSidePanePrivate
{
    Emu8086AppWindow *window;
    GtkWidget *AX_;
    GtkWidget *CX_;
    GtkWidget *DX_;
    GtkWidget *BX_;
    GtkWidget *SP_;
    GtkWidget *BP_;
    GtkWidget *SI_;
    GtkWidget *DI_;

    GtkWidget *FLAGS_;
    GtkWidget *IP_;
    GtkWidget *CS_;
    GtkWidget *DS_;
    GtkWidget *ip;
    GtkWidget *SS_;
    GtkWidget *ES_;
    GtkWidget *box;
    gboolean use_default;
};

G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppSidePane,
                           emu8086_app_side_pane,
                           GTK_TYPE_REVEALER);

static char *reg_str[] = {

    "AX",
    "CX",
    "DX",
    "BX",
    "SP",
    "BP",
    "SI",
    "DI",

    "FLAGS",
    "IP",
    "CS",
    "DS",
    "ES",
    "SS",
};

static void emu8086_app_side_pane_set_property(GObject *object,
                                               guint property_id,
                                               const GValue *value,
                                               GParamSpec *pspec);

static void emu8086_app_side_pane_get_property(GObject *object,
                                               guint property_id,
                                               GValue *value,
                                               GParamSpec *pspec);

static void emu8086_app_side_pane_class_init(Emu8086AppSidePaneClass *klass)
{
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    GObjectClass *object_class = G_OBJECT_CLASS(klass);

    
    object_class->set_property = emu8086_app_side_pane_set_property;
    object_class->get_property = emu8086_app_side_pane_get_property;

    g_object_class_install_property(object_class,
                                    PROP_SIDE_PANE_WINDOW,
                                    g_param_spec_object("window",
                                                        "Window",
                                                        "The side panel's window",

                                                        EMU8086_APP_WINDOW_TYPE,
                                                        G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY));
}

static void emu8086_app_side_pane_init(Emu8086AppSidePane *pane)
{

    pane->priv = emu8086_app_side_pane_get_instance_private(pane);
    PRIV_SP;
    priv->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget **regs[] = {
        &priv->AX_,
        &priv->CX_,
        &priv->DX_,
        &priv->BX_,
        &priv->SP_,
        &priv->BP_,
        &priv->SI_,
        &priv->DI_,

        &priv->FLAGS_,
        &priv->IP_,
        &priv->CS_,
        &priv->DS_,
        &priv->ES_,
        &priv->SS_,
    };

    for (int i = 0; i < 14; i++)
    {
        /* code */
        *(regs[i]) = gtk_label_new(reg_str[i]);
        gtk_container_add(GTK_CONTAINER(priv->box), *(regs[i]));
    }

    gtk_widget_set_margin_start(priv->box, 10);
    gtk_widget_set_margin_end(priv->box, 10);
    gtk_container_add(GTK_CONTAINER(pane), priv->box);
    // pane
}
Emu8086AppSidePane *emu8086_app_side_pane_new(GtkApplicationWindow *window)
{
    return g_object_new(EMU8086_APP_SIDE_PANE_TYPE, "window", window, NULL);
}

static void emu8086_app_side_pane_set_property(GObject *object,
                                               guint property_id,
                                               const GValue *value,
                                               GParamSpec *pspec)
{
    Emu8086AppSidePane *pane = EMU8086_APP_SIDE_PANE(object);
    PRIV_SP;
 gpointer v;
    switch ((Emu8086AppSidePaneProperty)property_id)
    {
    case PROP_SIDE_PANE_WINDOW:
        v = g_value_get_object(value);
        if (v != NULL)
            priv->window = EMU8086_APP_WINDOW(v);
        break;
    case PROP_USE_DEFAULT:
        priv->use_default = g_value_get_boolean(value);
        break;
    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void emu8086_app_side_pane_get_property(GObject *object,
                                               guint property_id,
                                               GValue *value,
                                               GParamSpec *pspec)
{
    Emu8086AppSidePane *pane = EMU8086_APP_SIDE_PANE(object);
    PRIV_SP;

    switch ((Emu8086AppSidePaneProperty)property_id)
    {
    case PROP_SIDE_PANE_WINDOW:
        g_value_set_object(value, priv->window);
        break;

    case PROP_USE_DEFAULT:
        g_value_set_boolean(value, priv->use_default);
        break;

        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

void milli(GtkLabel *label, char *r, unsigned short reg)
{
    char buf[20];
    sprintf(buf, "%s: %04x", r, reg);
    gtk_label_set_text(GTK_LABEL(label), buf);
}

void emu8086_app_side_pane_update_view(Emu8086AppSidePane *pane, unsigned short *values)
{
    PRIV_SP;
    GtkWidget **regs[] = {
        &priv->AX_,
        &priv->CX_,
        &priv->DX_,
        &priv->BX_,
        &priv->SP_,
        &priv->BP_,
        &priv->SI_,
        &priv->DI_,

        &priv->FLAGS_,
        &priv->IP_,
        &priv->CS_,
        &priv->DS_,
        &priv->ES_,
        &priv->SS_,
    };
    for (int i = 0; i < 14; i++)
    {
        gint index = i;
        if (index > 7)
            index = index + 8;

        unsigned short value = values[index];
        char *ren = (reg_str[i]);
        GtkLabel *label = GTK_LABEL(*(regs[i]));
        gchar buf[20];

        milli(label, ren, value);
      
    }
}


GtkWidget *emu8086_app_side_pane_get_box(Emu8086AppSidePane *pane){
    PRIV_SP;
    return priv->box;
}