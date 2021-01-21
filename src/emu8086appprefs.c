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
 * emu8086appprefs.c
 * CPU class
 */

#include <gtk/gtk.h>

#include <emu8086app.h>
#include <emu8086win.h>
#include <emu_8086_plugins_engine.h>
#include <emu8086stylescheme.h>
#include <emu8086appprefs.h>
#include <libpeas-gtk/peas-gtk-plugin-manager.h>

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
  Emu8086AppStyleScheme *scheme;
  gint theme_size;
  GSettings *settings;
  GtkWidget *font;
  GtkWidget *schemes_treeview;
  GtkWidget *highlight;
  GtkWidget *ul;
  GtkWidget *lf;
  GtkWidget *fcb;
  GtkListStore *themes_treeview_model;
  GtkWidget *themes_treeview;
  GtkWidget *indent;
  GtkWidget *pluginsbox;
  GtkAdjustment *update_frequency;GtkWidget *uninstall_scheme_button;
  GtkWidget *install_scheme_button;
  GtkWidget *install_scheme_file_schooser;
  them **themes;
};
struct _Emu8086AppPrefs
{
  GtkDialog parent;
  Emu8086AppPrefsPrivate *priv;
  gint theme_size;
};

G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppPrefs, emu8086_app_prefs, GTK_TYPE_DIALOG)
static refresh_themes(Emu8086AppPrefs *dlg);

static void
style_scheme_changed(GtkWidget *treeview,
                     Emu8086AppPrefs *dlg)
{
  GtkTreePath *path;
  GtkTreeIter iter;
  gchar *id;

  gtk_tree_view_get_cursor(GTK_TREE_VIEW(dlg->priv->schemes_treeview), &path, NULL);
  gtk_tree_model_get_iter(GTK_TREE_MODEL(dlg->priv->themes_treeview_model),
                          &iter, path);
  gtk_tree_path_free(path);
  gtk_tree_model_get(GTK_TREE_MODEL(dlg->priv->themes_treeview_model),
                     &iter, ID_COLUMN, &id, -1);
  GSettings *settings = dlg->priv->settings;
  g_settings_set_string(dlg->priv->settings, "theme", id);

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

static void
scheme_description_cell_data_func(GtkTreeViewColumn *column,
                                  GtkCellRenderer *renderer,
                                  GtkTreeModel *model,
                                  GtkTreeIter *iter,
                                  gpointer data)
{
  gchar *id;
  gchar *desc;
  gchar *text;
  gtk_tree_model_get(model, iter,
                     ID_COLUMN, &id,
                     NAME_COLUMN, &text,

                     -1);

  if (text != NULL)
  {
    desc = g_markup_printf_escaped("<b>%s</b> - %s",
                                   id,
                                   text);
  }
  else
  {
    desc = g_markup_printf_escaped("<b>%s</b>",
                                   id);
  }
  g_object_set(G_OBJECT(renderer),
               "markup",
               desc,
               NULL);

  g_free(text);
  g_free(desc);
  g_free(id);
}

static void
add_scheme_chooser_response_cb(GtkDialog *chooser,
                               gint res_id,
                               Emu8086AppPrefs *dlg)
{
  GFile *file = NULL;
  const gchar *scheme_id;

  if (res_id != GTK_RESPONSE_ACCEPT)
  {
    gtk_widget_hide(GTK_WIDGET(chooser));
    return;
  }
  file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(chooser));
  if (file == NULL)
    return;

  gtk_widget_hide(GTK_WIDGET(chooser));
  gchar *n_theme = emu_style_scheme_install_theme(dlg->priv->scheme, file);
  g_print("%s \n", n_theme);
  g_object_unref(file);
  if (n_theme == NULL)
    return;
  else
    g_free(n_theme);
  for (int i = dlg->priv->theme_size; i > 0; i--)
  {
    emu8086_theme_free(dlg->priv->themes[i - 1]);
  }
  

  dlg->priv->themes = NULL;g_print("here\n");
  refresh_themes(dlg);
  // gtk_list_store_clear(dlg->priv->themes_treeview_model);
}

