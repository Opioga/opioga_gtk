/* gtkemu8086
 * Copyright 2020 KRC
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source errtextview must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * errtextview.c
 * ErrTextView class
 */

#include <emu8086apperrtextview.h>

#include <pango_css.h>
#include <emu8086stylescheme.h>
#include <emu8086apptopgutter.h>
#include <emu8086appwin.h>

typedef enum
{
    PROP_0,
    PROP_ERR_TEXT_VIEW_FONT,

} Emu8086AppErrTextViewProperty;

struct _Emu8086AppErrTextViewPrivate
{

    Emu8086AppStyleScheme *scheme;
    gchar *font;
    GtkStyleProvider *provider;
    GtkTextBuffer *buffer;
    gchar *color;
    gboolean auto_indent;
    GSettings *settings;
    Emu8086AppTopGutter *gutter;
    GMenuModel *menu;
    Emu8086AppWindow *window;
    GtkMenu *popup;
    gboolean attached;
};

G_DEFINE_TYPE_WITH_PRIVATE(Emu8086AppErrTextView, emu8086_app_err_text_view, GTK_TYPE_TEXT_VIEW);
static void
emu8086_app_err_text_view_change_color(Emu8086AppErrTextView *view);

static gboolean
emu8086_app_err_text_view_draw(GtkWidget *widget,
                               cairo_t *cr);

static void
menu_item_activate_clear(GSimpleAction *action,
                         GVariant *parameter,
                         gpointer appe)
{
    Emu8086AppErrTextView *view;
    view = EMU8086_APP_ERR_TEXT_VIEW(appe);
    PRIV_ERR_TEXT_VIEW;
    g_print("lol\n");
    if(!GTK_IS_TEXT_BUFFER(priv->buffer))
        priv->buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(appe));
    gtk_text_buffer_set_text(priv->buffer, "Nothing Doing\n Errors: 0", -1);
}
static void close_bottom_pane(GSimpleAction *action,
                              GVariant *parameter,
                              gpointer appe)
{
    Emu8086AppErrTextView *view;
    view = EMU8086_APP_ERR_TEXT_VIEW(appe);
    PRIV_ERR_TEXT_VIEW;
    g_print("lol\n");
    emu8086_app_window_set_bottom_pane(priv->window, FALSE);
}

static GActionEntry err_text_view_entries[] = {

    {"clear", menu_item_activate_clear, NULL, NULL, NULL},
    {"close_bottom_pane", close_bottom_pane, NULL, NULL, NULL},
    // NULL

};

