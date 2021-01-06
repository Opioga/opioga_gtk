
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
emu8086win.c \
emu8086app.c \
emu8086appprefs.c \
emu8086aboutwin.c \
main.c 

