/*====================================================================================
Copyright (c) 2015 Barry Carter <barry.carter@gmail.com>


Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

A simple GUI Widget library for TFT screens. 
====================================================================================
*/

#ifndef _GUILIBRARY_H_
#define _GUILIBRARY_H_
#include <stdint.h>
#include "LinkedList.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <GuiConfig.h.template>
#include "TouchScreen.h"

// sometimes we want to disable this
//#define USING_SD 1

#if USING_SD
#include <SdFat.h>
#endif

// This is calibration data for the raw touch data to the screen coordinates
#ifndef TS_MINX
#define TS_MINX 150
#endif
#ifndef TS_MINY
#define TS_MINY 120
#endif
#ifndef TS_MAXX
#define TS_MAXX 920
#endif
#ifndef TS_MAXY
#define TS_MAXY 940
#endif

#define MINPRESSURE 15
#define MAXPRESSURE 1000

// debounce delay for button presses on crappy touchsreens
#define DEBOUNCE_DELAY 50

// Event types for button presse etc
#define GUI_EVENT_PRESS   0
#define GUI_EVENT_RELEASE 1
#define GUI_EVENT_DRAG    2


// settings flags to save memory
#define ENABLED     0
#define VISIBLE     1
#define TRANSPARENT 2
#define PRESSED     3

#define COLOR_BLACK       0x0000      /*   0,   0,   0 */
#define COLOR_NAVY        0x000F      /*   0,   0, 128 */
#define COLOR_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define COLOR_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define COLOR_MAROON      0x7800      /* 128,   0,   0 */
#define COLOR_PURPLE      0x780F      /* 128,   0, 128 */
#define COLOR_OLIVE       0x7BE0      /* 128, 128,   0 */
#define COLOR_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define COLOR_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define COLOR_BLUE        0x001F      /*   0,   0, 255 */
#define COLOR_GREEN       0x07E0      /*   0, 255,   0 */
#define COLOR_CYAN        0x07FF      /*   0, 255, 255 */
#define COLOR_RED         0xF800      /* 255,   0,   0 */
#define COLOR_MAGENTA     0xF81F      /* 255,   0, 255 */
#define COLOR_YELLOW      0xFFE0      /* 255, 255,   0 */
#define COLOR_WHITE       0xFFFF      /* 255, 255, 255 */
#define COLOR_ORANGE      0xFD20      /* 255, 165,   0 */
#define COLOR_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define COLOR_PINK        0xF81F

// default colours
#define DEFAULT_COLOUR_BG COLOR_DARKGREY
#define DEFAULT_COLOUR_FG COLOR_BLACK
#define DEFAULT_COLOUR_BORDER COLOR_WHITE


class GuiElement {
public:
    typedef uint8_t (*processEvent_t)(void *p, GuiElement* element, uint8_t event); // anonymous type to function pointer
    GuiElement(void);
    GuiElement(int16_t x, int16_t y, int16_t width, int16_t height);
    
    int16_t x, y, width, height;  // size and position related
    int8_t margin = 1;
    int8_t padding = 3;
    uint8_t borderWidth = 0;
    uint16_t borderColour = DEFAULT_COLOUR_BORDER;
    uint16_t backgroundColour = DEFAULT_COLOUR_BG;
    uint16_t foregroundColour = DEFAULT_COLOUR_FG;
    
    bool enabled() { return (settingsFlags >> ENABLED) & 1; }
    void enabled(bool isenabled);
    bool visible();
    void visible(bool isvisible);
    bool pressed() { return (settingsFlags >> PRESSED) & 1; }
    void pressed(bool ispressed);
    bool transparent() { return (settingsFlags >> TRANSPARENT) & 1; }
    void transparent(bool istransparent);
    
    GuiList children;
    virtual void addChild(GuiElement *child); // Add a child node to this node

    // get the absolute position of the control
    int16_t absoluteX();
    int16_t absoluteY();

    // do point bound checking
    virtual GuiElement* pointInWidget(int16_t x, int16_t y); // does this point exist in this widget?

