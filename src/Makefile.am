
# gsettings_SCHEMAS is a list of all the schemas you want to install
gsettings_SCHEMAS = com.krc.emu8086app.gschema.xml
 
# include the appropriate makefile rules for schema handling
@GSETTINGS_RULES@

EXTRA_DIST = com.krc.emu8086app.gschema.xml

lib_LTLIBRARIES = libemu8086.la
libemu8086_la_SOURCES = emu8086.c \
ops.c \
assembler.c
libemu8086_la_LDFLAGS = -version-info 1:0:0
libemu8086_la_CFLAGS = -I $(srcdir)/../include

bin_PROGRAMS = emu8086
AM_CFLAGS = $(EMU8086_CFLAGS)
emu8086_CFLAGS = -I $(srcdir)/../include $(EMU8086_CFLAGS)



emu8086_LDADD = $(EMU8086_LIBS) libemu8086.la

emu8086_SOURCES = \
resources.c \
pango_css.c \
code_buffer.c \
code_gutter.c \
code.c \
emu_8086_app_runner.c \
emu_plugin_box.c \
emu8086win.c \
emu_8086_plugins_engine.c \
emu8086stylescheme.c \
emu8086app.c \
emu8086appprefs.c \
emu8086aboutwin.c \
main.c 

if HAVE_INTROSPECTION
-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS = Emu8086-1.0.gir
INTROSPECTION_SCANNER_ARGS = -I$(top_srcdir)/include --warn-all

Emu8086-1.0.gir: emu8086
Emu8086_1_0_gir_INCLUDES = Gtk-3.0 
Emu8086_1_0_gir_FILES = $(top_srcdir)/include/emu8086app.h $(top_srcdir)/include/emu_plugin_box.h emu_plugin_box.c
Emu8086_1_0_gir_VERSION = 1.0
Emu8086_1_0_gir_PROGRAM = $(builddir)/emu8086
Emu8086_1_0_gir_NAMESPACE = Emu8086
Emu8086_1_0_gir_PREFIXES = emu_8086_app_window

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)
typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES = $(gir_DATA) $(typelib_DATA)

endif

dist-hook:
	cd $(distdir); rm -f $(BUILT_SOURCES)