static void
install_scheme_clicked(GtkButton *button,
                       Emu8086AppPrefs *dlg)
{
  GtkWidget *chooser;
  GtkFileFilter *filter;

  if (dlg->priv->install_scheme_file_schooser != NULL)
  {
    gtk_window_present(GTK_WINDOW(dlg->priv->install_scheme_file_schooser));
    gtk_widget_grab_focus(dlg->priv->install_scheme_file_schooser);
    return;
  }

  chooser = gtk_file_chooser_dialog_new(("Add Theme"),
                                        GTK_WINDOW(dlg),
                                        GTK_FILE_CHOOSER_ACTION_OPEN,
                                        "_Cancel",
                                        GTK_RESPONSE_CANCEL,
                                        "_Open",
                                        GTK_RESPONSE_ACCEPT,
                                        NULL);

  gtk_window_set_destroy_with_parent(GTK_WINDOW(chooser), TRUE);

  /* Filters */
  filter = gtk_file_filter_new();
  gtk_file_filter_set_name(filter, ("Theme Files"));
  gtk_file_filter_add_pattern(filter, "*.theme");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);

  gtk_file_chooser_set_filter(GTK_FILE_CHOOSER(chooser), filter);

  filter = gtk_file_filter_new();
  gtk_file_filter_set_name(filter, ("All Files"));
  gtk_file_filter_add_pattern(filter, "*");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(chooser), filter);

  gtk_dialog_set_default_response(GTK_DIALOG(chooser), GTK_RESPONSE_ACCEPT);

  g_signal_connect(chooser,
                   "response",
                   G_CALLBACK(add_scheme_chooser_response_cb),
                   dlg);

  dlg->priv->install_scheme_file_schooser = chooser;

  g_object_add_weak_pointer(G_OBJECT(chooser),
                            (gpointer)&dlg->priv->install_scheme_file_schooser);

  gtk_widget_show(chooser);
}

static refresh_themes(Emu8086AppPrefs *dlg)
{

  Emu8086AppPrefsPrivate *priv = dlg->priv;
  Emu8086AppStyleScheme *scheme = priv->scheme;
  gsize theme_size = 0;
 gtk_list_store_clear(priv->themes_treeview_model);

  them **ths = emu_8086_app_style_scheme_get_themes(scheme, &theme_size);
  
  gchar *selected_theme = g_settings_get_string(priv->settings, "theme");
   g_print("heren %d\n", theme_size);
   
  for (int i = 0; i < theme_size; i++)
  {
    const gchar *id;
    const gchar *name;
    them *theme = ths[i];
    id = theme->id;
    name = theme->text;
    
    GtkTreeIter iter;
    gtk_list_store_append(priv->themes_treeview_model, &iter);
    gtk_list_store_set(priv->themes_treeview_model, &iter, ID_COLUMN, id, NAME_COLUMN, name, ACTIVE_COLUMN, FALSE, -1);

    if (strcmp(id, selected_theme) == 0)
    {
      GtkTreeSelection *selection;

      selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(priv->schemes_treeview));
      gtk_tree_selection_select_iter(selection, &iter);
    }
  }

  priv->themes = ths;
  priv->theme_size = theme_size;
}

