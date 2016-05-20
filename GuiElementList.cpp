/*====================================================================================
Copyright (c) 2015 Barry Carter <barry.carter@gmail.com>


Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

A simple GUI Widget library for TFT screens. 
====================================================================================
*/

#include "GuiLibrary.h"

void GuiElementList::addChild(GuiElement *child) {
    // remap the x and y of the child
    child->y += totalHeight + child->margin + padding;
    totalHeight = absoluteY() + child->y + child->height + child->margin + padding;
    height = totalHeight;

    GuiElement::addChild(child);
};

GuiElement* GuiElementList::pointInWidget(int16_t x, int16_t y) {
    GuiElement* element = GuiUtils::pointInWidget(this, x, y);
    if (element != NULL) {
        selectedElement = element;
        return this;
    }
    return NULL;
}

void GuiElementList::selectElement(GuiElement* element, bool skipredraw) {
    GuiElement* backupParent = parent;
    
    // deselect all elements apart from the last selected element
    GuiNode *tmphead = children.head();

    while (tmphead != NULL) {
        GuiElement* element = (GuiElement*)tmphead->element();
        
        // the callback wants to go somewhere else, or otherwise doesn't want a redraw.
        // we are going to cheat here and tell this widget it has no parent, set the flags, and then restore the parent
        // TODO maybe bring back those nodraw options to the setters
        if (skipredraw)
            parent = NULL;
        
        if (element != selectedElement)
            element->pressed(false);
        else {
            element->pressed(true);
        }

        parent = backupParent;

        if (tmphead->next() == NULL)
            break;
            
        tmphead = tmphead->next();
    }
}


// process the callback. This invokes the callback by calling the handle, 
// and using the processEventPointer to send back the scope
uint8_t GuiElementList::callbackProcess(processEvent_t processEventHandle, void* processEventPointer, GuiElement *element, uint8_t event) {
    if (!visible() || !enabled())
        return 0;
    
    if (event == GUI_EVENT_PRESS && selectedElement) {
        selectElement(selectedElement, true);
        // chain the callback
        selectedElement->callbackProcess(processEventHandle, processEventPointer, selectedElement, event);
    }    
    
    if (processEventHandle != NULL)
        processEventHandle(NULL, selectedElement, event);
    
//     draw();

    // this return is for the c++ version of the callbacks
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
