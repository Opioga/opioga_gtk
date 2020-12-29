#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([emu8086], [1.0.1], [allisonkosy@gmail.com])
AC_CONFIG_SRCDIR([src/emu8086app.c])
AM_INIT_AUTOMAKE([-Wall foreign])

# Checks for programs.
AC_PROG_CC
PKG_PROG_PKG_CONFIG

# Checks for libraries.
PKG_CHECK_MODULES([EMU8086], [
    glib-2.0
    gtk+-3.0
])

GLIB_GSETTINGS
 

AC_OUTPUT([
Makefile
src/Makefile
include/Makefile
pixmaps/Makefile
pixbufs/Makefile
egs/Makefile
mime/Makefile
])
