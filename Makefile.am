SUBDIRS = src


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