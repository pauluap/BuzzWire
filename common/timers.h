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

#ifndef __COMMON_TIMERS_H__
#define __COMMON_TIMERS_H__

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief enumeration to set the behavior of the timer
 */
typedef enum
{
    TIMER_MODE_SINGLE = 0, //!< Once a timeout is asserted, it must be
                           //!< explicitly cleared

    TIMER_MODE_RECURRING,  //!< When the timer is called and a timeout is
                           //!< detected, it is cleared and the next timeout
                           //!< is set to occur in the configured period

    TIMER_MODE_INTERVAL,   //!< When the timer is called, and a timeout is
                           //!< detected, the amount of time past the period
                           //!< is recorded and the next timeout compensated
                           //!< by that amount  The amount can still be greater
                           //!< than the timeout period, so multiple calls to
                           //!< check the timeout may return a timeout event
    TIMER_NUMBER_OF_MODES
} timer_modes_t;

typedef struct
{
    timer_modes_t mode;
    uint32_t period;
    uint32_t remaining;
    uint32_t extra;
    uint16_t tick;
} timer_t;

typedef struct
{
    uint32_t counter;
    uint16_t tick;
    bool running;
} stopwatch_t;

/**
 * @brief Initializes a timer object
 *
 * @param timer Pointer to the timer object
 * @param mode Operating mode of the timer
 * @param period The timeout period in milliseconds
 */
void Timer_Initialize(timer_t *timer, timer_modes_t mode, uint32_t period);

/**
 * @brief Set timeout period
 *
 * Changes the timeout period and resets the timer object
 *
 * @param timer Pointer to the timer object
 * @param period The new timeout period in milliseconds
 */
void Timer_SetTimeoutPeriod(timer_t *timer, uint32_t period);

/**
 * @brief Resets the timer
 *
 * Resets the timer such that the next timeout event will occur in the
 * configured period amount
 *
 * @param timer Pointer to the timer object
 */
void Timer_Reset(timer_t *timer);

/**
 * @brief Queries if a timeout event occurred
 *
 * Depending on the mode, consecutive calls may return
 * timeout events @see timer_modes_t
 *
 * @param timer Pointer to the timer object
 *
 * @return true if a timeout event has occurred or if it is an single shot timer
 *         that has not been reset, false otherwise
 */
bool Timer_Timeout(timer_t *timer);

/**
 * @brief Initializes a stopwatch
 *
 * This sets the current count to 0 and starts it running
 *
 * @param stopwatch Pointer to the stopwatch instance
 */
void Stopwatch_Initialize(stopwatch_t *stopwatch);

/**
 * @brief Resets a stopwatch
 *
 * This sets the current count to 0 and stops it from running
 *
 * @param stopwatch Pointer to the stopwatch instance
 */
void Stopwatch_Reset(stopwatch_t *stopwatch);

/**
 * @brief Stops a stopwatch
 *
 * @param stopwatch Pointer to the stopwatch instance
 */
void Stopwatch_Stop(stopwatch_t *stopwatch);

/**
 * @brief Starts a stopwatch
 *
 * @param stopwatch Pointer to the stopwatch instance
 */
void Stopwatch_Start(stopwatch_t *stopwatch);

/**
 * @brief Gets the lap time
 *
 * This updates the stopwatch, obtains the elapsed time, sets the counter
 * to 0 and starts it running if it is not already running
 *
 * @param stopwatch Pointer to the stopwatch instance
 *
 * @return the number of elapsed milliseconds
 */
uint32_t Stopwatch_Lap(stopwatch_t *stopwatch);

/**
 * @brief Updates the stopwatch
 *
 * @param stopwatch Pointer to the stopwatch instance
 */
void Stopwatch_Update(stopwatch_t *stopwatch);

/**
 * @brief Returns the amount of milliseconds that has elapsed
 *
 * If the stopwatch is running, then it is updated first
 *
 * @param stopwatch Pointer to the stopwatch instance
 *
 * @return Number of milliseconds elapsed
 */
uint32_t Stopwatch_GetElapsed(stopwatch_t *stopwatch);

#endif /* __COMMON_TIMERS_H__ */

