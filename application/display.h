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

#ifndef __BUZZWIRE_DISPLAY_H__
#define __BUZZWIRE_DISPLAY_H__

/**
 * @brief Initializes the display
 *
 * Sets up the configuration for the LCD and blanks it
 */
void Display_Initialize(void);

/**
 * @brief Executes the display
 */
void Display_Run(void);

void Display_ShowPosts(void);

#endif /* __BUZZWIRE_DISPLAY_H__ */

