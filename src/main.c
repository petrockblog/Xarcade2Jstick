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
/*                 Copyright (c) 2014-2019, Florian Mueller                 */
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

void outputKeyPress(short keyPad, int keyCode, int state) {
	 uinput_gpad_write(&uinp_gpads[ keyPad ], keyCode, state, EV_KEY);
}

void outputAxisChange(short keyPad, int axisCode, int value) {
	 uinput_gpad_write(&uinp_gpads[ keyPad ], axisCode, value, EV_ABS);
}

int main(int argc, char* argv[]) {
	int rd, ctr, comboP1, comboP2 = 0;
	char keyStates[256];
	memset(keyStates, 0, sizeof(keyStates));
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
	int retry_time=3;
	while ( input_xarcade_open(&xarcdev, INPUT_XARC_TYPE_TANKSTICK) != 0) {
		if (errno == 0) {
			printf("Not found.\n");
			SYSLOG(LOG_ERR, "Xarcade not found, exiting.");
		} else {
			printf("Failed to get exclusive access to Xarcade: %d (%s)\n", errno, strerror(errno));
			SYSLOG(LOG_ERR, "Failed to get exclusive access to Xarcade, exiting: %d (%s)", errno, strerror(errno));
		}
		printf("Retrying in %ds.\n", retry_time);
		sleep( retry_time );
		retry_time *= 2;
		if (retry_time>30) retry_time=30;
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

				int value = xarcdev.ev[ctr].value;
				int isPressed = value > 0;
				switch (xarcdev.ev[ctr].code) {

				/* ----------------  Player 1 controls ------------------- */
				/* buttons */
				case KEY_LEFTCTRL:
					/* P1Start + P1Button0 = P1Select */
					if (keyStates[KEY_1])
					{
						outputKeyPress(0,BTN_SELECT, isPressed);
						comboP1 = 1;
					}
					else
					{
						outputKeyPress(0,BTN_A, isPressed);
					}
					break;
				case KEY_LEFTALT:
					outputKeyPress(0,BTN_B, isPressed);
					break;
				case KEY_SPACE:
					outputKeyPress(0,BTN_C, isPressed);
					break;
				case KEY_LEFTSHIFT:
					outputKeyPress(0,BTN_X, isPressed);
					break;
				case KEY_Z:
					outputKeyPress(0,BTN_Y, isPressed);
					break;
				case KEY_X:
					outputKeyPress(0,BTN_Z, isPressed);
					break;
				case KEY_C:
					outputKeyPress(0,BTN_TL, isPressed);
					break;
				case KEY_5:
					outputKeyPress(0,BTN_TR, isPressed);
					break;
				case KEY_1:
					/* Do nothing on key down because it could be a combo */
					if (isPressed)
						continue;

					if (!comboP1)
					{
						outputKeyPress(0,BTN_START, 1);
						uinput_gpad_sleep();
						outputKeyPress(0,BTN_START, 0);
					}
					else
					{
						/* Ensure P1Select is released it could get stuck if start is released before BTN_A */
						outputKeyPress(0,BTN_SELECT, 0);
						comboP1 = 0;
					}
					break;
				case KEY_3:
					outputKeyPress(0,BTN_SELECT, isPressed);
					break;
				case KEY_KP4:
				case KEY_LEFT:
					outputAxisChange(0,ABS_X, value == 0 ? 2 : 0); // center or left
					break;
				case KEY_KP6:
				case KEY_RIGHT:
					/* P1Start + P1Right = Tab */
					if (keyStates[KEY_1])
					{
						uinput_kbd_write(&uinp_kbd, KEY_TAB, isPressed, EV_KEY);
						comboP1 = 1;
					}
					else
					{
						outputAxisChange(0,ABS_X,	value == 0 ? 2 : 4); // center or right
					}
					break;
				case KEY_KP8:
				case KEY_UP:
					outputAxisChange(0,ABS_Y,	value == 0 ? 2 : 0); // center or up
					break;
				case KEY_KP2:
				case KEY_DOWN:
					outputAxisChange(0,ABS_Y,	value == 0 ? 2 : 4); // center or down
					break;

					/* ----------------  Player 2 controls ------------------- */
					/* buttons */
				case KEY_A:
					/* P2Start + P2Button0 = P2Select */
					if (keyStates[KEY_2])
					{
						outputKeyPress(1,BTN_SELECT, isPressed);
						comboP2 = 1;
					}
					else
					{

						outputKeyPress(1,BTN_A, isPressed);
					}
					break;
				case KEY_S:
					outputKeyPress(1,BTN_B, isPressed);
					break;
				case KEY_Q:
					outputKeyPress(1,BTN_C, isPressed);
					break;
				case KEY_W:
					outputKeyPress(1,BTN_X, isPressed);
					break;
				case KEY_E:
					outputKeyPress(1,BTN_Y, isPressed);
					break;
				case KEY_LEFTBRACE:
					outputKeyPress(1,BTN_Z, isPressed);
					break;
				case KEY_RIGHTBRACE:
					outputKeyPress(1,BTN_TL, isPressed);
					break;
				case KEY_6:
					outputKeyPress(1,BTN_TR, isPressed);
					break;
				case KEY_2:
					/* P1Start + P2Start = ESC */
					if (keyStates[KEY_1])
					{
						uinput_kbd_write(&uinp_kbd, KEY_ESC, isPressed, EV_KEY);
						comboP1 = 1;
					}
					else
					{
						/* Do nothing on key down because it could be a combo */
						if (isPressed)
							continue;

						if (!comboP2)
						{
							outputKeyPress(1,BTN_START, 1);
							uinput_gpad_sleep();
							outputKeyPress(1,BTN_START, 0);
						}
						else
						{
							/* Ensure P2Select is released it could get stuck if start is released before BTN_A */
							outputKeyPress(1,BTN_SELECT, 0);
							comboP2 = 0;
						}
					}
					break;
				case KEY_4:
					outputKeyPress(1,BTN_SELECT, isPressed);
					break;

					/* joystick */
				case KEY_D:
					outputAxisChange(1,ABS_X, value == 0 ? 2 : 0); // center or left
					break;
				case KEY_G:
					outputAxisChange(1,ABS_X, value == 0 ? 2 : 4); // center or right
					break;
				case KEY_R:
					outputAxisChange(1,ABS_Y, value == 0 ? 2 : 0); // center or up
					break;
				case KEY_F:
					outputAxisChange(1,ABS_Y,value == 0 ? 2 : 4); // center or down
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
