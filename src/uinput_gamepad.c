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

#include "uinput_gamepad.h"

/* sends a key event to the virtual device */
static void send_key_event(int fd, unsigned int keycode, int keyvalue,
		unsigned int evtype) {
	struct input_event event;
	gettimeofday(&event.time, NULL);

	event.type = evtype;
	event.code = keycode;
	event.value = keyvalue;

	if (write(fd, &event, sizeof(event)) < 0) {
		printf("[uinput_gamepad] Simulate key error\n");
	}

	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(fd, &event, sizeof(event));
	if (write(fd, &event, sizeof(event)) < 0) {
		printf("[uinput_gamepad] Simulate key error\n");
	}
}

/* Setup the uinput device */
int16_t uinput_gpad_open(UINP_GPAD_DEV* const gpad, UINPUT_GPAD_TYPE_E type,
			 unsigned char number) {
	int16_t uinp_fd = -1;
	gpad->fd = open("/dev/uinput", O_WRONLY | O_NDELAY);
	if (gpad->fd <= 0) {
		printf("Unable to open /dev/uinput\n");
		return -1;
	}

	struct uinput_user_dev uinp;
	memset(&uinp, 0, sizeof(uinp));
	snprintf(uinp.name, sizeof(uinp.name), "Xarcade-to-Gamepad Device %i", number);
	uinp.id.version = 4;
	uinp.id.bustype = BUS_USB;
	uinp.id.product = 1;
	uinp.id.vendor = 1;

	// Setup the uinput device
	ioctl(gpad->fd, UI_SET_EVBIT, EV_KEY);
	ioctl(gpad->fd, UI_SET_EVBIT, EV_REL);

	// gamepad, buttons
	ioctl(gpad->fd, UI_SET_KEYBIT, BTN_A);
	ioctl(gpad->fd, UI_SET_KEYBIT, BTN_B);
	ioctl(gpad->fd, UI_SET_KEYBIT, BTN_C);
	ioctl(gpad->fd, UI_SET_KEYBIT, BTN_X);
	ioctl(gpad->fd, UI_SET_KEYBIT, BTN_Y);
	ioctl(gpad->fd, UI_SET_KEYBIT, BTN_Z);
	ioctl(gpad->fd, UI_SET_KEYBIT, BTN_TL);
	ioctl(gpad->fd, UI_SET_KEYBIT, BTN_TR);
	ioctl(gpad->fd, UI_SET_KEYBIT, BTN_SELECT);
	ioctl(gpad->fd, UI_SET_KEYBIT, BTN_START);

	// gamepad, directions
	ioctl(gpad->fd, UI_SET_EVBIT, EV_ABS);
	ioctl(gpad->fd, UI_SET_ABSBIT, ABS_X);
	ioctl(gpad->fd, UI_SET_ABSBIT, ABS_Y);
	uinp.absmin[ABS_X] = 0;
	uinp.absmax[ABS_X] = 4;
	uinp.absmin[ABS_Y] = 0;
	uinp.absmax[ABS_Y] = 4;

	/* Create input device into input sub-system */
	write(gpad->fd, &uinp, sizeof(uinp));
	if (ioctl(gpad->fd, UI_DEV_CREATE)) {
		printf("[uinput_gamepad] Unable to create UINPUT device.");
		return -1;
	}

	send_key_event(gpad->fd, ABS_X, 2, EV_ABS);
	send_key_event(gpad->fd, ABS_Y, 2, EV_ABS);

	return uinp_fd;
}

int16_t uinput_gpad_close(UINP_GPAD_DEV* const gpad) {
	ioctl(gpad->fd, UI_DEV_DESTROY);
	return close(gpad->fd);
}

/* sends a key event to the virtual device */
int16_t uinput_gpad_write(UINP_GPAD_DEV* const gpad, uint16_t keycode,
		int16_t keyvalue, uint16_t evtype) {
	struct input_event event;
	gettimeofday(&event.time, NULL);

	event.type = evtype;
	event.code = keycode;
	event.value = keyvalue;

	if (write(gpad->fd, &event, sizeof(event)) < 0) {
		printf("[uinput_gamepad] Simulate key error\n");
	}

	event.type = EV_SYN;
	event.code = SYN_REPORT;
	event.value = 0;
	write(gpad->fd, &event, sizeof(event));
	if (write(gpad->fd, &event, sizeof(event)) < 0) {
		printf("[uinput_gamepad] Simulate key error\n");
	}
	return 0;
}

/* sleep between immediate successive gpad writes */
int16_t uinput_gpad_sleep() {
	usleep(50000);
	return 0;
}
