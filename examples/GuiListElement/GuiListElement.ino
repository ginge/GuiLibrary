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

// This will be called when the button is pressed or released
uint8_t callbackFunction(void *a, GuiElement *element, uint8_t event) {
  Serial.print("Button was ");
  if (event == GUI_EVENT_PRESS) {
    Serial.println("Pressed");
    ((GuiButton*)element)->text("Pressed");
  }
  else if (event == GUI_EVENT_RELEASE) {
    Serial.println("Released");
    // the element was actually a button, so we can cast to that type and access the text setter
    ((GuiButton*)element)->text("Released");

    // we are going to make the second label element non-invis manually. we could have kept a reference to the element
    // and used it again here to access it directly, but this should illustrate how to walk te elements
    // WARNING: ADVANCED
    // short version. getnthElement looks at the children and returns index n
    // it returns a pointer to char, so you have to convert it back to the relevant widget type
    //    GuiElement* elementlist = (GuiElement*)(gui.children.getNthElement(0));
    //    GuiElement* element = (GuiElement*)(elementlist->children.getNthElement(1));
    //
    // broken down
    //
    // From the top level window, gui, get the first child.
    // we set the first child up before to be a list box which has more elements in it
    // here is how the element tree looks with the setup below
    //   gui---
    //        |--->GuiListElement   (child 0 of gui)
    //                    |---> Label (disabled)  (child 0 of GuiListElement)
    //                    |---> Label (invisible)  (child 1 of GuiListElement)
    //                    |---> Label (label normal)  (child 2 of GuiListElement)
    //                    |---> Button (handled in this function here)  (child 3 of GuiListElement)
    //
    // cool, so now we know how it looks, lets get them elements!
    // first get the element list. The function getNthElement returns a pointer. 
    // If you don't know what a pointer is, don't worry. Thats cool. Just follow the steps below. Monkey see, Monkey do, you'll be just fine.
    // Anyone reading this who knows c in depth will shoot me for this, but here goes...
    // The pointer is the address where your data is at. If, say, your data is in a warehouse, the pointer is the zip code to the warehouse.
    //
    // Get the list element.
    char* elementlist = gui.children.getNthElement(0);
    // great, we got the list element. Now lets get the second element in the list... Remember that invisible label we set up earlier
    // hello invisible label, we're coming for you!
    // NOTE: see how this time we use the "zip" code to the data element list above. When we do -> in code, it means go and get that data from that warehouse.
    char* element = ((GuiElement*)elementlist)->children.getNthElement(1);
    // element is now the zip code to the data in the invisible label. nice! But wait, I can't use that char*?
    // this is where we use the power of casting. At this point, the computer has no idea what data is in those warehouses, it just knows where that data is.
    // So... How do we fix that?
    // We tell the computer (specifically the compiler) what the data is!
    // we create a GuiLabel* in a variable elementConvertedToALabel and set that to the zip code we got from element
    // but see that (GuiLabel *) ? Well that is how we tell the computer that the data in the zip code of element is actually a label. booyah. 
    // (GuiLabel*)element;
    // (Convert the element "TheAwesomeEelement" to the type in these brackets)TheAwesomeEelement;
    GuiLabel* elementConvertedToALabel = (GuiLabel*)element;
    // now we can make it appear!
    elementConvertedToALabel->visible(true);    
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
  // first we want 3 labels
  GuiLabel *label = new GuiLabel(10, 0, 110, 40, "Disabled");
  label->borderWidth = 2;
  // make sure it is disabled
  label->enabled(false);
  
  GuiLabel *label1 = new GuiLabel(10, 0, 110, 40, "Invis");
  label1->borderWidth = 2;
  // this one is invisible.
  label1->visible(false);

  GuiLabel *label2 = new GuiLabel(10, 0, 110, 40, "Label");
  label2->borderWidth = 2;

  GuiButton *button = new GuiButton(10, 0, 110, 40, "Button!");

  GuiButton *button1 = new GuiButton(10, 0, 60, 20, "Small!");
    button1->fontSize(1);
    
  // hook up the callback function defined above to the button so we can track clicks
  button->connectCallback(callbackFunction);
  
  // create a element list box and add the elements
  GuiElementList *l = new GuiElementList(10, 0, 200, 200); 
  l->addChild(label);
  l->addChild(label1);
  l->addChild(label2);
  l->addChild(button);
  l->addChild(button1);

  // finally add the widgt to the page
  gui.addChild(l);
 
  gui.draw();

  return;  
}


void loop()
{
  // this has to be called to keep the system up to date
  gui.update();

  return;
}

