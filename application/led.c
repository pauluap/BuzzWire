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

#include "common/bsp_interface.h"
#include "common/timers.h"

#include "bsp/bsp.h"

#include "controller.h"

static timer_t timer;
static timer_t subTimer;

static controller_state_t state;

/**
 * @brief Toggles the LEDs
 */
static void Toggle(void)
{
    static bool toggle = false;

    if (Timer_Timeout(&timer))
    {
        if (toggle)
        {
            toggle = false;
            BSPInterface_SetOutputState(BSP_OUTPUT_LED_GROUP0, false);
            BSPInterface_SetOutputState(BSP_OUTPUT_LED_GROUP1, true);
        }
        else
        {
            toggle = true;
            BSPInterface_SetOutputState(BSP_OUTPUT_LED_GROUP1, false);
            BSPInterface_SetOutputState(BSP_OUTPUT_LED_GROUP0, true);
        }
    }
}

/**
 * @brief Switches all LEDs off
 */
static void Off(void)
{
    BSPInterface_SetOutputState(BSP_OUTPUT_LED_GROUP0, false);
    BSPInterface_SetOutputState(BSP_OUTPUT_LED_GROUP1, false);
}

static void HandleTransistion(void)
{
    const controller_state_t currentState = Controller_GetState();

    if (state != currentState)
    {
        state = currentState;

        switch(state)
        {
        case STATE_INITIALIZE:
            Timer_SetTimeoutPeriod(&timer, 200);
            break;
        case STATE_DONE:
        case STATE_BEGIN:
            Timer_SetTimeoutPeriod(&timer, 300);
            break;
        case STATE_BUZZ:
            Timer_SetTimeoutPeriod(&timer, 50);
            break;
        case STATE_WAITING:
            Timer_SetTimeoutPeriod(&subTimer, 1000);
            Timer_SetTimeoutPeriod(&timer, 10);
            break;
        case STATE_RUNNING:
        default:
            /* do nothing */
            break;
        }

        Off();
        Timer_Reset(&timer);
    }
}

static void HandleState(void)
{
    static bool subToggle = false;

    /* The period of the timer used for toggling the LEDs */
    /* is set during the handling of the state transition */
    switch(state)
    {
    case STATE_INITIALIZE:
    case STATE_BUZZ:
    case STATE_DONE:
    case STATE_BEGIN:
        Toggle();
        break;
    case STATE_WAITING:
        /* Off for 4 seconds, then toggle for 1 second */
        if (subToggle)
        {
            if (Timer_Timeout(&subTimer))
            {
                subToggle = false;
                Timer_SetTimeoutPeriod(&subTimer, 4000);
            }
            else
            {
                Toggle();
            }
        }
        else
        {
            if (Timer_Timeout(&subTimer))
            {
                subToggle = true;
                Timer_SetTimeoutPeriod(&subTimer, 1000);
                Timer_Reset(&timer);
            }
            else
            {
                Off();
            }
        }
        break;
    case STATE_RUNNING:
    default:
        /* Everything off */
        Off();
        break;
    }
}

void LED_Initialize(void)
{
    state = (controller_state_t)0xFF;

    BSPInterface_SetOutputState(BSP_OUTPUT_LED_GROUP0, false);
    BSPInterface_SetOutputState(BSP_OUTPUT_LED_GROUP1, false);

    Timer_Initialize(&timer, TIMER_MODE_RECURRING, 50);
    Timer_Initialize(&timer, TIMER_MODE_RECURRING, 10);
}

void LED_Run(void)
{
    HandleTransistion();
    HandleState();
}

