fontsdir = $(prefix)/share/fonts/Digital

fonts_DATA = *.ttf

EXTRA_DIST = $(fonts_DATA)

gtk_update_font_cache= fc-cache -f -v

install-data-hook: update-font-cache
uninstall-hook: update-font-cache

update-font-cache:
	@-if test -z "$(DESTDIR)"; then \
		echo "Updating GTK font cache."; \
		$(gtk_update_font_cache); \
	else \
	echo "*** Font cache not updated. After (un)install, run this:"; \
	echo "*** $(gtk_update_font_cache)"; \
	fi

