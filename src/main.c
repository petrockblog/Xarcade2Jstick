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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <termios.h>
#include <signal.h>
#include <time.h>
#include <syslog.h>

#include "uinput_gamepad.h"
#include "uinput_kbd.h"
#include "input_xarcade.h"

// TODO Extract all magic numbers and collect them as defines in at a central location

#define GPADSNUM 2

UINP_KBD_DEV uinp_kbd;
UINP_GPAD_DEV uinp_gpads[GPADSNUM];
INP_XARC_DEV xarcdev;
int use_syslog = 0;

#define SYSLOG(...) if (use_syslog == 1) { syslog(__VA_ARGS__); }

static void teardown();
static void signal_handler(int signum);

int main(int argc, char* argv[]) {
	int result = 0;
	int rd, ctr, combo = 0;
	char keyStates[256];

	int detach = 0;
	int opt;
	while ((opt = getopt(argc, argv, "+ds")) != -1) {
		switch (opt) {
			case 'd':
				detach = 1;
				break;
			case 's':
				use_syslog = 1;
				break;
			default:
				fprintf(stderr, "Usage: %s [-d] [-s]\n", argv[0]);
				exit(EXIT_FAILURE);
				break;
		}
	}

	SYSLOG(LOG_NOTICE, "Starting.");

	printf("[Xarcade2Joystick] Getting exclusive access: ");
	result = input_xarcade_open(&xarcdev, INPUT_XARC_TYPE_TANKSTICK);
	if (result != 0) {
		if (errno == 0) {
			printf("Not found.\n");
			SYSLOG(LOG_ERR, "Xarcade not found, exiting.");
		} else {
			printf("Failed to get exclusive access to Xarcade: %d (%s)\n", errno, strerror(errno));
			SYSLOG(LOG_ERR, "Failed to get exclusive access to Xarcade, exiting: %d (%s)", errno, strerror(errno));
		}
		exit(EXIT_FAILURE);
	}

	SYSLOG(LOG_NOTICE, "Got exclusive access to Xarcade.");

	uinput_gpad_open(&uinp_gpads[0], UINPUT_GPAD_TYPE_XARCADE, 1);
	uinput_gpad_open(&uinp_gpads[1], UINPUT_GPAD_TYPE_XARCADE, 2);
	uinput_kbd_open(&uinp_kbd);

	if (detach) {
		if (daemon(0, 1)) {
			perror("daemon");
			return 1;
		}
	}
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);

	SYSLOG(LOG_NOTICE, "Running.");

	while (1) {
		rd = input_xarcade_read(&xarcdev);
		if (rd < 0) {
			break;
		}

		for (ctr = 0; ctr < rd; ctr++) {
			if (xarcdev.ev[ctr].type == 0)
				continue;
			if (xarcdev.ev[ctr].type == EV_MSC)
				continue;
			if (EV_KEY == xarcdev.ev[ctr].type) {

				keyStates[xarcdev.ev[ctr].code] = xarcdev.ev[ctr].value;

				switch (xarcdev.ev[ctr].code) {

				/* ----------------  Player 1 controls ------------------- */
				/* buttons */
				case KEY_LEFTCTRL:
					uinput_gpad_write(&uinp_gpads[0], BTN_A,
							xarcdev.ev[ctr].value > 0, EV_KEY);
					break;
				case KEY_LEFTALT:
					uinput_gpad_write(&uinp_gpads[0], BTN_B,
							xarcdev.ev[ctr].value > 0, EV_KEY);
					break;
				case KEY_SPACE:
					uinput_gpad_write(&uinp_gpads[0], BTN_C,
							xarcdev.ev[ctr].value > 0, EV_KEY);
					break;
				case KEY_LEFTSHIFT:
					uinput_gpad_write(&uinp_gpads[0], BTN_X,
							xarcdev.ev[ctr].value > 0, EV_KEY);
					break;
				case KEY_Z:
					uinput_gpad_write(&uinp_gpads[0], BTN_Y,
							xarcdev.ev[ctr].value > 0, EV_KEY);
					break;
				case KEY_X:
					uinput_gpad_write(&uinp_gpads[0], BTN_Z,
							xarcdev.ev[ctr].value > 0, EV_KEY);
					break;
				case KEY_C:
					uinput_gpad_write(&uinp_gpads[0], BTN_TL,
							xarcdev.ev[ctr].value > 0, EV_KEY);
					break;
				case KEY_5:
					uinput_gpad_write(&uinp_gpads[0], BTN_TR,
							xarcdev.ev[ctr].value > 0, EV_KEY);
					break;
				case KEY_1:
					uinput_gpad_write(&uinp_gpads[0], BTN_START,
							xarcdev.ev[ctr].value > 0, EV_KEY);
					break;
				case KEY_3:
					uinput_gpad_write(&uinp_gpads[0], BTN_SELECT,
							xarcdev.ev[ctr].value > 0, EV_KEY);
				case KEY_KP4:
				case KEY_LEFT:
					uinput_gpad_write(&uinp_gpads[0], ABS_X,
							xarcdev.ev[ctr].value == 0 ? 2 : 0, EV_ABS); // center or left
					break;
				case KEY_KP6:
				case KEY_RIGHT:
					uinput_gpad_write(&uinp_gpads[0], ABS_X,
							xarcdev.ev[ctr].value == 0 ? 2 : 4, EV_ABS); // center or right
					break;
				case KEY_KP8:
				case KEY_UP:
					uinput_gpad_write(&uinp_gpads[0], ABS_Y,
							xarcdev.ev[ctr].value == 0 ? 2 : 0, EV_ABS); // center or up
					break;
				case KEY_KP2:
				case KEY_DOWN:
					uinput_gpad_write(&uinp_gpads[0], ABS_Y,
							xarcdev.ev[ctr].value == 0 ? 2 : 4, EV_ABS); // center or down
					break;

					/* ----------------  Player 2 controls ------------------- */
					/* buttons */
				case KEY_A:
					uinput_gpad_write(&uinp_gpads[1], BTN_A,
							xarcdev.ev[ctr].value > 0, EV_KEY);
					break;
				case KEY_S:
					uinput_gpad_write(&uinp_gpads[1], BTN_B,
							xarcdev.ev[ctr].value > 0, EV_KEY);
					break;
				case KEY_Q:
					uinput_gpad_write(&uinp_gpads[1], BTN_C,
							xarcdev.ev[ctr].value > 0, EV_KEY);
					break;
				case KEY_W:
					uinput_gpad_write(&uinp_gpads[1], BTN_X,
							xarcdev.ev[ctr].value > 0, EV_KEY);
					break;
				case KEY_E:
					uinput_gpad_write(&uinp_gpads[1], BTN_Y,
							xarcdev.ev[ctr].value > 0, EV_KEY);
					break;
				case KEY_LEFTBRACE:
					uinput_gpad_write(&uinp_gpads[1], BTN_Z,
							xarcdev.ev[ctr].value > 0, EV_KEY);
					break;
				case KEY_RIGHTBRACE:
					uinput_gpad_write(&uinp_gpads[1], BTN_TL,
							xarcdev.ev[ctr].value > 0, EV_KEY);
					break;
				case KEY_6:
					uinput_gpad_write(&uinp_gpads[1], BTN_TR,
							xarcdev.ev[ctr].value > 0, EV_KEY);
					break;
				case KEY_2:
					/* handle combination */
					if (keyStates[KEY_4] && xarcdev.ev[ctr].value) {
						uinput_kbd_write(&uinp_kbd, KEY_TAB, 1, EV_KEY);
						uinput_kbd_sleep();
						uinput_kbd_write(&uinp_kbd, KEY_TAB, 0, EV_KEY);
						combo = 2;
						continue;
					}
					/* it's a key down, ignore */
					if (xarcdev.ev[ctr].value)
						continue;
					if (!combo) {
						uinput_gpad_write(&uinp_gpads[1], BTN_START, 1, EV_KEY);
						uinput_gpad_sleep();
						uinput_gpad_write(&uinp_gpads[1], BTN_START, 0, EV_KEY);
					} else
						combo--;
					break;
				case KEY_4:
					/* it's a key down, ignore */
					if (xarcdev.ev[ctr].value)
						continue;
					if (!combo) {
						uinput_gpad_write(&uinp_gpads[1], BTN_SELECT, 1, EV_KEY);
						uinput_gpad_sleep();
						uinput_gpad_write(&uinp_gpads[1], BTN_SELECT, 0, EV_KEY);
					} else
						combo--;

					break;

					/* joystick */
				case KEY_D:
					uinput_gpad_write(&uinp_gpads[1], ABS_X,
							xarcdev.ev[ctr].value == 0 ? 2 : 0, EV_ABS); // center or left
					break;
				case KEY_G:
					uinput_gpad_write(&uinp_gpads[1], ABS_X,
							xarcdev.ev[ctr].value == 0 ? 2 : 4, EV_ABS); // center or right
					break;
				case KEY_R:
					uinput_gpad_write(&uinp_gpads[1], ABS_Y,
							xarcdev.ev[ctr].value == 0 ? 2 : 0, EV_ABS); // center or up
					break;
				case KEY_F:
					uinput_gpad_write(&uinp_gpads[1], ABS_Y,
							xarcdev.ev[ctr].value == 0 ? 2 : 4, EV_ABS); // center or down
					break;

				default:
					break;
				}
			}
		}
	}

	teardown();
	return EXIT_SUCCESS;
}

static void teardown() {
	printf("Exiting.\n");
	SYSLOG(LOG_NOTICE, "Exiting.");
	
	input_xarcade_close(&xarcdev);
	uinput_gpad_close(&uinp_gpads[0]);
	uinput_gpad_close(&uinp_gpads[1]);
	uinput_kbd_close(&uinp_kbd);
}

static void signal_handler(int signum) {
	signal(signum, SIG_DFL);

	printf("Received signal %d (%s), exiting.\n", signum, strsignal(signum));
	SYSLOG(LOG_NOTICE, "Received signal %d (%s), exiting.", signum, strsignal(signum));
	teardown();
	exit(EXIT_SUCCESS);
}
