/*
    Copyright (C) 2017 Alexey Dynda

    This file is part of TinyButtons Library.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 *    ARDUINO A1 pin ------------------------------------
 *                  |        |        |        |        |
 *                 BTN1     BTN2     BTN3     BTN4     20K
 *                  |        |        |        |        |
 *    VCC --- 2.2K --- 2.2K --- 2.2K --- 2.2K --- 2.2K --- GND
 *                 
 */

#include <tiny_buttons.h>
                             //  BTN1   BTN2  BTN3   BTN4
const int16_t buttons[4]    =  { 752,   542,  362,   188 };

static TinyAnalogButtons s_buttons(A1, buttons, 4 );


void onButtonDown(uint8_t id, uint16_t timeDeltaMs)
{
    Serial.print("Pressed button: ");
    Serial.println(id + 1); /* indexes are zero-based */
}


void setup()
{
    Serial.begin(9600);
    s_buttons.init();
    s_buttons.onButtonDown(&onButtonDown);
}

void loop()
{
    s_buttons.update();
}
