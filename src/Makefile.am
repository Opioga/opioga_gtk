
# gsettings_SCHEMAS is a list of all the schemas you want to install
gsettings_SCHEMAS = com.krc.emu8086app.gschema.xml
 
# include the appropriate makefile rules for schema handling
@GSETTINGS_RULES@

EXTRA_DIST = com.krc.emu8086app.gschema.xml


bin_PROGRAMS = emu8086
AM_CFLAGS = $(EMU8086_CFLAGS)
emu8086_CFLAGS = -I $(srcdir)/../include $(EMU8086_CFLAGS)
emu8086_LDADD = $(EMU8086_LIBS)
emu8086_SOURCES = \
emu8086.c \
assembler.c \
ops.c \
resources.c \
pango_css.c \
code_buffer.c \
code.c \
emu8086win.c \
emu8086app.c \
emu8086appprefs.c \
emu8086aboutwin.c \
main.c 

