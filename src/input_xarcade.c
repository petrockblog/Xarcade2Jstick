#include "input_xarcade.h"

// declaration of supplementary functions  -------------------
int findXarcadeDevice(void);

// relizations ----------------------
int16_t input_xarcade_open(INP_XARC_DEV* const xdev, INPUT_XARC_TYPE_E type) {
	int result;

	// TODO check input parameter type
	xdev->fevdev = findXarcadeDevice();
	result = ioctl(xdev->fevdev, EVIOCGRAB, 1);
	return result;
}

int16_t input_xarcade_read  (INP_XARC_DEV* const xdev) {
	int rd;

	rd = read(xdev->fevdev, xdev->ev, sizeof(struct input_event) * 64);
	return (int) (rd / sizeof(struct input_event));
}

int16_t input_xarcade_close (INP_XARC_DEV* const xdev) {
	int result;

	result = ioctl(xdev->fevdev, EVIOCGRAB, 0);
	close(xdev->fevdev);
	return result;
}

// supplementary functions -------------------

int findXarcadeDevice(void) {
	char charbuffer[256];
	char name[256];
	int fevdev;
	int ctr;

	for (ctr = 0; ctr < 9; ++ctr) {
		snprintf(charbuffer, 256, "/dev/input/event%d", ctr);
		fevdev = open(charbuffer, O_RDONLY);
		if (fevdev == -1) {
			printf("Failed to open event device %d.\n", ctr);
			continue;
		}

		ioctl(fevdev, EVIOCGNAME(sizeof(name)), name);
		if (strcmp(name, "XGaming X-Arcade") == 0) {
			printf("Found %s (%s)\n", charbuffer, name);
			break;
		} else {
			close(fevdev);
		}
	}
	return fevdev;
}
