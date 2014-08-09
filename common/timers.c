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

#include "timers.h"

#include "bsp_interface.h"

void Timer_Initialize(timer_t *timer, timer_modes_t mode, uint32_t period)
{
    if (TIMER_NUMBER_OF_MODES > mode)
    {
        timer->mode = mode;
        timer->period = period;
    }
    else
    {
        /* Just set up something rational, have the timer always expire */
        timer->mode = TIMER_MODE_SINGLE;
        timer->period = 0;
    }

    Timer_Reset(timer);
}

void Timer_SetTimeoutPeriod(timer_t *timer, uint32_t period)
{
    if (TIMER_NUMBER_OF_MODES > timer->mode)
    {
        timer->period = period;
        Timer_Reset(timer);
    }
}

void Timer_Reset(timer_t *timer)
{
    timer->remaining = timer->period;
    timer->extra = 0;
    timer->tick = BSPInterface_GetTicks();
}

bool Timer_Timeout(timer_t *timer)
{
    uint16_t currTick = BSPInterface_GetTicks();
    uint16_t delta = currTick - timer->tick;
    bool timeoutOccurred = false;

    /* Determination of whether if a timeout event occurred is the same for */
    /* all modes; is the time elapsed since the last check greater than the */
    /* time remaining on the timer                                          */
    if ((0 == timer->remaining) || (delta >= timer->remaining))
    {
        /* if we overran the timer, calculate the extra time that has */
        /* elapsed                                                    */
        if (delta > timer->remaining)
        {
            timer->extra += delta - timer->remaining;
        }

        timer->remaining = 0;
        timeoutOccurred = true;

        /* the action on a timeout event depends on the timer mode.  For a */
        /* single shot timer, we do nothing, it will always evaluate to an */
        /* timeout event until we explicitly reset the timer.  For a       */
        /* recurring timer, we reset it now so that the next timeout event */
        /* will occur a period from now.  For interval timers, we use the  */
        /* extra parameter to make the timeout events occur in integer     */
        /* number of periods since the timer was started                   */
        if (TIMER_MODE_RECURRING == timer->mode)
        {
            Timer_Reset(timer);
        }
        else if (TIMER_MODE_INTERVAL == timer->mode)
        {
            /* If the extra time is more than the period, leave the        */
            /* remaining time as 0 to ensure that the next call will raise */
            /* the timeout event again, and reduce the extra time by the   */
            /* period.  If the period is greater than the extra time,      */
            /* compensate the remaining time to account for the extra time */
            if (timer->extra >= timer->period)
            {
                timer->extra -= timer->period;
            }
            else
            {
                timer->remaining = timer->period - timer->extra;
                timer->extra = 0;
            }
        }
    }
    else
    {
        timer->remaining -= delta;
    }

    timer->tick = currTick;

    return timeoutOccurred;
}

void Stopwatch_Initialize(stopwatch_t *stopwatch)
{
    Stopwatch_Reset(stopwatch);
    Stopwatch_Start(stopwatch);
}

void Stopwatch_Reset(stopwatch_t *stopwatch)
{
    stopwatch->counter = 0;
}

void Stopwatch_Stop(stopwatch_t *stopwatch)
{
    stopwatch->running = false;
}

void Stopwatch_Start(stopwatch_t *stopwatch)
{
    stopwatch->running = true;
    stopwatch->tick = BSPInterface_GetTicks();
}

uint32_t Stopwatch_Lap(stopwatch_t *stopwatch)
{
    uint32_t lap = Stopwatch_GetElapsed(stopwatch);
    Stopwatch_Initialize(stopwatch);
	
	return lap;
}

void Stopwatch_Update(stopwatch_t *stopwatch)
{
    if (stopwatch->running)
    {
        uint16_t currTick = BSPInterface_GetTicks();
        uint16_t delta = currTick - stopwatch->tick;

        stopwatch->counter += delta;
        stopwatch->tick = currTick;
    }
}

uint32_t Stopwatch_GetElapsed(stopwatch_t *stopwatch)
{
    Stopwatch_Update(stopwatch);

    return stopwatch->counter;
}

