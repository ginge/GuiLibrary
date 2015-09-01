/***************************************************
  Example showing a the power of grids and icons to make nice looking modernui tiles
  Click on settings for screen 2.
  Serial will also print some stuff for your pleasure
  Requires SdFat library

  NOTE: screeen rotation is set to 270 degrees. Don't worry, the touchscreen will map
  correctly.

  Uses Adafruit TFT library for the example:
  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

/** NOT FOR USE WITH THE TOUCH SHIELD, ONLY FOR THE BREAKOUT! **/
// Make sure the SD code is enabled for loading images
#define USING_SD 1

#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>
#include <Adafruit_ILI9341.h>
#include "TouchScreen.h"
#include "GuiLibrary.h"
#include <SdFat.h>

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
#define SD_CS 8

SdFat sd;

Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

// For better pressure precision, we need to know the resistance
// between X+ and X- Use any multimeter to read it
// For the one we're using, its 300 ohms across the X plate
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

// Setup the gui model
// needs reference to the tft and the touchscreen
Gui gui(&tft, &ts, 0, 0, tft.height(), tft.width());

// some global references to the menu panels
// we need these so we can show and hide them
GuiGrid* panelMenu;
GuiGrid* panelSettings;

// list of buttons
GuiButton deviceButtons[5];

// pointer to the selected output device
GuiElement* selectedElement = NULL;

// This will be called when the button is pressed or released
uint8_t callbackFunction_home(void *a, GuiElement *element, uint8_t event) {
  if (event == GUI_EVENT_RELEASE) {
    Serial.println("Pressed Back");
    panelSettings->visible(false);
    panelMenu->visible(true);
  }
  return 1; // tell the gui engine we are going to be moving away and not to redraw
}

// This will be called when the button is pressed or released
uint8_t callbackFunction_device(void *a, GuiElement *element, uint8_t event) {
  if (event == GUI_EVENT_RELEASE) {
    Serial.println("Pressed Device");
    //deselect the old selection
    if (selectedElement != NULL) {
      selectedElement->backgroundColour = GuiUtils::getColour(20, 109, 201);
      selectedElement->draw();
    }

    // select the new selection
    element->backgroundColour = GuiUtils::getColour(0, 159, 60);
    selectedElement = element;
  }
  return 0;
}

uint8_t callbackFunction_next(void *a, GuiElement *element, uint8_t event) {
  if (event == GUI_EVENT_RELEASE) {
    Serial.println("Pressed Next");
  }
  return 1;
}

void buildPanelMenu() {
  // build a 6x3 grid of elements
  panelMenu = new GuiGrid(0, 0, 320, 240, 6, 3);
  panelMenu->padding = 0;
  panelMenu->margin = 0;

  // 6 buttons, 5 kb shortcuts, one for "next"
//  GuiButton *button0 = new GuiButton(0, 0, 0, 0, "1");
//  GuiButton *button1 = new GuiButton(0, 0, 0, 0, "2");
//  GuiButton *button2 = new GuiButton(0, 0, 0, 0, "3");
//  GuiButton *button3 = new GuiButton(0, 0, 0, 0, "4");
//  GuiButton *button4 = new GuiButton(0, 0, 0, 0, "5");
  GuiButton *next = new GuiButton(0, 0, 0, 0, "More...");

  int row = 0;
  int col = 0;

  for (int i = 0; i < 5; i++) {
    deviceButtons[i].borderWidth = 0;
    deviceButtons[i].image("sc32/configure_alt_3_mirror.bmp");
    deviceButtons[i].backgroundColour = GuiUtils::getColour(20, 109, 201);
    deviceButtons[i].connectCallback(callbackFunction_device);

    // add to the grid
    if (i > 2)
        row = 1;
    col = 2*(i%3);
    panelMenu->addElement(&deviceButtons[i], col, row, 2, 1);
  }
  
  next->borderWidth = 0;  
  next->image("sc32/configure_alt_3_mirror.bmp");
  next->backgroundColour = GuiUtils::getColour(223, 0, 35);
  next->connectCallback(callbackFunction_next);
  
//  // A bunch of buttons for the small grid
//  GuiButton *button4 = new GuiButton(0, 0, 0, 0, "");
//  button4->backgroundColour = GuiUtils::getColour(0, 159, 60);
//  button4->image("sc32/system_restore_mirror.bmp");
//  button4->imageAlignH = TEXT_H_ALIGN_CENTRE;
//  button4->borderWidth = 0;
//
//  GuiButton *button5 = new GuiButton(0, 0, 0, 0, "");
//  button5->backgroundColour = GuiUtils::getColour(20, 109, 201);
//  button5->image("sc32/signal_mirror.bmp");
//  button5->imageAlignH = TEXT_H_ALIGN_CENTRE;
//  button5->borderWidth = 0;
//
//  GuiButton *button6 = new GuiButton(0, 0, 0, 0, "");
//  button6->backgroundColour = GuiUtils::getColour(223, 0, 35);
//  button6->image("sc32/mail_alt_mirror.bmp");
//  button6->imageAlignH = TEXT_H_ALIGN_CENTRE;
//  button6->borderWidth = 0;

  // add the elements into the grid. Each element it set to the grid by row and column
  // element, col, row, column span, row span
  panelMenu->addElement(next, 4, 1, 2, 1);
  
  // finally add the widgt to the page
  gui.addChild(panelMenu);
}

void buildPanelSettings() {
  // build a new panel and make it the same size as the screen
  // this panel will be hidden
  // Create a new grid, smaller than the last one. 2x2
  panelSettings = new GuiGrid(0, 0, 320, 240, 2, 2);
  panelSettings->padding = 0;
  panelSettings->margin = 0;

  // put a couple of controls on the demo page
  GuiLabel *label = new GuiLabel(0, 0, 0, 0, "System Status: Mauve");

  // A big icon on the button
  GuiButton *button1 = new GuiButton(0, 0, 0, 0, "");
  button1->backgroundColour = GuiUtils::getColour(163,98,10); // blue
  button1->connectCallback(callbackFunction_home);
  button1->image("sc64/40405_mirror.bmp");
  button1->imageAlignH = TEXT_H_ALIGN_CENTRE;
  button1->imageSize = 64;
  button1->borderWidth = 0;

  // add to grid
  panelSettings->addElement(label, 0, 0, 2, 1);
  panelSettings->addElement(button1, 0, 1, 1, 1);

  // make it invisible before it is added to the gui
  panelSettings->visible(false);
  
  // finally add the widget to the page
  gui.addChild(panelSettings);
}

void setup(void) {
 // while (!Serial);     // used for leonardo debugging

  Serial.begin(115200);
  delay(1000);
  Serial.println(F("Gui Widgets test!"));

  // SD is a go
  if (!sd.begin(SD_CS, SPI_HALF_SPEED)) {
    Serial.println("Begin failed");
    return;
  }

  // TFT is a go
  tft.begin();
  
  // in multiples of 90 only (duh)
  gui.setRotation(270);

  // create our GUI elements
  buildPanelMenu();
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

