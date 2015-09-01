/*====================================================================================
Copyright (c) 2015 Barry Carter <barry.carter@gmail.com>


Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

A simple GUI Widget library for TFT screens. 
====================================================================================
*/

#include "GuiLibrary.h"

//an external global reference to the tft.
extern Adafruit_ILI9341* _tft;

class GuiRadioButton : public GuiCheckBox {
public:
    GuiRadioButton(int16_t x, int16_t y, int16_t width, int16_t height, char const* text) : GuiCheckBox(x, y, width, height, text) {};
    void draw(void) {
        if (!visible()) return;    
        Serial.println(absoluteX());
        int16_t newx = absoluteX() + padding;
        int16_t midy = GuiUtils::getElementCentreY(this);
        int8_t boxSize = fontSize() * 7;
        
        // we are going to recalc the width here too. not ideal but the width isn't know when an element is added
        if (parent == NULL) {
            width = 30;
        }
        else {
            width = parent->width - (2*margin) - (2*parent->padding);
        }
        
        GuiElement::drawBase();    
        
        _tft->drawCircle(newx + (boxSize / 2), midy, boxSize/2, ILI9341_WHITE);
        if (pressed())
            Serial.println("pressed");
        
        // erase or fill the bg
        if (pressed()) {
            // tick it
            _tft->fillCircle(newx + (boxSize / 2), midy, (boxSize/2) - 3, ILI9341_WHITE);
        }
        extraXOffset = 3 + fontSize() * 7;
        
        // draw all of the children now
        GuiLabel::draw(false);
    }
};

void GuiRadioList::addListElement(char const* text, uint16_t value) {
    Serial.print("w ");Serial.println(width);
    GuiRadioButton *label = new GuiRadioButton(0, 0, 30, 30, text);
    label->value(value);
    if (children.count == 0) {
        label->pressed(true);
    }
    addChild(label);
}
