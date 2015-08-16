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

GuiGrid::GuiGrid(int16_t x, int16_t y, int16_t width, int16_t height, uint8_t columns, uint8_t rows) 
    : GuiElement(x, y, width, height) { 
    columnCount = columns;       
    rowCount = rows;
};

void GuiGrid::addElement(GuiElement *element, uint8_t col, uint8_t row, uint8_t colspan, uint8_t rowspan) {
    uint16_t columnWidth = width / columnCount;
    uint16_t columnHeight = height / rowCount;
    // remap the x and y of the child
    element->x = (col * columnWidth);
    element->y = (row * columnHeight);
    GuiElement::addChild(element);
    element->width = (columnWidth * colspan) - (2 * element->margin);
    element->height = (columnHeight * rowspan) - (2 * element->margin);;
    
}

void GuiGrid::draw(void) {
    if (!visible()) return;    

    // draw the borders background, all that stuff
    GuiElement::drawBase();
    
    // draw all of the children now
    GuiElement::draw();
}


//////////////////////////