static void populate_schemes(Emu8086AppPrefs *dlg)
{

  Emu8086AppPrefsPrivate *priv = dlg->priv;
  GSList *schemes;
  GSList *l;
  Emu8086AppStyleScheme *scheme = emu_8086_app_style_scheme_get_default();

  GtkTreePath *tree;
  GtkListStore *store;
  GtkTreeIter iter;
  GtkCellRenderer *renderer;
  GtkTreeViewColumn *column;
  GtkTreeSelection *selection;

  priv->themes_treeview_model = gtk_list_store_new(NUM_COLUMNS,
                                                   G_TYPE_STRING,
                                                   G_TYPE_STRING,
                                                   G_TYPE_BOOLEAN);
  gtk_tree_view_set_model(GTK_TREE_VIEW(priv->schemes_treeview), GTK_TREE_MODEL(priv->themes_treeview_model));

  gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE(priv->themes_treeview_model),
                                       0,
                                       GTK_SORT_ASCENDING);

  column = gtk_tree_view_column_new();
  renderer = gtk_cell_renderer_text_new();
  g_object_set(renderer, "ellipsize", PANGO_ELLIPSIZE_END, NULL);
  gtk_tree_view_column_pack_start(column, renderer, TRUE);
  gtk_tree_view_column_set_cell_data_func(column,
                                          renderer,
                                          scheme_description_cell_data_func,
                                          dlg,
                                          NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(dlg->priv->schemes_treeview),
                              column);

  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(dlg->priv->schemes_treeview));
  gtk_tree_selection_set_mode(selection, GTK_SELECTION_BROWSE);

  priv->scheme = scheme;

  g_signal_connect(dlg->priv->schemes_treeview,
                   "cursor-changed",
                   G_CALLBACK(style_scheme_changed),
                   dlg);

  g_signal_connect(dlg->priv->install_scheme_button,
                   "clicked",
                   G_CALLBACK(install_scheme_clicked),
                   dlg);  refresh_themes(dlg);

                   gtk_button_set_label(dlg->priv->uninstall_scheme_button, "More Info");
}
static void license_file_chosen(GtkFileChooserButton *fcb, Emu8086AppPrefs *dlg)
{
  GFile *file;
  file = gtk_file_chooser_get_file(GTK_FILE_CHOOSER(fcb));
  g_settings_set_string(dlg->priv->settings, "lf", g_file_get_path(file));
  g_settings_set_string(dlg->priv->settings, "license", g_file_get_basename(file));
  g_object_unref(file);
}
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
  g_settings_bind(priv->settings, "ul",
                  priv->fcb, "sensitive",
                  G_SETTINGS_BIND_GET);
  g_settings_bind(priv->settings, "license",
                  priv->lf, "label",
                  G_SETTINGS_BIND_GET);
  g_settings_bind(priv->settings, "license",
                  GTK_FILE_CHOOSER_BUTTON(priv->fcb), "title",
                  G_SETTINGS_BIND_GET);
  g_settings_bind(priv->settings, "frequency", priv->update_frequency, "value", G_SETTINGS_BIND_DEFAULT);

  g_signal_connect(prefs,
                   "response",
                   G_CALLBACK(dialog_response_handler),
                   NULL);
  Emu8086PluginsEngine *engine = emu8086_plugins_engine_get_default();
  GtkWidget *page_content;
  page_content = peas_gtk_plugin_manager_new(PEAS_ENGINE(engine));
  g_return_if_fail(page_content != NULL);
  g_signal_connect(priv->fcb, "file-set", G_CALLBACK(license_file_chosen), prefs);
  gtk_box_pack_start(GTK_BOX(priv->pluginsbox),
                     page_content,
                     TRUE,
                     TRUE,
                     0);
  prefs->priv = priv;
  priv->install_scheme_file_schooser = NULL;
  populate_schemes(prefs);

  // g_signal_connect(GTK_WIDGET("destroy",))
}

static void
emu8086_app_prefs_dispose(GObject *object)
{
  Emu8086AppPrefsPrivate *priv;

  priv = emu8086_app_prefs_get_instance_private(EMU8086_APP_PREFS(object));
  if (priv->themes != NULL)
  {
    g_print("destroy\n");
    for (int i = priv->theme_size; i > 0; i--)
    {
      emu8086_theme_free(priv->themes[i - 1]);
    }
    priv->themes = NULL;
  }
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
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppPrefs, schemes_treeview);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppPrefs, highlight);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppPrefs, ul);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppPrefs, lf);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppPrefs, fcb);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppPrefs, indent);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppPrefs, pluginsbox);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppPrefs, install_scheme_button);
   gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppPrefs, uninstall_scheme_button);
  gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), Emu8086AppPrefs, update_frequency);
}

Emu8086AppPrefs *
emu8086_app_prefs_new(Emu8086AppWindow *win)
{
  return g_object_new(EMU8086_APP_PREFS_TYPE, "transient-for", win, "use-header-bar", FALSE, NULL);
}
