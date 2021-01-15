


#ifndef __EMU8086_PLUGINS_ENGINE_H__
#define __EMU8086_PLUGINS_ENGINE_H__

#include <glib.h>
#include <libpeas/peas-engine.h>



G_BEGIN_DECLS

#define EMU8086_TYPE_PLUGINS_ENGINE              (emu8086_plugins_engine_get_type ())
#define EMU8086_PLUGINS_ENGINE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), EMU8086_TYPE_PLUGINS_ENGINE, Emu8086PluginsEngine))
#define EMU8086_PLUGINS_ENGINE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), EMU8086_TYPE_PLUGINS_ENGINE, Emu8086PluginsEngineClass))
#define EMU8086_IS_PLUGINS_ENGINE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), EMU8086_TYPE_PLUGINS_ENGINE))
#define EMU8086_IS_PLUGINS_ENGINE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), EMU8086_TYPE_PLUGINS_ENGINE))
#define EMU8086_PLUGINS_ENGINE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), EMU8086_TYPE_PLUGINS_ENGINE, Emu8086PluginsEngineClass))

typedef struct _Emu8086PluginsEngine		Emu8086PluginsEngine;
typedef struct _Emu8086PluginsEnginePrivate	Emu8086PluginsEnginePrivate;

struct _Emu8086PluginsEngine
{
	PeasEngine parent;
	Emu8086PluginsEnginePrivate *priv;
};

typedef struct _Emu8086PluginsEngineClass		Emu8086PluginsEngineClass;

struct _Emu8086PluginsEngineClass
{
	PeasEngineClass parent_class;
};

GType			 emu8086_plugins_engine_get_type		(void) G_GNUC_CONST;

Emu8086PluginsEngine	*emu8086_plugins_engine_get_default	(void);

G_END_DECLS

#endif  /* __EMU8086_PLUGINS_ENGINE_H__ */
