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

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>

#include "common/timers.h"
#include "common/bsp_interface.h"

#include "lib44780/hd44780_low.h"
#include "lib44780fw/hd44780fw.h"

#include "bsp/bsp.h"

#include "controller.h"
#include "score_keeper.h"

static const uint8_t leftArrows[8][8] = {
    {0x03, 0x07, 0x0F, 0x1F, 0x1F, 0x0F, 0x07, 0x03},
    {0x00, 0x00, 0x00, 0x1F, 0x01, 0x00, 0x00, 0x00},
    {0x06, 0x0E, 0x1E, 0x1F, 0x1E, 0x1E, 0x0E, 0x06},
    {0x00, 0x00, 0x01, 0x1F, 0x03, 0x01, 0x00, 0x00},
    {0x0C, 0x1C, 0x1C, 0x1F, 0x1C, 0x1C, 0x1C, 0x0C},
    {0x00, 0x01, 0x03, 0x1F, 0x07, 0x03, 0x01, 0x00},
    {0x18, 0x18, 0x18, 0x1F, 0x18, 0x18, 0x18, 0x18},
    {0x01, 0x03, 0x07, 0x1F, 0x0F, 0x07, 0x03, 0x01}
};

static const uint8_t rightArrows[8][8] = {
    {0x18, 0x1C, 0x1E, 0x1F, 0x1F, 0x1E, 0x1C, 0x18},
    {0x0C, 0x0E, 0x0F, 0x1F, 0x0F, 0x0F, 0x0E, 0x0C},
    {0x00, 0x00, 0x00, 0x1F, 0x10, 0x00, 0x00, 0x00},
    {0x06, 0x07, 0x07, 0x1F, 0x07, 0x07, 0x07, 0x06},
    {0x00, 0x00, 0x10, 0x1F, 0x18, 0x10, 0x00, 0x00},
    {0x03, 0x03, 0x03, 0x1F, 0x03, 0x03, 0x03, 0x03},
    {0x00, 0x10, 0x18, 0x1F, 0x1C, 0x18, 0x10, 0x00},
    {0x10, 0x18, 0x1C, 0x1F, 0x1E, 0x1C, 0x18, 0x10}
};

static const char leftArrowline[5][20] = {
    { '-', '-', '-',  0x00, '-', '-', '-',  0x00, '-', '-', '-',  0x00, '-', '-', '-',  0x00, '-', '-', '-',  0x00 },
    { '-', '-', 0x01, 0x02, '-', '-', 0x01, 0x02, '-', '-', 0x01, 0x02, '-', '-', 0x01, 0x02, '-', '-', 0x01, 0x02 },
    { '-', '-', 0x03, 0x04, '-', '-', 0x03, 0x04, '-', '-', 0x03, 0x04, '-', '-', 0x03, 0x04, '-', '-', 0x03, 0x04 },
    { '-', '-', 0x05, 0x06, '-', '-', 0x05, 0x06, '-', '-', 0x05, 0x06, '-', '-', 0x05, 0x06, '-', '-', 0x05, 0x06 },
    { '-', '-', 0x07, '-',  '-', '-', 0x07, '-',  '-', '-', 0x07, '-',  '-', '-', 0x07, '-',  '-', '-', 0x07, '-'  }
};

static const char rightArrowline[5][20] = {
    { 0x00, '-',  '-', '-', 0x00, '-',  '-', '-', 0x00, '-',  '-', '-', 0x00, '-',  '-', '-', 0x00, '-',  '-', '-' },
    { 0x01, 0x02, '-', '-', 0x01, 0x02, '-', '-', 0x01, 0x02, '-', '-', 0x01, 0x02, '-', '-', 0x01, 0x02, '-', '-' },
    { 0x03, 0x04, '-', '-', 0x03, 0x04, '-', '-', 0x03, 0x04, '-', '-', 0x03, 0x04, '-', '-', 0x03, 0x04, '-', '-' },
    { 0x05, 0x06, '-', '-', 0x05, 0x06, '-', '-', 0x05, 0x06, '-', '-', 0x05, 0x06, '-', '-', 0x05, 0x06, '-', '-' },
    { '-',  0x07, '-', '-', '-',  0x07, '-', '-', '-',  0x07, '-', '-', '-',  0x07, '-', '-', '-',  0x07, '-', '-' }
};

static const uint8_t antiasterik[8] = {0x00, 0x0A, 0x04, 0x1F, 0x04, 0x0A, 0x00, 0x00};

static const char buzzline[2][16] = {
    { '*',   0x00, '*',  0x00, '*',  ' ', 'B', 'U', 'Z', 'Z', ' ', 0x00, '*',  0x00, '*',  0x00 },
    { 0x00,  '*',  0x00, '*',  0x00, ' ', 'B', 'U', 'Z', 'Z', ' ', '*',  0x00, '*',  0x00, '*'  }
};

static const char startInstructions[] = "Touch ring to start... Touch ring to s";
static const char winInstructions[] = "Touch ring to win... Touch ring to w";
static struct hd44780_l_conf low_conf;
static struct hd44780fw_conf fw_conf;

