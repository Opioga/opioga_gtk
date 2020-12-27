#include <gtk/gtk.h>

#include <emu8086app.h>
#include <emu8086win.h>
#include <emu8086appprefs.h>

struct _Emu8086AppPrefs
{
  GtkDialog parent;
};

typedef struct _Emu8086AppPrefsPrivate Emu8086AppPrefsPrivate;

struct _Emu8086AppPrefsPrivate
{
  GSettings *settings;
  GtkWidget *font;
  GtkWidget *theme;
  GtkWidget *highlight;
  GtkWidget *ul;
  GtkWidget *lf;
};

G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppPrefs, emu8086_app_prefs, GTK_TYPE_DIALOG)

static void
emu8086_app_prefs_init(Emu8086AppPrefs *prefs)
{
  Emu8086AppPrefsPrivate *priv;

  priv = emu8086_app_prefs_get_instance_private(prefs);
  gtk_widget_init_template(GTK_WIDGET(prefs));
  priv->settings = g_settings_new("org.gtk.emu8086app");

  g_settings_bind(priv->settings, "font",
                  priv->font, "font",
                  G_SETTINGS_BIND_DEFAULT);
  g_settings_bind(priv->settings, "theme",
                  priv->theme, "active-id",
                  G_SETTINGS_BIND_DEFAULT);

  g_settings_bind(priv->settings, "highlight",
                  priv->highlight, "active",
                  G_SETTINGS_BIND_DEFAULT);

  g_settings_bind(priv->settings, "ul",
                  priv->ul, "active",
                  G_SETTINGS_BIND_DEFAULT);
  g_settings_bind(priv->settings, "lf",
                  priv->lf, "label",
                  G_SETTINGS_BIND_DEFAULT);
}

static void
emu8086_app_prefs_dispose(GObject *object)
{
  Emu8086AppPrefsPrivate *priv;

  priv = emu8086_app_prefs_get_instance_private(EMU8086_APP_PREFS(object));
  g_clear_object(&priv->settings);

  G_OBJECT_CLASS(emu8086_app_prefs_parent_class)->dispose(object);
}

static void
emu8086_app_prefs_class_init(Emu8086AppPrefsClass *class)
{
  G_OBJECT_CLASS(class)->dispose = emu8086_app_prefs_dispose;

  gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                                              "/com/krc/emu8086app/prefs.ui");
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppPrefs, font);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppPrefs, theme);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppPrefs, highlight);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppPrefs, ul);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppPrefs, lf);
}

Emu8086AppPrefs *
emu8086_app_prefs_new(Emu8086AppWindow *win)
{
  return g_object_new(EMU8086_APP_PREFS_TYPE, "transient-for", win, "use-header-bar", TRUE, NULL);
}
