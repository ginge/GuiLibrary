/***************************************************
  Example showing a list box and a selected item panel

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

class DataBlock {
public:
    DataBlock(uint8_t theid, char const* fname, char const *lname, uint8_t theage) { id = theid; lastName = lname; firstName = fname; age = theage; };
    uint8_t id = 0;
    char const* firstName;
    char const* lastName;
    uint8_t age;
};

DataBlock* dataBlocks[3];

GuiLabel* labelNameValue = new GuiLabel(10, 35, 170, 40, "Select");
GuiLabel* labelAgeValue = new GuiLabel(10, 120, 80, 40, "Select");


// This will be called when the button is pressed or released
uint8_t callbackFunction(void *a, GuiElement *element, uint8_t event) {
  Serial.print("Button was ");
  if (event == GUI_EVENT_PRESS) {
    Serial.println("Pressed");
  }
  else if (event == GUI_EVENT_RELEASE) {
    Serial.println("Released");

    Serial.print("The value of the selected element is ");
    Serial.println(element->value());

    DataBlock* block = dataBlocks[element->value() - 1];
    char name[strlen(block->firstName) + strlen(block->lastName) + 2];

    strcpy(name, block->firstName);
    strcat(name, " ");
    strcat(name, block->lastName);
    labelNameValue->text(name);

    String s = String(block->age);
    char arr[4];
    s.toCharArray(arr, 4);
    labelAgeValue->text(arr);
  }
    
  return 0;
}


void setup(void) {
  // while (!Serial);     // used for leonardo debugging

  // initialise the fake data for this demo
  dataBlocks[0] = new DataBlock(1, "Robert", "Smith", 27);
  dataBlocks[1] = new DataBlock(2, "Steve", "Jones", 42);
  dataBlocks[2] = new DataBlock(3, "John", "Wheeler", 35);


  Serial.begin(115200);
  delay(1000);
  Serial.println(F("Gui Widgets test!"));
  
  tft.begin();
  // in multiples of 90 only (duh)
  gui.setRotation(270);

  // build some widgets
  GuiListBox *listbox = new GuiListBox(10, 0, 100, 230);

  for (uint8_t i = 0; i < 3; i++) {
    listbox->addListElement(dataBlocks[i]->firstName, dataBlocks[i]->id);
  }
  
  // hook up the callback function defined above to the button so we can track clicks
  listbox->connectCallback(callbackFunction);


  // now lets setup the right hand panel
  // we want a few labels to display this stuff.
  // first we want a panel
  GuiPanel *panel = new GuiPanel(120, 0, 190, 230);
  panel->borderWidth = 2;
  GuiLabel* labelName = new GuiLabel(10, 0, 65, 40, "Name:");
  GuiLabel* labelAge = new GuiLabel(10, 80, 65, 40, "Age:");
  
  panel->addChild(labelName);
  panel->addChild(labelNameValue);
  panel->addChild(labelAge);
  panel->addChild(labelAgeValue);
  
  // finally add the widgt to the page
  gui.addChild(listbox);
  gui.addChild(panel);
  
  gui.draw();

  return;  
}


void loop()
{
  // this has to be called to keep the system up to date
  gui.update();

  return;
}