static void
emu8086_app_err_text_view_set_property(GObject *object,
                                       guint property_id,
                                       const GValue *value,
                                       GParamSpec *pspec)
{
    Emu8086AppErrTextView *view;
    view = EMU8086_APP_ERR_TEXT_VIEW(object);
    PRIV_ERR_TEXT_VIEW;
    PangoFontDescription *desc;

    gchar *m, *v;

    switch ((Emu8086AppErrTextViewProperty)property_id)
    {
    case PROP_ERR_TEXT_VIEW_FONT:

        v = g_value_get_string(value);
        desc = pango_font_description_from_string(v);
        priv->font = g_strdup(v);

        pango_font_description_free(desc);
        emu8086_app_err_text_view_change_color(view);
        break;
    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}
static void
emu8086_app_err_text_view_get_property(GObject *object,
                                       guint property_id,
                                       const GValue *value,
                                       GParamSpec *pspec)
{
    Emu8086AppErrTextView *view;
    view = EMU8086_APP_ERR_TEXT_VIEW(object);

    switch ((Emu8086AppErrTextViewProperty)property_id)
    {
    case PROP_ERR_TEXT_VIEW_FONT:
        g_value_set_string(value, view->priv->font);
        break;
    default:
        /* We don't have any other property... */
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
do_popup_menu(GtkWidget *my_widget, GdkEventButton *event)
{
    Emu8086AppErrTextView *view;
    view = EMU8086_APP_ERR_TEXT_VIEW(my_widget);
    PRIV_ERR_TEXT_VIEW;
    GtkWidget *menu;
    int button, event_time;

    // g_signal_connect(menu, "deactivate",
    //                  G_CALLBACK(gtk_widget_destroy), NULL);

    /* ... add menu items ... */
    g_return_if_fail(GTK_IS_MENU(priv->popup));
    menu = GTK_WIDGET(priv->popup);

    if (event)
    {
        button = event->button;
        event_time = event->time;
    }
    else
    {
        button = 0;
        event_time = gtk_get_current_event_time();
    }

    if (!priv->attached)
    {
        gtk_menu_attach_to_widget(GTK_MENU(menu), my_widget, NULL);
        priv->attached = TRUE;
    }

    gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
                   button, event_time);
}

static void emu8086_app_err_text_view_populate_popup(GtkWidget *widget)
{

    do_popup_menu(widget, NULL);

    g_print("lol\n");
    return TRUE;
}
static gboolean emu8086_app_err_text_view_button_press_event(GtkWidget *widget,
                                                             GdkEventButton *event)
{
    if (gdk_event_triggers_context_menu((GdkEvent *)event) &&
        event->type == GDK_BUTTON_PRESS)
    {
        do_popup_menu(widget, event);
        return TRUE;
    }

    return GTK_WIDGET_CLASS(emu8086_app_err_text_view_parent_class)->button_press_event(widget, event);
}

static void emu8086_app_err_text_view_class_init(Emu8086AppErrTextViewClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS(klass);
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
    object_class->set_property = emu8086_app_err_text_view_set_property;
    object_class->get_property = emu8086_app_err_text_view_get_property;
    // widget_class->draw = emu8086_app_err_text_view_draw;
    // widget_class->popup_menu = emu8086_app_err_text_view_populate_popup;
    // widget_class->button_press_event = emu8086_app_err_text_view_button_press_event;

    g_object_class_install_property(object_class, PROP_ERR_TEXT_VIEW_FONT,
                                    g_param_spec_string("font", "Font", "Editor Font", "Monospace Regular 16",
                                                        G_PARAM_READWRITE));
}
static void _refresh_theme(Emu8086AppStyleScheme *scheme,
                           gpointer user_data)

{

    Emu8086AppErrTextView *view;
    view = EMU8086_APP_ERR_TEXT_VIEW(user_data);
    emu8086_app_err_text_view_change_color(view);
}

static void
emu8086_app_err_text_view_change_color(Emu8086AppErrTextView *view)
{
    PRIV_ERR_TEXT_VIEW;
    gchar *v = emu8086_app_style_scheme_get_color_by_index(priv->scheme, 12);
    PangoFontDescription *desc = pango_font_description_from_string(g_strdup(priv->font));

    gchar *m = emu8086_pango_font_description_to_css(desc, v);
    gtk_css_provider_load_from_data(priv->provider, m, -1, NULL);

    priv->color = v;

    pango_font_description_free(desc);
    // exit(1);
}
static void emu8086_app_err_text_view_init(Emu8086AppErrTextView *view)
{

    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), FALSE);
    GSimpleActionGroup *errtva = g_simple_action_group_new();
    g_action_map_add_action_entries(G_ACTION_MAP(errtva),
                                    err_text_view_entries, G_N_ELEMENTS(err_text_view_entries),
                                    view);

    gtk_widget_insert_action_group(GTK_WIDGET(view), "err_text_view", G_ACTION_GROUP(errtva));

    view->priv = emu8086_app_err_text_view_get_instance_private(view);
    PRIV_ERR_TEXT_VIEW;
    priv->popup = NULL;
    priv->attached = FALSE;
    GMenuModel *_menu;
    GtkBuilder *builder;
    builder = gtk_builder_new_from_resource("/com/krc/emu8086app/ui/errtv.ui");
    _menu = G_MENU_MODEL(gtk_builder_get_object(builder, "err_tv_menu"));

    priv->popup = GTK_MENU(gtk_menu_new_from_model(_menu));

    priv->gutter = emu8086_app_top_gutter_new(GTK_TEXT_VIEW(view), GTK_TEXT_WINDOW_RIGHT);
    GdkRGBA color;
    gtk_text_view_set_border_window_size(view,
                                         GTK_TEXT_WINDOW_LEFT,
                                         10);
    gtk_text_view_set_border_window_size(view,
                                         GTK_TEXT_WINDOW_TOP,
                                         20);
    priv->settings = g_settings_new("com.krc.emu8086app");
    priv->provider = GTK_STYLE_PROVIDER(gtk_css_provider_new());
    priv->scheme = emu8086_app_style_scheme_get_default();

    color.red = 0.22;
    color.green = 0.22;
    color.blue = 0.22;
    color.alpha = 1;
    gtk_style_context_add_provider(gtk_widget_get_style_context(view), priv->provider, G_MAXUINT);

    gtk_widget_override_background_color(view, GTK_STATE_NORMAL, &color);

    g_signal_connect(priv->scheme, "theme_changed", G_CALLBACK(_refresh_theme), view);
    g_settings_bind(priv->settings, "font", view, "font", G_SETTINGS_BIND_GET);
}

Emu8086AppErrTextView *emu8086_app_err_text_view_new(void)
{
    return g_object_new(EMU8086_APP_ERR_TEXT_VIEW_TYPE, NULL);
}

static gboolean
emu8086_app_err_text_view_draw(GtkWidget *widget,
                               cairo_t *cr)
{

    Emu8086AppErrTextView *view;
    view = EMU8086_APP_ERR_TEXT_VIEW(widget);
    PRIV_ERR_TEXT_VIEW;
    gboolean event_handled;
    event_handled = GTK_WIDGET_CLASS(emu8086_app_err_text_view_parent_class)->draw(widget, cr);

    if (priv->gutter != NULL)
    {
        top_draw(priv->gutter, cr);
    }
    return event_handled;
}

void emu8086_app_err_set_win(Emu8086AppErrTextView *view, Emu8086AppWindow *win)
{
    view->priv->window = win;
}

void emu8086_app_err_set_msgs(Emu8086AppErrTextView *view, gchar *msgs) {

}
