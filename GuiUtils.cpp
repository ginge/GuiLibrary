====================================================================================
Copyright (c) 2015 Barry Carter <barry.carter@gmail.com>


Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

A simple GUI Widget library for TFT screens. 
====================================================================================

#include "GuiLibrary.h"

#if USING_SD
#include <SdFat.h>
#endif

GuiElement* GuiUtils::pointInWidget(GuiElement* element, int16_t x, int16_t y) {
 // do collision detection
    GuiNode *tmphead = element->children.head();

    if (tmphead != NULL) {      
        while (tmphead != NULL) {
            GuiElement* element = (GuiElement*)tmphead->element();
            GuiElement *found = element->pointInWidget(x, y);
            // see if the point element returned is valid
            // if it is then we know the element returned the top level, but nt a child descendant
            if (found != NULL) {
                return found;
            }

            if (tmphead->next() == NULL)
                break;
                
            tmphead = tmphead->next();
        }
    }

    return NULL;
}

void GuiUtils::drawText(GuiElement* element, char const* text, uint16_t colour, int16_t x, int16_t y, uint8_t fontSize) {
    if (element->enabled())
        _tft->setTextColor(colour);
    else
        _tft->setTextColor(ILI9341_DARKGREY);
    
    _tft->setCursor(x, y);
    _tft->setTextSize(fontSize);
    _tft->println(text);
}

void GuiUtils::setBitValue(uint8_t* reg, uint8_t bit, uint8_t value) {
    if (value)
        setBit(reg, bit);
    else
        clearBit(reg, bit);
}

void GuiUtils::setBit(uint8_t* reg, uint8_t bit) {
    *reg |= bit;
}

void GuiUtils::clearBit(uint8_t* reg, uint8_t bit) {
    *reg &= ~(bit);
}

bool GuiUtils::checkBit(uint8_t* reg, uint8_t bit) {
    return *reg & bit;
}

// convert from RGB to RGR565 that the GFX lib uses
uint16_t GuiUtils::getColour(uint8_t red, uint8_t green, uint8_t blue) {
    return ((red / 8) << 11) | ((green / 4) << 5) | (blue / 8);
}

uint16_t GuiUtils::getElementCentreX(GuiElement* element) {
    return element->absoluteX() - element->margin + (element->width / 2);
}

uint16_t GuiUtils::getElementCentreY(GuiElement* element) {
    return element->absoluteY() - element->margin + (element->height / 2);
}


#if USING_SD
// This function opens a Windows Bitmap (BMP) file and
// displays it at the given coordinates.  It's sped up
// by reading many pixels worth of data at a time
// (rather than pixel by pixel).  Increasing the buffer
// size takes more of the Arduino's precious RAM but
// makes loading a little faster.  20 pixels seems a
// good balance.

#define BUFFPIXEL 20

void GuiUtils::bmpDraw(char const* filename, uint16_t x, uint16_t y) {
  SdFile   bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  
  if((x >= _tft->width()) || (y >= _tft->height())) return;

  // Open requested file on SD card
  if (!bmpFile.open(filename, O_READ)) {
    Serial.print(F("File not found"));
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
     Serial.print(F("File size: ")); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
//     Serial.print(F("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    Serial.print(F("Header size: ")); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
//       Serial.print(F("Bit Depth: ")); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
//         Serial.print(F("Image size: "));
//         Serial.print(bmpWidth);
//         Serial.print('x');
//         Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= _tft->width())  w = _tft->width()  - x;
        if((y+h-1) >= _tft->height()) h = _tft->height() - y;

        // Set TFT address window to clipped image bounds
        _tft->setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...

          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.curPosition() != pos) { // Need seek?
            bmpFile.seekSet(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each pixel...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format, push to display
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            _tft->pushColor(_tft->color565(r,g,b));
          } // end pixel
        } // end scanline
        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println(F("BMP format not recognized."));
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t GuiUtils::read16(SdFile &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t GuiUtils::read32(SdFile &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

#endif
