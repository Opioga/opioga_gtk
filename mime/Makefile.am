mimedir = /usr/share/mime/packages/
mime_DATA = x-asm.xml
EXTRA_DIST = $(mime_DATA)

update_mime_database = update-mime-database /usr/share/mime

install-data-hook: update-mime-databases
uninstall-hook: update-mime-databases

update-mime-databases: 

	@-if test -z "$(DESTDIR)"; then \
		echo "Updating mime databases."; \
		$(update_mime_database); \
	else \
	echo "*** Mime database not updated. After (un)install, run this:"; \
	echo "*** $(update_mime_database)"; \
	fi
