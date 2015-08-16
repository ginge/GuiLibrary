/***************************************************
  A few labels with different text alignment along with a group of radio 
  buttons and also checkbox

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
#include "SdFat.h"

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

// we need these so we can show and hide them
GuiGrid* panelSettings;

// This will be called when the button is pressed or released
uint8_t callbackFunction_radio(void *a, GuiElement *element, uint8_t event) {
  if (event == GUI_EVENT_RELEASE) {
    Serial.print("Radio "); Serial.print(element->value()); Serial.println("Selected");
  }
  return 0;
}

// This will be called when the button is pressed or released
uint8_t callbackFunction_checkbox(void *a, GuiElement *element, uint8_t event) {
  if (event == GUI_EVENT_RELEASE) {
    if (element->pressed())
        Serial.println("Checkbox checked");
    else
        Serial.println("Checkbox un-checked");
  }
  return 0;
}

void buildPanelSettings() {
  // build a new panel and make it the same size as the screen
  // this panel will be hidden
  // Create a new grid, smaller than the last one. 2x2
  panelSettings = new GuiGrid(0, 0, 320, 240, 4, 4);
  panelSettings->padding = 0;
  panelSettings->margin = 0;

  // Some labs with different alignment
  GuiLabel *label = new GuiLabel(0, 0, 0, 0, "Label Default");
  GuiLabel *label1 = new GuiLabel(0, 0, 0, 0, "Centered");
  label1->textAlignH = TEXT_H_ALIGN_CENTRE;
  GuiLabel *label2 = new GuiLabel(0, 0, 0, 0, "Label Right");
  label2->textAlignH = TEXT_H_ALIGN_RIGHT;

  // our checkbox
  GuiCheckBox* checkBox = new GuiCheckBox(0,0,0,0, "Check Me!");
  checkBox->connectCallback(callbackFunction_checkbox);

  // Create a radio list and add 2 element
  GuiRadioList* radioList = new GuiRadioList(0,0,0,0); // the grid will sort the sizes out
  radioList->addListElement("Check 1", 1);
  radioList->addListElement("Check 2", 2);
  radioList->connectCallback(callbackFunction_radio);
  
  // add to grid
  panelSettings->addElement(label, 0, 0, 2, 1);
  panelSettings->addElement(label1, 0, 1, 2, 1);
  panelSettings->addElement(label2, 0, 2, 2, 1);
  panelSettings->addElement(radioList, 2, 0, 2, 4);

  panelSettings->addElement(checkBox, 2, 3, 2, 1);

  // finally add the widget to the page
  gui.addChild(panelSettings);
}

void setup(void) {
 // while (!Serial);     // used for leonardo debugging

  Serial.begin(115200);
  delay(1000);
  Serial.println(F("Gui Widgets test!"));

  // TFT is a go
  tft.begin();
  
  // in multiples of 90 only (duh)
  gui.setRotation(270);

  buildPanelSettings();

  gui.draw();

  return;  
}


void loop()
{
  // this has to be called to keep the system up to date and catch touch input
  gui.update();

  return;
}

