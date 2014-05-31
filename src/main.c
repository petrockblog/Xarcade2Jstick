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
#include "uinput_gamepad.h"
#include "uinput_kbd.h"

// TODO Extract all magic numbers and collect them as defines in at a central location

#define GPADSNUM 2

UINP_KBD_DEV uinp_kbd;
UINP_GPAD_DEV uinp_gpads[GPADSNUM];

int findXarcadeDevice(int result, int fevdev) {

	char charbuffer[256];
	char name[256];
	int ctr;

	for (ctr = 0; ctr < 9; ++ctr) {
		snprintf(charbuffer, 256, "/dev/input/event%d", ctr);
		fevdev = open(charbuffer, O_RDONLY);
		if (fevdev == -1) {
			printf("Failed to open event device %d.\n", ctr);
			continue;
		}

		result = ioctl(fevdev, EVIOCGNAME(sizeof(name)), name);
		if (strcmp(name, "XGaming X-Arcade") == 0) {
			printf("Found %s (%s)\n", charbuffer, name);
			break;
		} else {
			close(fevdev);
		}
	}
	return fevdev;
}

int main(int argc, char* argv[]) {
	struct input_event ev[64];
	int fevdev = -1;
	int result = 0;
	int size = sizeof(struct input_event);
	int rd, ctr;
	char keyStates[256];

	fevdev = findXarcadeDevice(result, fevdev);

	printf("Getting exclusive access: ");
	result = ioctl(fevdev, EVIOCGRAB, 1);
	printf("%s\n", (result == 0) ? "SUCCESS" : "FAILURE");
	if (result != 0) {
		exit(-1);
	}

	uinput_gpad_open(&uinp_gpads[0], UINPUT_GPAD_TYPE_XARCADE);
	uinput_gpad_open(&uinp_gpads[1], UINPUT_GPAD_TYPE_XARCADE);
	uinput_kbd_open(&uinp_kbd);

	while (1) {
		if ((rd = read(fevdev, ev, size * 64)) < size) {
			break;
		}

		for (ctr = 0; ctr < (int) (rd / sizeof(struct input_event)); ctr++) {
			if (ev[ctr].type == 0)
				continue;
			if (ev[ctr].type == EV_MSC)
				continue;
			if (EV_KEY == ev[ctr].type) {
//				printf("type %d code %d value %d\n", ev[ctr].type, ev[ctr].code,
//						ev[ctr].value);

				keyStates[ev[ctr].code] = ev[ctr].value;

				switch (ev[ctr].code) {

				/* ----------------  Player 1 controls ------------------- */
				/* buttons */
				case 29:
					uinput_gpad_write(&uinp_gpads[0], BTN_A, ev[ctr].value > 0,
							EV_KEY);
					break;
				case 56:
					uinput_gpad_write(&uinp_gpads[0], BTN_B, ev[ctr].value > 0,
							EV_KEY);
					break;
				case 57:
					uinput_gpad_write(&uinp_gpads[0], BTN_C, ev[ctr].value > 0,
							EV_KEY);
					break;
				case 42:
					uinput_gpad_write(&uinp_gpads[0], BTN_X, ev[ctr].value > 0,
							EV_KEY);
					break;
				case 44:
					uinput_gpad_write(&uinp_gpads[0], BTN_Y, ev[ctr].value > 0,
							EV_KEY);
					break;
				case 45:
					uinput_gpad_write(&uinp_gpads[0], BTN_Z, ev[ctr].value > 0,
							EV_KEY);
					break;
				case 46:
					uinput_gpad_write(&uinp_gpads[0], BTN_TL, ev[ctr].value > 0,
							EV_KEY);
					break;
				case 6:
					uinput_gpad_write(&uinp_gpads[0], BTN_TR, ev[ctr].value > 0,
							EV_KEY);
					break;
				case 2:
					uinput_gpad_write(&uinp_gpads[0], BTN_START,
							ev[ctr].value > 0, EV_KEY);
					break;
				case 4:
					uinput_gpad_write(&uinp_gpads[0], BTN_SELECT,
							ev[ctr].value > 0, EV_KEY);
					break;

					/* joystick */
				case 75:
					uinput_gpad_write(&uinp_gpads[0], ABS_X,
							ev[ctr].value == 0 ? 2 : 0, EV_ABS); // center or left
					break;
				case 77:
					uinput_gpad_write(&uinp_gpads[0], ABS_X,
							ev[ctr].value == 0 ? 2 : 4, EV_ABS); // center or right
					break;
				case 72:
					uinput_gpad_write(&uinp_gpads[0], ABS_Y,
							ev[ctr].value == 0 ? 2 : 0, EV_ABS); // center or up
					break;
				case 80:
					uinput_gpad_write(&uinp_gpads[0], ABS_Y,
							ev[ctr].value == 0 ? 2 : 4, EV_ABS); // center or down
					break;

					/* ----------------  Player 2 controls ------------------- */
					/* buttons */
				case 30:
					uinput_gpad_write(&uinp_gpads[1], BTN_A, ev[ctr].value > 0,
							EV_KEY);
					break;
				case 31:
					uinput_gpad_write(&uinp_gpads[1], BTN_B, ev[ctr].value > 0,
							EV_KEY);
					break;
				case 16:
					uinput_gpad_write(&uinp_gpads[1], BTN_C, ev[ctr].value > 0,
							EV_KEY);
					break;
				case 17:
					uinput_gpad_write(&uinp_gpads[1], BTN_X, ev[ctr].value > 0,
							EV_KEY);
					break;
				case 18:
					uinput_gpad_write(&uinp_gpads[1], BTN_Y, ev[ctr].value > 0,
							EV_KEY);
					break;
				case 26:
					uinput_gpad_write(&uinp_gpads[1], BTN_Z, ev[ctr].value > 0,
							EV_KEY);
					break;
				case 27:
					uinput_gpad_write(&uinp_gpads[1], BTN_TL, ev[ctr].value > 0,
							EV_KEY);
					break;
				case 7:
					uinput_gpad_write(&uinp_gpads[1], BTN_TR, ev[ctr].value > 0,
							EV_KEY);
					break;
				case 3:
					uinput_gpad_write(&uinp_gpads[1], BTN_START,
							ev[ctr].value > 0, EV_KEY);
					break;
				case 5:
					uinput_gpad_write(&uinp_gpads[1], BTN_SELECT,
							ev[ctr].value > 0, EV_KEY);
					break;

					/* joystick */
				case 32:
					uinput_gpad_write(&uinp_gpads[1], ABS_X,
							ev[ctr].value == 0 ? 2 : 0, EV_ABS); // center or left
					break;
				case 34:
					uinput_gpad_write(&uinp_gpads[1], ABS_X,
							ev[ctr].value == 0 ? 2 : 4, EV_ABS); // center or right
					break;
				case 19:
					uinput_gpad_write(&uinp_gpads[1], ABS_Y,
							ev[ctr].value == 0 ? 2 : 0, EV_ABS); // center or up
					break;
				case 33:
					uinput_gpad_write(&uinp_gpads[1], ABS_Y,
							ev[ctr].value == 0 ? 2 : 4, EV_ABS); // center or down
					break;

				default:
					break;
				}

				/* button combinations */
				if (keyStates[2] == 2 && keyStates[4] > 0) { // TAB key
					uinput_kbd_write(&uinp_kbd, KEY_TAB, 1, EV_KEY);
				} else {
					uinput_kbd_write(&uinp_kbd, KEY_TAB, 0, EV_KEY);
				}
				if (keyStates[3] == 2 && keyStates[5] > 0) { // ESC key
					uinput_kbd_write(&uinp_kbd, KEY_ESC, 1, EV_KEY);
				} else {
					uinput_kbd_write(&uinp_kbd, KEY_ESC, 0, EV_KEY);
				}
			}
		}

	}

	printf("Exiting.\n");
	result = ioctl(fevdev, EVIOCGRAB, 0);
	close(fevdev);
	uinput_gpad_close(&uinp_gpads[0]);
	uinput_gpad_close(&uinp_gpads[1]);
	uinput_kbd_close(&uinp_kbd);
	return 0;
}

