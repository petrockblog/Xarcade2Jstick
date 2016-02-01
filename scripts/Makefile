installservice:
	install -d $(DESTDIR)/lib/udev/rules.d/
	install -m 0644 80-xarcade.rules $(DESTDIR)/lib/udev/rules.d/
	install -d $(DESTDIR)/lib/systemd/system/
	install -m 0644 xarcade2jstick.service $(DESTDIR)/lib/systemd/system/
	udevadm trigger --action=add --subsystem-match=input

uninstallservice:
	-rm $(DESTDIR)/lib/udev/rules.d/80-xarcade.rules
	-rm $(DESTDIR)/lib/systemd/system/xarcade2jstick.service
