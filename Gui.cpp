/*====================================================================================
Copyright (c) 2015 Barry Carter <barry.carter@gmail.com>


Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

A simple GUI Widget library for TFT screens. 
====================================================================================
*/
#if ARDUINO < 100
    #include <WProgram.h>
#else
    #include <Arduino.h>
#endif

#include <stddef.h>

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "GuiLibrary.h"

Adafruit_ILI9341* _tft;

#if USING_SD
SdFat* _sd;
uint8_t _TFT_CS = 10;
#endif

Gui::Gui(void) {

};

void Gui::draw(void) {
    drawBase();
    GuiElement::draw();
}

void Gui::setRotation(int16_t degrees) { // multiples of 90
    if (degrees == 0)
        _tft->setRotation(0);
    else
        _tft->setRotation(degrees/90);
    
    rotation = degrees;
}

void Gui::sendEventToWidget(int16_t x, int16_t y, int8_t eventid) {
    // do collision detection
    GuiNode *tmphead = children.head();

    if (tmphead != NULL) {
        // get the last element
        while (tmphead != NULL) {
            GuiElement* element = (GuiElement*)tmphead->element();
            GuiElement *found = element->pointInWidget(x, y);

            if (found != NULL) {
                found->callbackProcess(found, eventid);
                return;
            }

            if (tmphead->next() == NULL)
                break;
                
            tmphead = tmphead->next();
        }
    }
}

// main app processing loop
void Gui::update(void) {
    static bool isPressing = false;
    static int16_t pressing_x, pressing_y;
    static uint16_t stamp = millis();

    // Retrieve a point  
    TSPoint p = ts->getPoint();
    
    // we have some minimum pressure we consider 'valid'
    // pressure of 0 means no pressing!
    if (p.z < MINPRESSURE || p.z > MAXPRESSURE) {
        uint16_t stampnow = millis() - stamp;

        // this serves as a debouncer/filter for those seemingly super crapola resistive panels
        // we are going to release this key after 5k millis regardless
        if ((stampnow > DEBOUNCE_DELAY && stampnow < 5000) && isPressing) {
            sendEventToWidget(pressing_x, pressing_y, GUI_EVENT_RELEASE);
            isPressing = false;
            stamp = millis();
        }
        return;
    }
    
    stamp = millis();

    // if the finger is still on the screen we don't need to do any work until it is released
    if (isPressing)
        return;

    // given a rotation for this tft, set th input device to map correctly to the display's orientation
    if (rotation == 0) {
        p.x = map(p.x, TS_MINX, TS_MAXX, 0, _tft->width());
        p.y = map(p.y, TS_MINY, TS_MAXY, 0, _tft->height());
    }
    else if (rotation == 90) {
        
        p.x = map(p.y, TS_MINY, TS_MAXY, 0, _tft->width());
        p.y = map(p.x, TS_MINX, TS_MAXX, _tft->height(), 0);
    }
    else if (rotation == 180) {
        
        p.x = map(p.x, TS_MINX, TS_MAXX, _tft->width(), 0);
        p.y = map(p.y, TS_MINY, TS_MAXY, _tft->height(), 0);
    }
    else if (rotation == 270) {
        int16_t oldx = p.x;
        p.x = map(p.y, TS_MINY, TS_MAXY, _tft->width(), 0);
        p.y = map(oldx, TS_MINX, TS_MAXX, 0, _tft->height());
    }

    Serial.print("x: "); Serial.print(p.x); Serial.print(" y: "); Serial.println(p.y);

    pressing_x = p.x;
    pressing_y = p.y;
    
    sendEventToWidget(p.x, p.y, GUI_EVENT_PRESS);

    isPressing = true;    
}