static timer_t quickTimer;
static timer_t mediumTimer;
static timer_t slowTimer;

static bool scoreToggle;

static uint8_t arrowIndex;
static uint8_t instructionIndex;
static uint8_t group;
static uint8_t idx;

static controller_state_t state;

/**
 * @brief Builds a time string that fits in 6 characters
 *
 * Formats:
 *
 * Less than 10 seconds: '   x.x'
 * Less than a minute:   '  xx.x'
 * Less than 10 minutes: 'x:xx.x'
 * More than 10 minutes: ' xx:xx'
 *
 * @param s Pointer to a 7-char array
 * @param ms Time in milliseconds to display
 */
static void BuildTimeString(char *s, uint32_t ms)
{
    uint16_t minutes = ms / 60000;
    double seconds = (ms % 60000) / 1000.0f;

    if (0 < minutes)
    {
        if (10 <= minutes)
        {
            sprintf(s, "%3d:%02.0f", minutes, seconds);
        }
        else
        {
            sprintf(s, "%d:%04.1f", minutes, seconds);
        }
    }
    else
    {
        sprintf(s, "%6.1f", seconds);
    }
}

static void DisplayWait(void)
{
    if (Timer_Timeout(&quickTimer))
    {
        arrowIndex = (arrowIndex + 1) % 20;

        group = arrowIndex % 5;

        if (0 == group)
        {
            idx = (idx + 1) % 4;
        }

        hd44780fw_write_len(&fw_conf, &leftArrowline[group][idx], 16, 0, HD44780FW_WR_NO_CLEAR_BEFORE);
    }

    if (Timer_Timeout(&mediumTimer))
    {
        instructionIndex = (instructionIndex + 1) % 23;
        hd44780fw_write_len(&fw_conf, &startInstructions[instructionIndex], 16, 16, HD44780FW_WR_NO_CLEAR_BEFORE);
    }
}

static void DisplayBegin(void)
{
    if (Timer_Timeout(&quickTimer))
    {
        arrowIndex = (arrowIndex + 1) % 20;

        group = arrowIndex % 5;

        if (0 == group)
        {
            idx--;
            idx = idx % 4;
        }

        hd44780fw_write_len(&fw_conf, &rightArrowline[group][idx], 16, 0, HD44780FW_WR_NO_CLEAR_BEFORE);
    }

    if (Timer_Timeout(&mediumTimer))
    {
        instructionIndex = (instructionIndex + 1) % 21;
        hd44780fw_write_len(&fw_conf, &winInstructions[instructionIndex], 16, 16, HD44780FW_WR_NO_CLEAR_BEFORE);
    }
}

static void DisplayRun(void)
{
    if (Timer_Timeout(&quickTimer))
    {
        char p[6] = "99999";
        char s[7];

        const score_t score = ScoreKeeper_GetScore();

        BuildTimeString(s, score.runningTime);
        hd44780fw_write_len(&fw_conf, s, 6, 4, HD44780FW_WR_NO_CLEAR_BEFORE);

        BuildTimeString(s, score.totalTime);
        hd44780fw_write_len(&fw_conf, s, 6, 20, HD44780FW_WR_NO_CLEAR_BEFORE);

        if (99999 > score.penalties)
        {
            int pp = score.penalties;
            sprintf(p, "%d", pp);
        }

        hd44780fw_write_len(&fw_conf, p, 5, 27, HD44780FW_WR_NO_CLEAR_BEFORE);
    }
}

static void DisplayBuzz(void)
{
    bool toggle = false;

    if (Timer_Timeout(&quickTimer))
    {
        if (toggle)
        {
            toggle = false;
            hd44780fw_write_len(&fw_conf, buzzline[0], 16, 0, HD44780FW_WR_NO_CLEAR_BEFORE);
            hd44780fw_write_len(&fw_conf, buzzline[1], 16, 16, HD44780FW_WR_NO_CLEAR_BEFORE);
        }
        else
        {
            toggle = true;
            hd44780fw_write_len(&fw_conf, buzzline[1], 16, 0, HD44780FW_WR_NO_CLEAR_BEFORE);
            hd44780fw_write_len(&fw_conf, buzzline[0], 16, 16, HD44780FW_WR_NO_CLEAR_BEFORE);
        }
    }
}

static void DisplayScore(void)
{
    if (Timer_Timeout(&slowTimer))
    {
        if (scoreToggle)
        {
            scoreToggle = false;
            DisplayRun();
        }
        else
        {
            char r[7] = "NONE  ";
            char t[7] = "NONE  ";
            char p[6] = "NONE ";
            int rRank = ScoreKeeper_GetRunningTimeRank();
            int tRank = ScoreKeeper_GetTotalTimeRank();
            int pRank = ScoreKeeper_GetPenaltyRank();

            if (-1 < rRank) { sprintf(r, "#%d", rRank); }
            if (-1 < tRank) { sprintf(t, "#%d", rRank); }
            if (-1 < pRank) { sprintf(p, "#%d", rRank); }

            hd44780fw_write_len(&fw_conf, r, 6, 4, HD44780FW_WR_NO_CLEAR_BEFORE);
            hd44780fw_write_len(&fw_conf, t, 6, 20, HD44780FW_WR_NO_CLEAR_BEFORE);
            hd44780fw_write_len(&fw_conf, p, 5, 27, HD44780FW_WR_NO_CLEAR_BEFORE);

            scoreToggle = true;
        }
    }
}

