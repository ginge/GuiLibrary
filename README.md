====================================================================================
Copyright (c) 2015 Barry Carter <barry.carter@gmail.com>


Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

A simple GUI Widget library for TFT screens. 
====================================================================================

What
----

This is a library that allows you to draw simple widgets onto the screen in the
simplest possible way.
There are simple yet powerful grid and list layout options to let you get 
working straight away. Multiple screens of information are available by 
placing widgets on panels, and making the page you want to see visible, and the
rest invisible.
Callback functions can be used to trigger page changes, code execution or 
anything you want to do in your sketch.


Requires:
* Adafruit TFT and Adafruid GFX library
* Touch input library
* SdFat (for optional image support)


Supported widgets
* Label
* Panel
* Button
* Radio Group
* Checkbox
* ListBox
* Multi Line Text Box
* Image Box


Supported layout widgets
* Grid view (with rowspan and colspan)
* List views that auto place the elements
* Just place the widget where you want using x and y


supported features
* Widget margins and padding
* Text size and alignment (only one shitty font)
* Element borders
* Element background colours and font colours
* Icons for buttons
* Press/unpress states (where supported)
* Callback functions for element actions (press, release etc)
* Colours (background, border etc) are specified in normal good ol' RGB
* Easily create your own widget elements


Bad things
* While every effort has been made to keep this lightweight, it just isn't. 
  The library may not fit on an Arduino, tests required.
* Sometimes the elements on the scren overdraw. There is 0 dame detection
  and redraws in the library are crafted to keep redrawing while elements low
* The drawing engine is pretty dumb. If the text is bigger than your element,
  it will overflow. You will have to work that one out yourself for now.
* There is no other input except for touchscreen. You can't move around your 
  widgets with a joystick, say.
* It isn't going to win any screen redraw speed awards
* No scrollbars I guess thats up to you for a little while.


TODO:
* The header is a bit monolithic. It screams for a refactor
* Those linked lists could probably be thinned down
* you can't delete an element. Sorry!
* Scrollbars
* more fonts?
* Now we have primitives, some shiny widgets.
  * Progress bar
  * Dials
  * Etc


Example:
There are lots of examples int he examples folder.

Here is the minimum you need to get going

#include "GuiLibrary.h"
...
// Set our main window.
Gui gui(&tft, &ts, 0, 0, tft.height(), tft.width());

void setup() {
    GuiLabel *label = new GuiLabel(0, 0, 50, 30, "Hello World!");
    gui.addChild(label);
    gui.draw(); // set first draw going
}



Custom Widget:
You can create your own widgets really easily by taking on the attributes of another element and overriding it. This is called subclassing.

Example, lets create a new label that draws a line through the centre.

// create a new label of type GuiLabel and override the draw
class GuiLabelLine : public GuiLabel {
public:
    GuiLabelLine(int16_t x, int16_t y, int16_t width, int16_t height, char const* text) : GuiLabel(x, y, width, height, text) { };

    // This is the only function we need!
    void draw(void);
};

void GuiLabelLine::draw() {
    if (!visible()) return;
    
    // we want to draw the outlines and whatnot
    GuiElement::drawBase(); 

    GuiUtils::drawLine(absoluteX(), GuiUtils::getElementCentreY(), absoluteX() + width, GuiUtils::getElementCentreY(), GuiUtils::getColour(255, 0, 0));
    
    // draw the normal label
    GuiLabel::draw(false);
}

//////////////////////////


