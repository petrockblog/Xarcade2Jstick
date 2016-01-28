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

#ifndef UINPUT_GAMEPAD_H_
#define UINPUT_GAMEPAD_H_

#include <stdint.h>

typedef enum {
	UINPUT_GPAD_TYPE_NES = 0,
	UINPUT_GPAD_TYPE_SNES = 1,
	UINPUT_GPAD_TYPE_XARCADE = 2
} UINPUT_GPAD_TYPE_E;

typedef struct {
	int fd;
	int16_t state;
} UINP_GPAD_DEV;

int16_t uinput_gpad_open(UINP_GPAD_DEV* const gpad, UINPUT_GPAD_TYPE_E type,
			 unsigned char number);
int16_t uinput_gpad_close(UINP_GPAD_DEV* const gpad);
int16_t uinput_gpad_write(UINP_GPAD_DEV* const gpad, uint16_t keycode,
		int16_t keyvalue, uint16_t evtype);
int16_t uinput_gpad_sleep();

#endif /* UINPUT_GAMEPAD_H_ */
