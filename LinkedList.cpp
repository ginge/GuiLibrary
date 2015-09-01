/*====================================================================================
Copyright (c) 2015 Barry Carter <barry.carter@gmail.com>


Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

A simple GUI Widget library for TFT screens. 
====================================================================================
*/

#include "LinkedList.h"

void GuiList::addElement(char *element) {
    GuiNode *newNode = new GuiNode();
    newNode->setElement(element);
    newNode->setNext(NULL);
    
    GuiNode *tmphead = head_element;

    // if head is not empty, find the last and append
    if (tmphead != NULL) {
        // get the last element
        while (tmphead->next() != NULL) {
            tmphead = tmphead->next();
        }

        tmphead->setNext(newNode); // we got the last element. now point the last element at our new element
    }
    else {
        // the new node is now the head of the list
        head_element = newNode;
    }

    count++;
}

char* GuiList::getNthElement(uint8_t index) {
    GuiNode *tmphead = head_element;
    uint8_t count = 0;
    
    if (tmphead != NULL) {        
        while (tmphead != NULL) {
            if (count == index)
                return tmphead->element();

            if (tmphead->next() == NULL)
                break;
                
            tmphead = tmphead->next();
            count++;
        }
    }

    return NULL;
}