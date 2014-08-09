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

#include "bsp.h"

#include <stdbool.h>

#include "timers.h"

void BSPInterface_Initialize(void)
{
    /* Pull up the unused pins */
    MCUSR &= 0xEF;

                  /* DDRA7: 1 - Output - LCD D7              */
                  /* DDRA6: 1 - Output - LCD D6              */
                  /* DDRA5: 1 - Output - LCD D5              */
                  /* DDRA4: 1 - Output - LCD D4              */
                  /* DDRA3: 1 - Output - LCD D3              */
                  /* DDRA2: 1 - Output - LCD D2              */
                  /* DDRA1: 1 - Output - LCD D1              */
	DDRA = 0xFF;  /* DDRA0: 1 - Output - LCD D0              */

                  /* PORTA7: 0 - Output                      */
                  /* PORTA6: 0 - Output                      */
                  /* PORTA5: 0 - Output                      */
                  /* PORTA4: 0 - Output                      */
                  /* PORTA3: 0 - Output                      */
                  /* PORTA2: 0 - Output                      */
                  /* PORTA1: 0 - Output                      */
	PORTA = 0x00; /* PORTA0: 0 - Output                      */

                  /* DDRB7: 0 -  Input - Unused - Pulled up  */
                  /* DDRB6: 0 -  Input - Unused - Pulled up  */
                  /* DDRB5: 0 -  Input - Unused - Pulled up  */
                  /* DDRB4: 0 -  Input - Unused - Pulled up  */
                  /* DDRB3: 0 -  Input - Unused - Pulled up  */
                  /* DDRB2: 0 -  Input - INT2 Buzz Left Post */
                  /* DDRB1: 1 - Output - LED Group1          */
    DDRB = 0x03;  /* DDRB0: 1 - Output - LED Group0          */

                  /* PORTB7: 1 - Pulled up                   */
                  /* PORTB6: 1 - Pulled up                   */
                  /* PORTB5: 1 - Pulled up                   */
                  /* PORTB4: 1 - Pulled up                   */
                  /* PORTB3: 1 - Pulled up                   */
                  /* PORTB2: 0 - Not pulled up               */
                  /* PORTB1: 0 - Output                      */
    PORTB = 0xF8; /* PORTB0: 0 - Output                      */

                  /* DDRC7:  0 - Input - Unused - Pulled up  */
                  /* DDRC6:  0 - Input - Unused - Pulled up  */
                  /* DDRC5:  0 - Input - JTAG                */
                  /* DDRC4:  0 - Input - JTAG                */
                  /* DDRC3:  0 - Input - JTAG                */
                  /* DDRC2:  0 - Input - JTAG                */
                  /* DDRC1:  0 - Input - Unused - Pulled up  */
    DDRC = 0x00;  /* DDRC0:  0 - Input - Unused - Pulled up  */

                  /* PORTC7:  1 - Pulled up                  */
                  /* PORTC6:  1 - Pulled up                  */
                  /* PORTC5:  0 - Not pulled up              */
                  /* PORTC4:  0 - Not pulled up              */
                  /* PORTC3:  0 - Not pulled up              */
                  /* PORTC2:  0 - Not pulled up              */
                  /* PORTC1:  1 - Pulled up                  */
    PORTC = 0xC3; /* PORTC0:  1 - Pulled up                  */

                  /* DDRD7:  Input - Unused - Pulled up      */
                  /* DDRD6:  Input - LCD EN                  */
                  /* DDRD5:  Input - LCD RW                  */
                  /* DDRD4:  Input - LCD RS                  */
                  /* DDRD3:  Input - INT1 Buzz Post          */
                  /* DDRD2:  Input - INT0 Buzz Wire          */
                  /* DDRD1:  Input - Unused - Pulled up      */
    DDRD = 0x70;  /* DDRD0:  Input - Unused - Pulled up      */

                  /* DDRD7:  1 - Pulled up                   */
                  /* DDRD6:  0 - Output                      */
                  /* DDRD5:  0 - Output                      */
                  /* DDRD4:  0 - Output                      */
                  /* DDRD3:  0 - Not pulled up               */
                  /* DDRD2:  0 - Not pulled up               */
                  /* DDRD1:  1 - Pulled up                   */
    PORTD = 0x70; /* DDRD0:  1 - Pulled up                   */

    PORTB |= 0xF8; /* Don't pull up the pins that are used for ext interrupt - they're already pulled externally */
    PORTC |= 0xC3; /* Don't mess with the pins used for JTAG */
    PORTD |= 0x83; /* Don't pull up the pins that are used for ext interrupt - they're already pulled externally */

    /* Interrupt on the falling edge of ext interrupts */
    EICRA = 0x2A;

    /* Clear the external interrupt flags */
    EIFR = 0x03;

    BSP_InitializeTimers();

    /* Enable interrupts */
    SREG |= 0x80;
}

void BSP_ConfigureDisplay(struct hd44780_l_conf *lcdfw)
{
	lcdfw->rs_i = 4;
	lcdfw->rw_i = 5;
	lcdfw->en_i = 6;
	lcdfw->db7_i = 7;
	lcdfw->db6_i = 6;
	lcdfw->db5_i = 5;
	lcdfw->db4_i = 4;
	lcdfw->db3_i = 3;
	lcdfw->db2_i = 2;
	lcdfw->db1_i = 1;
	lcdfw->db0_i = 0;
	lcdfw->rs_port = &PORTD;
	lcdfw->rw_port = &PORTD;
	lcdfw->en_port = &PORTD;
	lcdfw->db7_port = &PORTA;
	lcdfw->db6_port = &PORTA;
	lcdfw->db5_port = &PORTA;
	lcdfw->db4_port = &PORTA;
	lcdfw->db3_port = &PORTA;
	lcdfw->db2_port = &PORTA;
	lcdfw->db1_port = &PORTA;
	lcdfw->db0_port = &PORTA;
}

void BSPInterface_SetOutputState(uint8_t id, bool state)
{
    switch ((bsp_outputs_t)id)
    {
    case BSP_OUTPUT_LED_GROUP0:
        if (state) { PORTB |=  _BV(PINB0); }
        else       { PORTB &= ~_BV(PINB0); }
        break;
    case BSP_OUTPUT_LED_GROUP1:
        if (state) { PORTB |=  _BV(PINB1); }
        else       { PORTB &= ~_BV(PINB1); }
        break;
    default:
        /* do nothing */
        break;
    }
}

bool BSPInterface_GetInputState(uint8_t id)
{
    bool state = false;

    /* the buzz inputs have interrupt-on-falling-edge set, so for those, the */
    /* state is true if either the interrupt flag is asserted or the line    */
    /* itself is low.  When requested, also clear the interrupt flag         */
    switch((bsp_inputs_t)id)
    {
    case BSP_INPUT_BUZZ_LEFT_POST:
        if (!(PINB & _BV(2)) || (EIFR & _BV(2)))
        {
            state = true;
        }

        EIFR |= _BV(2);
        break;
    case BSP_INPUT_BUZZ_RIGHT_POST:
        if (!(PIND & _BV(3)) || (EIFR & _BV(1)))
        {
            state = true;
        }

        EIFR |= _BV(1);
        break;
    case BSP_INPUT_BUZZ_WIRE:
        if (!(PIND & _BV(2)) || (EIFR & _BV(0)))
        {
            state = true;
        }

        EIFR |= _BV(0);
        break;
    default:
        /* do nothing */
        break;
    }

    return state;
}

