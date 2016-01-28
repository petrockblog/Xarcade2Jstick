/* ======================================================================== */
/*  This program is free software; you can redistribute it and/or modify    */
/*  it under the terms of the GNU General Public License as published by    */
/*  the Free Software Foundation; either version 2 of the License, or       */
/*  (at your option) any later version.                                     */
/*                                                                          */
/*  This program is distributed in the hope that it will be useful,         */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of          */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       */
/*  General Public License for more details.                                */
/*                                                                          */
/*  You should have received a copy of the GNU General Public License       */
/*  along with this program; if not, write to the Free Software             */
/*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.               */
/* ======================================================================== */
/*                 Copyright (c) 2014, Florian Mueller                      */
/* ======================================================================== */

#include <linux/input.h>
#include <linux/uinput.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "uinput_kbd.h"

/* Setup the uinput keyboard device */
int16_t uinput_kbd_open(UINP_KBD_DEV* const kbd) {
	kbd->fd = open("/dev/uinput", O_WRONLY | O_NDELAY);
	if (kbd->fd == 0) {
		printf("Unable to open /dev/uinput\n");
		return -1;
	}

	struct uinput_user_dev uinp;
	memset(&uinp, 0, sizeof(uinp));
	strncpy(uinp.name, "SNES-to-Keyboard Device",
			strlen("SNES-to-Keyboard Device"));
	uinp.id.version = 4;
	uinp.id.bustype = BUS_USB;
	uinp.id.product = 1;
	uinp.id.vendor = 1;

	// Setup the uinput device
	// keyboard
	ioctl(kbd->fd, UI_SET_EVBIT, EV_KEY);
	ioctl(kbd->fd, UI_SET_EVBIT, EV_REL);
	int i = 0;
	for (i = 0; i < 256; i++) {
		ioctl(kbd->fd, UI_SET_KEYBIT, i);
	}

	/* Create input device into input sub-system */
	write(kbd->fd, &uinp, sizeof(uinp));
	if (ioctl(kbd->fd, UI_DEV_CREATE)) {
		printf("[SNESDev-Rpi] Unable to create UINPUT device.");
		return -1;
	}

	return kbd->fd;
}

int16_t uinput_kbd_close(UINP_KBD_DEV* const kbd) {
	ioctl(kbd->fd, UI_DEV_DESTROY);
	return close(kbd->fd);
}

/* sends a key event to the virtual device */
int16_t uinput_kbd_write(UINP_KBD_DEV* const kbd, unsigned int keycode,
		int keyvalue, unsigned int evtype) {
	struct input_event event;
	gettimeofday(&event.time, NULL);

	event.type = evtype;
	event.code = keycode;
	event.value = keyvalue;

	if (write(kbd->fd, &event, sizeof(event)) < 0) {
		printf("[SNESDev-Rpi] Simulate key error\n");
	}

	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(kbd->fd, &event, sizeof(event));
	if (write(kbd->fd, &event, sizeof(event)) < 0) {
		printf("[SNESDev-Rpi] Simulate key error\n");
	}
	return 0;
}

/* sleep between immediate successive keyboard writes */
int16_t uinput_kbd_sleep() {
	usleep(50000);
	return 0;
}
