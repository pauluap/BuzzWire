/*
BuzzWire Project
Copyright (C) 2012  Paul Thompson (paul.r.thompson@gmail.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __BUZZWIRE_BSP_H__
#define __BUZZWIRE_BSP_H__

#include "lib44780fw/hd44780fw.h"

typedef enum
{
    BSP_OUTPUT_LED_GROUP0 = 0,
    BSP_OUTPUT_LED_GROUP1
} bsp_outputs_t;

typedef enum
{
    BSP_INPUT_BUZZ_LEFT_POST = 0,
    BSP_INPUT_BUZZ_RIGHT_POST,
    BSP_INPUT_BUZZ_WIRE
} bsp_inputs_t;

/**
 * @brief Configures the LCD driver struct hookups
 *
 * @param lcdfw Pointer to the display struct to hook up
 */
void BSP_ConfigureDisplay(struct hd44780_l_conf *lcdfw);

#endif /* __BUZZWIRE_BSP_H__ */

