====================================================================================
Copyright (c) 2015 Barry Carter <barry.carter@gmail.com>


Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

A simple GUI Widget library for TFT screens. 
====================================================================================

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

//an external global reference to the tft.
extern Adafruit_ILI9341* _tft;

GuiElement::GuiElement(void) {
}

GuiElement::GuiElement(int16_t posX, int16_t posY, int16_t w, int16_t h)
{
    x = posX;
    y = posY;
    width = w;
    height = h;
}

// set enabled or not the for element. 
void GuiElement::enabled(bool isenabled) {
    if (GuiUtils::checkBit(&settingsFlags, ENABLED) == isenabled) return;

    GuiUtils::setBitValue(&settingsFlags, ENABLED, isenabled);
    
    // we don't draw unparented widgets. They are usually just being built
    if (parent != NULL)
        draw();
}

void GuiElement::transparent(bool istransparent) {
    if (GuiUtils::checkBit(&settingsFlags, ENABLED) == istransparent) return;

    GuiUtils::setBitValue(&settingsFlags, TRANSPARENT, istransparent);
    
    // we don't draw unparented widgets. They are usually just being built
    if (parent != NULL)
        draw();
}

bool GuiElement::visible(void) {
    // if the parent isn't visible, we shouldn't be
    if (settingsFlags & VISIBLE)
    {
        if (parent != NULL && !parent->visible())
            return false;
        return true;
    }
    return false;
}

// hide the element and stop any further processing
void GuiElement::visible(bool isvisible) {
    if (GuiUtils::checkBit(&settingsFlags, VISIBLE) == isvisible) return;

    GuiUtils::setBitValue(&settingsFlags, VISIBLE, isvisible);
    
    // we have to clear the area the element was in now
    _tft->fillRect(absoluteX(), absoluteY(), width, height, backgroundColourFromParent());
    
    // we don't draw unparented widgets. They are usually just being built
    if (parent != NULL)
        draw();
}

void GuiElement::pressed(bool ispressed) {
    if (GuiUtils::checkBit(&settingsFlags, PRESSED) == ispressed) return;
    
    GuiUtils::setBitValue(&settingsFlags, PRESSED, ispressed);
    
    // we don't draw unparented widgets. They are usually just being built
    if (parent != NULL)
        draw();
}

// calculate the actual position of this element
int16_t GuiElement::absoluteX() {
    return ((parent == NULL) ? (x + margin) : parent->absoluteX() + (x + margin));
}

int16_t GuiElement::absoluteY() {
    return ((parent == NULL) ? (y + margin) : parent->absoluteY() + (y + margin));
}

// draw the base of the widget. teh background, 
void GuiElement::drawBase(void) {
    if (GuiUtils::checkBit(&settingsFlags, VISIBLE) == false) return;
    // if the parent isn't visible, we shouldn't be
    if (parent != NULL && !parent->enabled()) return;
    
    bool isEnabled = GuiUtils::checkBit(&settingsFlags, ENABLED);
    
    if (!transparent() || !isEnabled) {
        _tft->fillRect(absoluteX(), absoluteY(), width, height, (isEnabled ? backgroundColour : ILI9341_LIGHTGREY));
    }
    else if (transparent()) {
        // it won't get drawn if there is no background on it, so we swipe it from the parent
        uint16_t colour = backgroundColourFromParent();
        _tft->fillRect(absoluteX(), absoluteY(), width, height, colour);
    }
    
    // draw the guide lines
//     uint16_t y = GuiUtils::getElementCentreY(this);
//     _tft->drawLine(absoluteX(), y, absoluteX() + width - margin, y, ILI9341_RED);
    
    // draw the outline border
    if (borderWidth > 0) {
        int16_t colour = borderColour;
        
        if (!enabled()) {
            colour = ILI9341_DARKGREY;
        }        
        _tft->drawRect(absoluteX(), absoluteY(), width, height, colour);
    }
}

void GuiElement::draw(void) {
    if (!visible()) return;
   
    GuiNode *tmphead = children.head();

    while (tmphead != NULL) {
        GuiElement* element = (GuiElement*)tmphead->element();
        element->draw();

        if (tmphead->next() == NULL)
            break;
            
        tmphead = tmphead->next();
    }
}

uint16_t GuiElement::backgroundColourFromParent(void) {
    GuiElement* el = parent;
    while (el != NULL) {
        if (parent->backgroundColour >= 0)
            break;
        el = el->parent;
    }
    if (el == NULL || backgroundColour < 0)
        return DEFAULT_COLOUR_BG;
    else
        return el->backgroundColour;
}

void GuiElement::addChild(GuiElement *child) {
    child->parent = this;   
    children.addElement((char*)child);
}

GuiElement* GuiElement::pointInWidget(int16_t x, int16_t y) { 
    // if the parent isn't visible, we shouldn't be
    if (parent != NULL && !parent->visible()) return NULL;
    if (GuiUtils::checkBit(&settingsFlags, VISIBLE) == false) return NULL;

    if ((x > absoluteX() && x < absoluteX() + width) &&
        (y > absoluteY() && y < absoluteY() + height)) {
                // we know the point is in this element somewhere, 
        // check all childrent to see if this event is for a child
        GuiElement* found = GuiUtils::pointInWidget(this, x, y);
        if (!found) {
            return this;
        }
        return found;
    }
    return NULL;
}

