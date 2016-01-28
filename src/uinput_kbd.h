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

#ifndef UINPUT_KBD_H_
#define UINPUT_KBD_H_

#include <stdint.h>

typedef struct {
	int16_t fd;
} UINP_KBD_DEV;

int16_t uinput_kbd_open(UINP_KBD_DEV* const kbd);
int16_t uinput_kbd_close(UINP_KBD_DEV* const kbd);
int16_t uinput_kbd_write(UINP_KBD_DEV* const kbd, unsigned int keycode,
		int keyvalue, unsigned int evtype);
int16_t uinput_kbd_sleep();

#endif /* UINPUT_KBD_H_ */
