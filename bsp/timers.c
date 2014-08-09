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

#include <avr/interrupt.h>
#include <avr/io.h>

static uint16_t ticks;

ISR(TIMER0_COMPA_vect)
{
    ticks++;
}

void BSP_InitializeTimers(void)
{
    ticks = 0;

    /* Set up for 1ms ticks using Timer0 */

    /* First, halt the timers to make sure that we */
    /* don't mess up the first interrupt           */
    TIMSK0 = 0x00;
    TCCR0A = 0x00;
    TCCR0B = 0x00;

    /* Clear interrupts */
    TIFR0 = 0x07;

    /* Clear the timer register */
    TCNT0 = 0;

    /* Set OCR0A to match every 1ms (at 1MHz and Clk_io / 8 )
     * that is every 125 counts
     */
    OCR0A = 125;

    /*
     * Bits 7:6 - Normal port operation, OC0A disconnected
     * Bits 5:4 - Normal port operation, OC0B disconnected
     * Bits 3:2 - Reserved
     * Bits 1:0 - Clear Timer on Compare (WGM02 is in TCCR0B
     */
    TCCR0A = 0x02;

    /*
     * Bits 7:6 - Do nothing
     * Bits 5:4 - Reserved
     * Bit  3   - Clear Timer on Compare (WGM01:WGM00 is in TCCR0A)
     * Bits 2:0 - Select Clk_io / 8 prescaler
     *
     * When this register is set, the counter also starts
     */
    TCCR0B = 0x02;

    /* Turn on Output Compare Match A interrupt */
    TIMSK0 = 0x02;
}

uint16_t BSPInterface_GetTicks(void)
{
    /*
     * This is a 8-bit micro, so need to disable interrupts to prevent
     * interrupts from causing a rollover in middle of updating the
     * 16-bit tick value by temporarily disabling timer0 interrupts
     */
    uint16_t ticksToReturn;

    TIMSK0 = 0x00;

    ticksToReturn = ticks;

    TIMSK0 = 0x02;

    return ticksToReturn;
}

