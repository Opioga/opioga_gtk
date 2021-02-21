SUBDIRS = src include egs pixmaps pixbufs mime plugins themes


desktopdir = $(datadir)/applications

update_desktop_database = update-desktop-database

dist_desktop_DATA = emu8086.desktop

install-data-hook: update-desktop-databases
uninstall-hook: update-desktop-databases

update-desktop-databases:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating desktop databases."; \
		$(update_desktop_database); \
	else \
	echo "*** Desktop database not updated. After (un)install, run this:"; \
	echo "*** $(update_desktop_database)"; \
	fi

MAINTAINERCLEANFILES =  \
 	.vscode	\
	aclocal.m4		\
	ar-lib \
	compile \
	config.guess		\
	config.h.in		\
	config.sub		\
	depcomp			\
	INSTALL \
	gtk-doc.make		\
	install-sh		\
	ltmain.sh		\
	missing			\
	mkinstalldirs		\
	omf.make		\
	py-compile		\
	xmldocs.make		\
	m4/gtk-doc.m4		\
	m4/libtool.m4		\
	m4/ltoptions.m4		\
	m4/ltsugar.m4		\
	m4/ltversion.m4		\
	m4/lt~obsolete.m4		\
	`find "$(srcdir)" -type f -name Makefile.in -print`


-include $(top_srcdir)/git.mk