    // button and callback
    void connectCallback(processEvent_t callback, void *p) { processEventHandle = callback; processEventPointer = p; } // c++ classes (for class to class callbacks)
    void connectCallback(processEvent_t callback) { processEventHandle = callback; processEventPointer = NULL; } // c classes (use in a sketch for example

    // allow events from the input device (key/mouse etc) to be triggered in this event handler
    void triggerEvent(uint8_t event) { callbackProcess(this, event); };

    virtual void draw(void);  // needs calling after a change. most state changes will deal with this for you
    void drawBase(void); // draw the outline

    virtual uint8_t callbackProcess(GuiElement *element, uint8_t event) { callbackProcess(processEventHandle, processEventPointer, element, event); return 0; };
    virtual uint8_t callbackProcess(processEvent_t processEventHandle, void* processEventPointer, GuiElement *element, uint8_t event) { return 0; };

    GuiElement* parent = NULL;  // the element this is attached to
    uint16_t backgroundColourFromParent(void); // used to get the background colour the widget is sat on
    
    int16_t value() { return dataValue; } // elements can hold an arbitary data tag
    void value(int16_t newvalue) { dataValue = newvalue; }

private:   
    
    int16_t dataValue = 0; // an arbitary value reference
    // button press callback related
    processEvent_t processEventHandle = NULL;
    void* processEventPointer = NULL;
    
    uint8_t settingsFlags = (1 << VISIBLE) | (1 << ENABLED) | (1 << TRANSPARENT);
};

///////////////////

#define TEXT_H_ALIGN_LEFT   0
#define TEXT_H_ALIGN_CENTRE 1
#define TEXT_H_ALIGN_RIGHT  2

class GuiLabel : public GuiElement {
public:
    GuiLabel(void) : GuiElement(0, 0, 0, 0) { labelText = ""; };
    GuiLabel(int16_t x, int16_t y, int16_t width, int16_t height, char const* text): GuiElement(x, y, width, height) { labelText = text; };

    uint8_t textAlignH = TEXT_H_ALIGN_LEFT;
    char const* text() { return labelText; }
    void text(char const* text) { labelText = text; draw(); }

    void fontSize(uint8_t size) { labelFontSize = size; }
    uint8_t fontSize() { return labelFontSize; }
    void draw();
    void draw(bool andBase);
    uint8_t extraXOffset = 0;
private:
    char const* labelText = NULL;
    uint8_t labelFontSize = 2;
};

//////////////////

class GuiElementList : public GuiElement {
public:
    GuiElementList(void) : GuiElement(0, 0, 0, 0) {};
    GuiElementList(int16_t x, int16_t y, int16_t width, int16_t height) : GuiElement(x, y, width, height) {};

    void addChild(GuiElement* child);
    GuiElement* pointInWidget(int16_t x, int16_t y);
    
    uint8_t callbackProcess(processEvent_t processEventHandle, void* processEventPointer, GuiElement *element, uint8_t event);
    GuiElement* selectedElement = NULL;
    
    void selectElement(GuiElement* element, bool skipredraw);
private:
    uint16_t totalHeight = 0;
};


//////////////////

class GuiButton : public GuiLabel {
public:
    GuiButton(void) : GuiLabel(0, 0, 0, 0, "") { borderWidth = 1; textAlignH = TEXT_H_ALIGN_CENTRE; transparent(false); };
    GuiButton(int16_t x, int16_t y, int16_t width, int16_t height, char const* text) : GuiLabel(x, y, width, height, text) { borderWidth = 1; textAlignH = TEXT_H_ALIGN_CENTRE; transparent(false); };

    void draw(void);
    
    void image(char const* imageFileNameBmp) { imageFileName = imageFileNameBmp; }
    char const* image(void) { return imageFileName; }
    uint8_t imageAlignH = TEXT_H_ALIGN_LEFT;
    uint16_t imageSize = 32;

    uint8_t callbackProcess(processEvent_t processEventHandle, void* processEventPointer, GuiElement *element, uint8_t event);
private:
    char const* imageFileName = NULL;
};

//////////////

