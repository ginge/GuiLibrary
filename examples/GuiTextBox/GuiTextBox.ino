/***************************************************
  Example showing a list of elements, some enabled, some disabled some touch enabled

  NOTE: screeen rotation is set to 270 degrees. Don't worry, the touchscreen will map
  correctly.

  Uses Adafruit TFT library for the example:
  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

/** NOT FOR USE WITH THE TOUCH SHIELD, ONLY FOR THE BREAKOUT! **/

#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>
#include <Adafruit_ILI9341.h>
#include "TouchScreen.h"
#include "GuiLibrary.h"

// These are the four touchscreen analog pins
#define YP A0  // must be an analog pin, use "An" notation!
#define XM A1  // must be an analog pin, use "An" notation!
#define YM 6   // can be a digital pin
#define XP 5   // can be a digital pin

// This is calibration data for the raw touch data to the screen coordinates
#define TS_MINX 150
#define TS_MINY 120
#define TS_MAXX 920
#define TS_MAXY 940


// The display uses hardware SPI, plus #9 & #10
#define TFT_CS 4
#define TFT_DC 5
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Setup the gui model
// needs reference to the tft and the touchscreen
Gui gui(&tft, &ts, 0, 0, tft.height(), tft.width());

GuiMultiLineTextBox* textbox = new GuiMultiLineTextBox(150, 0, 100, 200);

// This will be called when the button is pressed or released
uint8_t callbackFunction(void *a, GuiElement *element, uint8_t event) {
  Serial.print("Button was ");
  if (event == GUI_EVENT_PRESS) {
    Serial.println("Pressed");
    ((GuiButton*)element)->text("Pressed");
    textbox->addLine("Pressed Button");
  }
  else if (event == GUI_EVENT_RELEASE) {
    Serial.println("Released");
    // the element was actually a button, so we can cast to that type and access the text setter
    ((GuiButton*)element)->text("Released");
    textbox->addLine("Released Button");
  }
    
  return 0;
}


void setup(void) {
 // while (!Serial);     // used for leonardo debugging

  Serial.begin(115200);
  delay(1000);
  Serial.println(F("Gui Widgets test!"));
  
  tft.begin();
  // in multiples of 90 only (duh)
  gui.setRotation(270);

  // build some widgets
  GuiButton *button = new GuiButton(10, 0, 110, 40, "Button!");
    
  // hook up the callback function defined above to the button so we can track clicks
  button->connectCallback(callbackFunction);
  
  // finally add the widgt to the page
  gui.addChild(button);

  // now the log output box
  
  gui.addChild(textbox);
 
  gui.draw();

  return;  
}


void loop()
{
  // this has to be called to keep the system up to date
  gui.update();

  return;
}

