installservice:
	install -m 0755 Xarcade2Joystick /etc/init.d
	update-rc.d Xarcade2Joystick defaults
	/etc/init.d/Xarcade2Joystick start

uninstallservice:
	/etc/init.d/Xarcade2Joystick stop
	update-rc.d Xarcade2Joystick remove
	-rm /etc/init.d/Xarcade2Joystick
