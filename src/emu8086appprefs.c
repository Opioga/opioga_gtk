#include <gtk/gtk.h>

#include <emu8086app.h>
#include <emu8086win.h>
#include <emu8086appprefs.h>

static GtkWidget *preferences_dialog = NULL;

enum
{
  ID_COLUMN = 0,
  NAME_COLUMN,
  ACTIVE_COLUMN,
  NUM_COLUMNS
};

typedef enum
{
  DRAW_NONE = 0,
  DRAW_TRAILING = 1,
  DRAW_ALL = 2
} DrawSpacesSettings;

typedef struct _Emu8086AppPrefsPrivate Emu8086AppPrefsPrivate;

struct _Emu8086AppPrefsPrivate
{
  GSettings *settings;
  GtkWidget *font;
  GtkWidget *theme;
  GtkWidget *highlight;
  GtkWidget *ul;
  GtkWidget *lf;
  GtkWidget *fcb;
  GtkListStore *themes_treeview_model;
  GtkWidget *themes_treeview;
  GtkWidget *indent;
};
struct _Emu8086AppPrefs
{
  GtkDialog parent;
  Emu8086AppPrefsPrivate *priv;
};

G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppPrefs, emu8086_app_prefs, GTK_TYPE_DIALOG)

static void
style_scheme_changed(GtkWidget *treeview,
                     Emu8086AppPrefs *dlg)
{
  GtkTreePath *path;
  GtkTreeIter iter;
  gchar *id;

  gtk_tree_view_get_cursor(GTK_TREE_VIEW(dlg->priv->themes_treeview), &path, NULL);
  gtk_tree_model_get_iter(GTK_TREE_MODEL(dlg->priv->themes_treeview_model),
                          &iter, path);
  gtk_tree_path_free(path);
  gtk_tree_model_get(GTK_TREE_MODEL(dlg->priv->themes_treeview_model),
                     &iter, ID_COLUMN, &id, -1);
  GSettings *settings = dlg->priv->settings;
  g_settings_set_string(settings, "theme", id);

  set_buttons_sensisitivity_according_to_scheme(dlg, id);

  g_free(id);
}

static void
dialog_response_handler(GtkDialog *dlg,
                        gint res_id)
{
  g_print("here");
  switch (res_id)
  {
  case GTK_RESPONSE_HELP:
    open_help();

    break;

  default:
    gtk_widget_hide(GTK_WIDGET(dlg));
  }
}

static them themes[] = {
    {"dark+", "Dark+"},
    {"cobalt", "Cobalt"},
    {"light", "Light"}};

static void
emu8086_app_prefs_init(Emu8086AppPrefs *prefs)
{
  Emu8086AppPrefsPrivate *priv;

  priv = emu8086_app_prefs_get_instance_private(prefs);
  gtk_widget_init_template(GTK_WIDGET(prefs));
  priv->settings = g_settings_new("com.krc.emu8086app");

  g_settings_bind(priv->settings, "font",
                  priv->font, "font",
                  G_SETTINGS_BIND_DEFAULT);

  g_settings_bind(priv->settings, "highlight",
                  priv->highlight, "active",
                  G_SETTINGS_BIND_DEFAULT);
  g_settings_bind(priv->settings, "ai",
                  priv->indent, "active",
                  G_SETTINGS_BIND_DEFAULT);

  g_settings_bind(priv->settings, "ul",
                  priv->ul, "active",
                  G_SETTINGS_BIND_DEFAULT);
  g_settings_bind(priv->settings, "lf",
                  priv->lf, "label",
                  G_SETTINGS_BIND_DEFAULT);

  GtkTreePath *tree;
  GtkListStore *list_store;
  GtkTreeIter iter;
  gint i;

  list_store = gtk_list_store_new(NUM_COLUMNS,
                                  G_TYPE_INT,
                                  G_TYPE_STRING,
                                  G_TYPE_BOOLEAN);
  for (i = 0; i < 3; i++)
  {
    them some_data = themes[i];
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(priv->theme), some_data.id, some_data.text);
  }

  g_settings_bind(priv->settings, "theme",
                  priv->theme, "active-id",
                  G_SETTINGS_BIND_DEFAULT);
  g_signal_connect(prefs,
                   "response",
                   G_CALLBACK(dialog_response_handler),
                   NULL);
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
                                              "/com/krc/emu8086app/ui/pref.ui");
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppPrefs, font);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppPrefs, theme);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppPrefs, highlight);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppPrefs, ul);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppPrefs, lf);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppPrefs, indent);
}

Emu8086AppPrefs *
emu8086_app_prefs_new(Emu8086AppWindow *win)
{
  return g_object_new(EMU8086_APP_PREFS_TYPE, "transient-for", win, "use-header-bar", FALSE, NULL);
}
