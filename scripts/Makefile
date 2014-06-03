installservice:
	install -m 0755 xarcade2jstick /etc/init.d
	update-rc.d xarcade2jstick defaults
	/etc/init.d/xarcade2jstick start

uninstallservice:
	/etc/init.d/xarcade2jstick stop
	update-rc.d xarcade2jstick remove
	-rm /etc/init.d/xarcade2jstick
