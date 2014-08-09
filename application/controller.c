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

#include "controller.h"

#include <stdint.h>

#include "common/bsp_interface.h"
#include "common/timers.h"

#include "bsp/bsp.h"

#include "score_keeper.h"

#define BUZZ_TIME 500
#define STARTUP_TIME   5000
#define SHOWSCORE_TIME 5000

typedef enum
{
    EV_NONE = 0,
    EV_LEFTPOST,
    EV_BUZZ,
    EV_RIGHTPOST,
    EV_TIMEREXPIRE,
    EV_ANY
} event_t;

typedef enum
{
    ST_INITIALIZE = 0,
    ST_BOOTUP,
    ST_WAITING,
    ST_BEGIN,
    ST_START,
    ST_RUNNING,
    ST_BUZZ,
    ST_DONE,
    ST_SHOWSCORE,
    ST_ANY
} state_t;

typedef const event_t (*handler_fp)(void);

typedef struct
{
    state_t state;
    event_t event;
    handler_fp handler;
    state_t next;
} transition_t;

static const event_t Initialize(void);
static const event_t Bootup(void);
static const event_t Waiting(void);
static const event_t Begin(void);
static const event_t Start(void);
static const event_t Running(void);
static const event_t Buzz(void);
static const event_t Done(void);
static const event_t ShowScore(void);

static const transition_t transistions[] =
{
    { ST_INITIALIZE, EV_ANY,         Bootup,     ST_BOOTUP     },
    { ST_BOOTUP,     EV_TIMEREXPIRE, Waiting,    ST_WAITING    },
    { ST_BOOTUP,     EV_ANY,         Bootup,     ST_BOOTUP     },
    { ST_WAITING,    EV_LEFTPOST,    Begin,      ST_BEGIN      },
    { ST_WAITING,    EV_ANY,         Waiting,    ST_WAITING    },
    { ST_BEGIN,      EV_LEFTPOST,    Begin,      ST_BEGIN      },
    { ST_BEGIN,      EV_ANY,         Start,      ST_START      },
    { ST_START,      EV_ANY,         Running,    ST_RUNNING    },
    { ST_RUNNING,    EV_RIGHTPOST,   Done,       ST_DONE       },
    { ST_RUNNING,    EV_BUZZ,        Buzz,       ST_BUZZ       },
    { ST_RUNNING,    EV_ANY,         Running,    ST_RUNNING    },
    { ST_BUZZ,       EV_RIGHTPOST,   Done,       ST_DONE       },
    { ST_BUZZ,       EV_TIMEREXPIRE, Running,    ST_RUNNING    },
    { ST_BUZZ,       EV_ANY,         Buzz,       ST_BUZZ       },
    { ST_DONE,       EV_ANY,         ShowScore,  ST_SHOWSCORE  },
    { ST_SHOWSCORE,  EV_TIMEREXPIRE, Waiting,    ST_WAITING    },
    { ST_SHOWSCORE,  EV_ANY,         ShowScore,  ST_SHOWSCORE  },
    { ST_ANY,        EV_ANY,         Initialize, ST_INITIALIZE }

};

#define TRANSISTIONS_COUNT (sizeof(transistions) / sizeof(transition_t))

static transition_t current;
static timer_t timer;

static bool leftPostHit;
static bool rightPostHit;
static bool wireHit;

static const event_t Initialize(void)
{
    Timer_Initialize(&timer, TIMER_MODE_SINGLE, 5000);
    Timer_Reset(&timer);
    return EV_NONE;
}

static const event_t Bootup(void)
{
    event_t event = EV_NONE;

    if (Timer_Timeout(&timer))
    {
        event = EV_TIMEREXPIRE;
    }

    return event;
}

static const event_t Waiting(void)
{
    event_t event = EV_NONE;

    if (leftPostHit)
    {
        event = EV_LEFTPOST;
    }

    return event;
}

static const event_t Begin(void)
{
    event_t event = EV_LEFTPOST;

    /* Start the game when we leave the left post */
    if (!leftPostHit)
    {
        event = EV_NONE;
    }

    return event;
}

static const event_t Start(void)
{
    ScoreKeeper_Start();
    return EV_NONE;
}

static const event_t Running(void)
{
    event_t event = EV_NONE;

    /* Winning the game takes priority */
    if (rightPostHit)
    {
        event = EV_RIGHTPOST;
    }
    else if (wireHit)
    {
        event = EV_BUZZ;
        Timer_SetTimeoutPeriod(&timer, BUZZ_TIME);
        Timer_Reset(&timer);
        ScoreKeeper_Penalty();
    }

    return event;
}

static const event_t Buzz(void)
{
    event_t event = EV_NONE;

    /* Winning the game takes priority */
    if (rightPostHit)
    {
        event = EV_RIGHTPOST;
    }
    else if (Timer_Timeout(&timer))
    {
        event = EV_TIMEREXPIRE;
    }

    return event;
}
static const event_t Done(void)
{
    ScoreKeeper_End();
    Timer_SetTimeoutPeriod(&timer, SHOWSCORE_TIME);
    Timer_Reset(&timer);

    return EV_NONE;
}

static const event_t ShowScore(void)
{
    event_t event = EV_NONE;

    if (Timer_Timeout(&timer))
    {
        event = EV_TIMEREXPIRE;
    }

    return event;
}

void Controller_Initialize(void)
{
    current.state = ST_INITIALIZE;
    current.handler = Initialize;

    Timer_Initialize(&timer, TIMER_MODE_SINGLE, 0);
}

void Controller_Run(void)
{
    uint8_t i;
    event_t event;

    leftPostHit  = BSPInterface_GetInputState(BSP_INPUT_BUZZ_LEFT_POST);
    rightPostHit = BSPInterface_GetInputState(BSP_INPUT_BUZZ_RIGHT_POST);
    wireHit      = BSPInterface_GetInputState(BSP_INPUT_BUZZ_WIRE);

    event = current.handler();

    for (i = 0; i < TRANSISTIONS_COUNT; i++)
    {
        if ((transistions[i].state == current.state) || (ST_ANY == transistions[i].state))
        {
            if ((transistions[i].event == event) || (EV_ANY == transistions[i].event))
            {
                current = transistions[i];
                current.state = current.next;
                break;
            }
        }
    }

    if (i >= TRANSISTIONS_COUNT)
    {
        Controller_Initialize();
    }
}

const controller_state_t Controller_GetState(void)
{
    controller_state_t state;

    switch (current.state)
    {
    case ST_WAITING:
        state = STATE_WAITING;
        break;
    case ST_BEGIN:
        state = STATE_BEGIN;
        break;
    case ST_START:
    case ST_RUNNING:
        state = STATE_RUNNING;
        break;
    case ST_BUZZ:
        state = STATE_BUZZ;
        break;
    case ST_DONE:
    case ST_SHOWSCORE:
        state = STATE_DONE;
        break;
    case ST_INITIALIZE:
    case ST_BOOTUP:
    default:
        state = STATE_INITIALIZE;
        break;
    }

    return state;
}

