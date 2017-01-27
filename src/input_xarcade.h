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

#ifndef INPUT_XARCADE_H_
#define INPUT_XARCADE_H_

#include <stdio.h>
#include <stdint.h>
#include <linux/input.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

typedef enum {
	INPUT_XARC_TYPE_TANKSTICK = 0
} INPUT_XARC_TYPE_E;

typedef struct {
	int fevdev;
	struct input_event ev[64];
} INP_XARC_DEV;

int16_t input_xarcade_open(INP_XARC_DEV* const xdev, INPUT_XARC_TYPE_E type, char* evedv);
int16_t input_xarcade_close(INP_XARC_DEV* const xdev);
int16_t input_xarcade_read(INP_XARC_DEV* const xdev);

#endif /* INPUT_XARCADE_H_ */