class GuiCheckBox : public GuiLabel {
public:
    GuiCheckBox(void) : GuiLabel(0, 0, 0, 0, "") { borderWidth = 0; textAlignH = TEXT_H_ALIGN_LEFT; transparent(true); };
    GuiCheckBox(int16_t x, int16_t y, int16_t width, int16_t height, char const* text) : GuiLabel(x, y, width, height, text) { borderWidth = 0; textAlignH = TEXT_H_ALIGN_LEFT; transparent(true); };

    void draw(void);

    uint8_t callbackProcess(processEvent_t processEventHandle, void* processEventPointer, GuiElement *element, uint8_t event);
private:

};


//////////////////

extern Adafruit_GFX* _tft;
extern uint8_t _TFT_CS;

#if USING_SD
extern SdFat* _sd;
#endif

class Gui : public GuiElement {
public:
    Gui(void);
    Gui(Adafruit_GFX* thetft, TouchScreen* thets, int16_t x, int16_t y, int16_t width, int16_t height)
        : GuiElement(x, y, width, height) { _tft = thetft; ts = thets; borderWidth = 0; margin = 0; backgroundColour = DEFAULT_COLOUR_BG; transparent(false); }; // usually the size of you tft. i.e. (0,0,320,240)
    void setRotation(int16_t degrees);
    
    void update(void);    
    
    void draw(void);
    TouchScreen* ts;

private:
    void sendEventToWidget(int16_t x, int16_t y, int8_t eventid);
    int16_t rotation = 0;
};

////////////////////

class GuiListBox : public GuiElementList {
public:
    GuiListBox(void);
    GuiListBox(int16_t x, int16_t y, int16_t width, int16_t height) : GuiElementList(x, y, width, height) {};

    void addListElement(char const* text, uint16_t value);
    
private:
    uint16_t totalHeight = 0;
};

////////////////////
class GuiRadioList : public GuiElementList {
public:
    GuiRadioList(void);
    GuiRadioList(int16_t x, int16_t y, int16_t width, int16_t height) : GuiElementList(x, y, width, height) {};
    void addListElement(char const* text, uint16_t value);
    
private:
    uint16_t totalHeight = 0;
};

////////////////////
class GuiPanel : public GuiElement {
public:
    GuiPanel(void);
    GuiPanel(int16_t x, int16_t y, int16_t width, int16_t height) : GuiElement(x, y, width, height) {};
    void draw(void);
};

////////////////////

class GuiMultiLineTextBox : public GuiElementList {
public:
    GuiMultiLineTextBox(void);
    GuiMultiLineTextBox(int16_t x, int16_t y, int16_t width, int16_t height) : GuiElementList(x, y, width, height) { backgroundColour = COLOR_WHITE; transparent(false); };
    
    void addChild(GuiElement *child);
    void addLine(char const* text);
    void clear();
    
private:
    uint16_t totalHeight = 0;
};

////////////////////
class GuiGrid : public GuiElement {
public:
    GuiGrid(void);
    GuiGrid(int16_t x, int16_t y, int16_t width, int16_t height, uint8_t columns, uint8_t rows);
    void draw(void);
    void addElement(GuiElement *element, uint8_t col, uint8_t row, uint8_t rowspan, uint8_t colspan);
private:
    uint8_t columnCount = 0;
    uint8_t rowCount = 0;
};

////////////////////
class GuiUtils {
public:
    static GuiElement* pointInWidget(GuiElement* element, int16_t x, int16_t y);
    static void drawText(GuiElement* element, char const* text, uint16_t colour, int16_t x, int16_t y, uint8_t fontSize);
    
    static uint16_t getColour(uint8_t red, uint8_t green, uint8_t blue);
    
    // find centre lines in an eleemnt
    static uint16_t getElementCentreX(GuiElement* element);
    static uint16_t getElementCentreY(GuiElement* element);
    
    #if USING_SD
    // SD Card and bitmap
    static void bmpDraw(char const* filename, uint16_t x, uint16_t y);
    static uint16_t read16(SdFile &f);
    static uint32_t read32(SdFile &f);
    #endif
};









#endif

