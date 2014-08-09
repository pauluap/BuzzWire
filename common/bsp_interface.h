/*
Microcontroller Common Code
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

#ifndef __COMMON_BSP_INTERFACE_H__
#define __COMMON_BSP_INTERFACE_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief initializes the board layer stuff
 */
extern void BSPInterface_Initialize(void);

/**
 * @brief Gets the number of ticks
 *
 * This is not intended to act as a RTC, but it will never
 * be intentionally cleared after BSP initialization
 *
 * 1 tick == 1ms
 *
 * @return ticks
 */
extern uint16_t BSPInterface_GetTicks(void);

/**
 * @brief Toggle logic state of an output interface
 *
 * Interacts with external functionality.  The state
 * represents a logic state and may not directly correlate
 * to a high or low state of a single output line
 *
 * @param id Identifier of the functionality to change state
 * @param state Logic state to set the functionality to
 */
extern void BSPInterface_SetOutputState(uint8_t id, bool state);

/**
 * @brief Obtains the logic state of an input interface
 *
 * Interacts with external functionality.  The state
 * represents a logic state and may not directly correlate
 * to a high or low state of a single input line
 *
 * @param id Identifier of the functionality to change state
 *
 * @return Logic state of the functionality
 */
extern bool BSPInterface_GetInputState(uint8_t id);

#endif /* __COMMON_BSP_INTERFACE_H__ */

