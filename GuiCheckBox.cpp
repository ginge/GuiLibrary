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
  
// process the callback. This invokes the callback by calling the handle, 
// and using the processEventPointer to send back the scope
uint8_t GuiCheckBox::callbackProcess(processEvent_t processEventHandle, void* processEventPointer, GuiElement *element, uint8_t event) {
    if (!visible() || !enabled())
        return 0;
   
    GuiElement* backupParent = parent;
    
    // the callback wants to go somewhere else, or otherwise doesn't want a redraw.
    // we are going to cheat here and tell this widget it has no parent, set the flags, and then restore the parent
    // TODO maybe bring back those nodraw options to the setters
//     if (ret > 0)
//         parent = NULL;
    
    // when the finger comes away from the checkbox, we toggle state
    if (event == GUI_EVENT_RELEASE) {
        pressed(!(pressed()));
    }

        // this return is for the c++ version of the callbacks, for a c callback the EventPointer will be null    
    if (processEventHandle != NULL)
        processEventHandle(processEventPointer, element, event);

//     parent = backupParent;
    
    return 0;

    /*
     * Here is the caller side implementation. it needs a static class wrapper adding to the caller
     * which when called uses casting to cast back to the stored source class
     * 
     *  static uint8_t processEvent(void *p, GuiElement *element, uint8_t event) {
     *      return ((CallBackSourceClass *)p)->callbackFunction(element, event);
     *  }
     *
     *  where the callback function is defined as
     *  uint8_t callbackFunction(GuiElement *element, uint8_t event);
     *  
     *  don't forget to connect the callback
     *  
     *  button.connectCallback(processEvent, this); <-- note we pass the static function
     *
     */
}


void GuiCheckBox::draw(void) {
    if (!visible()) return;    

    int16_t newx = absoluteX() + padding;
    int16_t midy = GuiUtils::getElementCentreY(this);
    int16_t midFontHeight = (fontSize() * 7)/2;
    int8_t boxSize = fontSize() * 7;
    
    GuiElement::drawBase();    
    
    _tft->drawRect(newx, midy - midFontHeight, boxSize, boxSize, ILI9341_WHITE);
    
    // erase or fill the bg
    if (pressed()) {
        // tick it
        _tft->drawLine(newx, midy - midFontHeight, newx + boxSize - 1, midy + (boxSize /2) - 1, ILI9341_WHITE);
        _tft->drawLine(newx + boxSize - 1, midy - midFontHeight, newx, midy + (boxSize /2) - 1, ILI9341_WHITE);
    }
    extraXOffset = 5 + fontSize() * 7;
    
    // draw all of the children now
    GuiLabel::draw(false);

}
