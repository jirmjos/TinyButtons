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

#include "tiny_buttons.h"
#include "sys/tiny_booster.h"

#define BUTTONS_SAMPLING_PERIOD   20 /// period in milliseconds
#define DEBOUNCE_SAMPLING_PERIOD  5  /// period in milliseconds
#define DEBOUNCE_CYCLES           3  /// 3*5 = 15 milliseconds
#define MIN_DELAY_AFTER_UP        50 /// minimum delay before the button can be pressed again
#define MAX_WAIT_DOUBLE_PRESS_TIMEOUT  \
                                  300


void TinyAnalogButtons::init()
{
    pinMode(m_pin, INPUT);
};


/**
 *  In milliseconds
 */
void TinyAnalogButtons::update()
{
    /* Do not sample too often */
    m_wasButtonDown = m_isButtonDown;
    if ((0 == m_checkBounce) && ((uint16_t)TinyOs::millis() - m_lastEventTimestampMs < BUTTONS_SAMPLING_PERIOD))
    {
        return;
    }
    if ((m_checkBounce != 0) && ((uint16_t)TinyOs::millis() - m_lastEventTimestampMs < DEBOUNCE_SAMPLING_PERIOD))
    {
        return;
    }
    int value = TinyOs::analogRead(m_pin);
    if (value == ADC_IN_PROGRESS) return;
    if ((value > m_lastReadAdc + ANALOG_BUTTONS_THRESHOLD) || (value < m_lastReadAdc - ANALOG_BUTTONS_THRESHOLD))
    {
        /* Read value differs from last rememberred. So, it seems that we need to perform
           debounce cycles for newly pressed button */
        m_checkBounce = 1;
    }
    m_lastEventTimestampMs = (uint16_t)TinyOs::millis();
    m_lastReadAdc = value;
    if (m_checkBounce < DEBOUNCE_CYCLES)
    {
        m_checkBounce++;
        return;      
    }
    m_checkBounce = 0;
    bool     found = false;
    for(byte n = 0; n < m_count; n++)
    {
        found = (value < m_buttons[n] + ANALOG_BUTTONS_THRESHOLD) && 
                (value > m_buttons[n] - ANALOG_BUTTONS_THRESHOLD);
        if (found && ((uint16_t)TinyOs::millis() - m_upTimestampMs >= MIN_DELAY_AFTER_UP))
        {
            // Change only, if new button is pressed and no much time passed since last hit
            if ( ((m_id != n) && ((uint16_t)TinyOs::millis() - m_downTimestampMs < MAX_WAIT_DOUBLE_PRESS_TIMEOUT))
                ||
                // or if button is being pressed
                (m_isButtonDown == false))
            {
                m_id = n;
                m_isButtonDown = true;
                m_downTimestampMs = (uint16_t)TinyOs::millis(); 
                if (m_downHandler != nullptr)
                {
                    m_downHandler(m_id, (uint16_t)TinyOs::millis() - m_upTimestampMs);
                }
            }
            else
            {
                /* Otherwise, just holding button */
                if (m_holdHandler != nullptr)
                {
                    m_holdHandler(m_id, (uint16_t)TinyOs::millis() - m_downTimestampMs);
                }
            }
        }
        if (found)
        {
            break;
        }
    }
    /* If no pressed button is found, then all buttons are up */
    if (!found)
    {
        if (m_isButtonDown)
        {
            m_isButtonDown = false;
            if (m_disableAction == true)
            {
                m_wasButtonDown = false;
                m_disableAction = false;
            }
            else if ( m_upHandler != nullptr )
            {
                m_upHandler(m_id, (uint16_t)TinyOs::millis() - m_downTimestampMs);
            }
            m_upTimestampMs = (uint16_t)TinyOs::millis(); 
        }
    }
}



