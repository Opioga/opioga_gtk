



#define LIBDIR "../lib"
#define DATADIR "../share"


#include <string.h>

#include <glib/gi18n.h>
#include <girepository.h>

#include <emu8086apppluginsengine.h>
#include <emu8086appmain.h>

struct _Emu8086PluginsEnginePrivate
{
	GSettings *plugin_settings;
};

G_DEFINE_TYPE_WITH_PRIVATE(Emu8086PluginsEngine, emu8086_plugins_engine, PEAS_TYPE_ENGINE)

Emu8086PluginsEngine *default_engine = NULL;

static void
emu8086_plugins_engine_init(Emu8086PluginsEngine *engine)
{
	gchar *private_path, *plugins_path;
	GError *error = NULL;
	g_print(g_get_user_data_dir());

	peas_engine_enable_loader(PEAS_ENGINE(engine), "python3");
	g_print("lion");
	engine->priv = emu8086_plugins_engine_get_instance_private(engine);

	engine->priv->plugin_settings = g_settings_new(EMU8086_SCHEMA_ID);

	/* This should be moved to libpeas */
	if (!g_irepository_require(g_irepository_get_default(),
							   "Peas", "1.0", 0, &error))
	{
		g_warning("Could not load Peas repository: %s", error->message);
		g_clear_error(&error);
	}

	if (!g_irepository_require(g_irepository_get_default(),
							   "PeasGtk", "1.0", 0, &error))
	{
		g_warning("Could not load PeasGtk repository: %s", error->message);
		g_clear_error(&error);
	}

	private_path = g_build_filename(LIBDIR, "girepository-1.0", NULL);

	if (!g_irepository_require_private(g_irepository_get_default(),
									   private_path, "Emu8086", "1.0", 0, &error))
	{
		g_warning("Could not load Emu8086 repository: %s", error->message);
		g_clear_error(&error);
	}

	g_free(private_path);

#ifdef EMU_DEBUG

	peas_engine_add_search_path(PEAS_ENGINE(engine),
								"/home/lion/Desktop/kosyWork/Desktop/gtkemu8086/plugins",
								"/home/lion/Desktop/kosyWork/Desktop/gtkemu8086/plugins");

#else
	plugins_path = g_build_filename(DATADIR, "emu8086/plugins", NULL);
	peas_engine_add_search_path(PEAS_ENGINE(engine),
								plugins_path,
								plugins_path);

	g_free(plugins_path);
#endif

	g_settings_bind (engine->priv->plugin_settings,
	                 "active-plugins",
	                 engine,
	                 "loaded-plugins",
	                 G_SETTINGS_BIND_DEFAULT);
}

static void
emu8086_plugins_engine_dispose(GObject *object)
{
	Emu8086PluginsEngine *engine = EMU8086_PLUGINS_ENGINE(object);

	if (engine->priv->plugin_settings != NULL)
	{
		g_object_unref(engine->priv->plugin_settings);
		engine->priv->plugin_settings = NULL;
	}

	G_OBJECT_CLASS(emu8086_plugins_engine_parent_class)->dispose(object);
}

static void
emu8086_plugins_engine_class_init(Emu8086PluginsEngineClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS(klass);

	object_class->dispose = emu8086_plugins_engine_dispose;
}

Emu8086PluginsEngine *
emu8086_plugins_engine_get_default(void)
{
	if (default_engine != NULL)
		return default_engine;

	default_engine = EMU8086_PLUGINS_ENGINE(g_object_new(EMU8086_TYPE_PLUGINS_ENGINE, NULL));
	g_object_add_weak_pointer(G_OBJECT(default_engine),
							  (gpointer)&default_engine);

	return default_engine;
}
