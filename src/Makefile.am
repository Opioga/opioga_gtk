
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
emu8086_CFLAGS = -I $(srcdir)/../include $(EMU8086_CFLAGS) -DDATADIR=\""$(datadir)"\" \
-DLIBDIR=\""$(libdir)"\"



emu8086_LDADD = $(EMU8086_LIBS) libemu8086.la


emu8086app_c_files = \
emu8086appcodebuffer.c \
emu8086appcodegutter.c \
emu8086appcode.c \
emu8086apprunner.c \
emu8086apppluginbox.c \
emu8086appwin.c \
emu8086apppluginsengine.c \
emu8086stylescheme.c \
emu8086appuredomanager.c \
emu8086searchbar.c \
emu8086appsidepane.c \
emu8086app.c \
emu8086appprefs.c \
emu8086aboutwin.c 


emu8086_SOURCES = \
resources.c \
pango_css.c \
$(emu8086app_c_files) \
main.c 



INST_H_FILES =			\
    $(top_srcdir)/include/emu8086appcodebuffer.h \
    $(top_srcdir)/include/emu8086appcodegutter.h \
    $(top_srcdir)/include/emu8086appcode.h \
    $(top_srcdir)/include/emu8086apprunner.h \
    $(top_srcdir)/include/emu8086apppluginbox.h \
    $(top_srcdir)/include/emu8086appwin.h \
    $(top_srcdir)/include/emu8086stylescheme.h \
    $(top_srcdir)/include/emu8086appuredomanager.h \
    $(top_srcdir)/include/emu8086searchbar.h \
    $(top_srcdir)/include/emu8086app.h \
    $(top_srcdir)/include/emu8086appprefs.h \
    $(top_srcdir)/include/emu8086appsidepane.h \
    $(top_srcdir)/include/emu8086aboutwin.h 


if HAVE_INTROSPECTION
-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS = Emu8086-1.0.gir
INTROSPECTION_SCANNER_ARGS = -I$(top_srcdir)/include --warn-all



Emu8086-1.0.gir: emu8086
Emu8086_1_0_gir_INCLUDES = Gtk-3.0 
Emu8086_1_0_gir_FILES =  $(INST_H_FILES) $(emu8086app_c_files) 
Emu8086_1_0_gir_VERSION = 1.0
Emu8086_1_0_gir_PROGRAM = $(builddir)/emu8086
Emu8086_1_0_gir_NAMESPACE = Emu8086



girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)
typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES = $(gir_DATA) $(typelib_DATA)

endif

dist-hook:
	cd $(distdir); rm -f $(BUILT_SOURCES)
-include $(top_srcdir)/git.mk