static void HandleTransistion(void)
{
    const controller_state_t currentState = Controller_GetState();

    /* Only clear and reset the display on a transition */
    if (currentState != state)
    {
        uint8_t i;

        state = currentState;

        switch(state)
        {
        case STATE_INITIALIZE:
            hd44780fw_write(&fw_conf, "Visual BuzzWire", 0, HD44780FW_WR_CLEAR_BEFORE);
            hd44780fw_write(&fw_conf, "Version 1.0.00", 16, HD44780FW_WR_NO_CLEAR_BEFORE);
            break;
        case STATE_WAITING:
            for (i = 0; i < 8; i++)
            {
                hd44780fw_build_cc(&fw_conf, i, leftArrows[i]);
            }

            hd44780fw_write_len(&fw_conf, leftArrowline[0], 16, 0, HD44780FW_WR_CLEAR_BEFORE);
            hd44780fw_write_len(&fw_conf, startInstructions, 16, 16, HD44780FW_WR_NO_CLEAR_BEFORE);
            arrowIndex = 0;
            idx = 0;
            instructionIndex = 0;
            Timer_Reset(&quickTimer);
            Timer_Reset(&mediumTimer);
            break;
        case STATE_BEGIN:
            for (i = 0; i < 8; i++)
            {
                hd44780fw_build_cc(&fw_conf, i, rightArrows[i]);
            }

            hd44780fw_write_len(&fw_conf, rightArrowline[0], 16, 0, HD44780FW_WR_CLEAR_BEFORE);
            hd44780fw_write_len(&fw_conf, winInstructions, 16, 16, HD44780FW_WR_NO_CLEAR_BEFORE);
            arrowIndex = 0;
            idx = 0;
            instructionIndex = 0;
            Timer_Reset(&quickTimer);
            Timer_Reset(&mediumTimer);
            break;
        case STATE_RUNNING:
            hd44780fw_write(&fw_conf, "RUN    0.0 TOUCH", 0, HD44780FW_WR_CLEAR_BEFORE);
            hd44780fw_write(&fw_conf, "TOT    0.0     0", 16, HD44780FW_WR_NO_CLEAR_BEFORE);
            break;
        case STATE_BUZZ:
            hd44780fw_build_cc(&fw_conf, 0, antiasterik);
            hd44780fw_write_len(&fw_conf, buzzline[0], 16, 0, HD44780FW_WR_CLEAR_BEFORE);
            hd44780fw_write_len(&fw_conf, buzzline[1], 16, 16, HD44780FW_WR_NO_CLEAR_BEFORE);
            break;
        case STATE_DONE:
            /* Force the medium timer to expire so that it'll display the score */
            mediumTimer.remaining = 0;
            Timer_Reset(&slowTimer);

            hd44780fw_write(&fw_conf, "RUN    0.0 TOUCH", 0, HD44780FW_WR_CLEAR_BEFORE);
            hd44780fw_write(&fw_conf, "TOT    0.0     0", 16, HD44780FW_WR_NO_CLEAR_BEFORE);
            DisplayRun();
            scoreToggle = false;
            break;
        default:
            /* do nothing */
            break;
        }
    }
}

static void HandleState(void)
{

    switch(state)
    {
    case STATE_INITIALIZE: /* do nothing */ break;
    case STATE_WAITING:    DisplayWait();   break;
    case STATE_BEGIN:      DisplayBegin();  break;
    case STATE_RUNNING:    DisplayRun();    break;
    case STATE_BUZZ:       DisplayBuzz();   break;
    case STATE_DONE:       DisplayScore();  break;
    default:               /* do nothing */ break;
    }
}

void Display_Initialize(void)
{
    state = (controller_state_t)0xFF;

    Timer_Initialize(&quickTimer, TIMER_MODE_RECURRING, 100);
    Timer_Initialize(&mediumTimer, TIMER_MODE_RECURRING, 300);
    Timer_Initialize(&slowTimer, TIMER_MODE_RECURRING, 1000);

    BSP_ConfigureDisplay(&low_conf);

	fw_conf.low_conf = &low_conf;
	low_conf.line1_base_addr = 0x00;
	low_conf.line2_base_addr = 0x40;
	low_conf.dl = HD44780_L_FS_DL_8BIT;

	fw_conf.total_chars = 32;
	fw_conf.font = HD44780_L_FS_F_58;
	fw_conf.lines = HD44780_L_FS_N_DUAL;

	hd44780fw_init(&fw_conf);
}

void Display_Run(void)
{
    HandleTransistion();
    HandleState();
}

