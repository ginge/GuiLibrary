====================================================================================
Copyright (c) 2015 Barry Carter <barry.carter@gmail.com>


Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

A simple GUI Widget library for TFT screens. 
====================================================================================
#include "GuiLibrary.h"

//an external global reference to the tft.
extern Adafruit_ILI9341* _tft;

void GuiLabel::draw() {
    draw(true);
}

void GuiLabel::draw(bool andBase) {
    if (!visible()) return;
    
    if (andBase)
        GuiElement::drawBase(); 
    
     // draw the text
    int16_t newx = absoluteX() + padding;
    int16_t newy = absoluteY() + padding;

    // only do this if there is some text to draw
    if (labelText != NULL) {
        // vertically align
        newy = GuiUtils::getElementCentreY(this) - ((labelFontSize * 7)/2);

        if (textAlignH == TEXT_H_ALIGN_CENTRE) {
            uint16_t centrex = GuiUtils::getElementCentreX(this);
            newx = extraXOffset + centrex - ((strlen(labelText) * (labelFontSize *6)) / 2);
        }
        else if (textAlignH == TEXT_H_ALIGN_RIGHT) {
            newx = extraXOffset + (newx + width) - (strlen(labelText) * (labelFontSize *6)) - padding;
        }
        else {
            newx += extraXOffset;
        }
        Serial.print("y");
        Serial.println(newy);

        GuiUtils::drawText(this, labelText, foregroundColour, newx, newy, labelFontSize);
    }
    // draw all children
    GuiElement::draw();
}

//////////////////////////
