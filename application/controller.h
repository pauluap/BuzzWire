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

#ifndef __BUZZWIRE_CONTROLLER_H__
#define __BUZZWIRE_CONTROLLER_H__


typedef enum
{
    STATE_INITIALIZE = 0,
    STATE_WAITING,
    STATE_BEGIN,
    STATE_RUNNING,
    STATE_BUZZ,
    STATE_DONE
} controller_state_t;

/**
 * @brief Sets up the state machine controller
 */
void Controller_Initialize(void);

/**
 * @brief Executes the state machine
 */
void Controller_Run(void);

/**
 * @brief Gets the current controller state
 *
 * @return Controller state
 */
const controller_state_t Controller_GetState(void);

#endif /* __BUZZWIRE_CONTROLLER_H__ */

