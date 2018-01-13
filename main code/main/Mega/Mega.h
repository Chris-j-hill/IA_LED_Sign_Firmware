#ifndef Mega_H
#define MEGA_H

#include "libraries.h"                //file containing all 3rd party libraries used
#include "Pins.h"                     //list of pins used in each board
#include "function_declarations.h"    //any functions that need prototypes
#include "Global_variables.h"         //all global variables defined here
#include "due_structs.h"              //structs for a variety of aspects of the sign (fans, encoder, text etc)

#include "Interrupts.h"               //interrupt functions, arduino doesnt like them in classes so created seperately 

#include "base_class.h"               //classes
#include "due_class.h"
#include "mega_class.h"

mega_class mega;
//RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE1, OE2, false, 64, screen_mode);
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE2, false, 64);

  
void mega_setup(){
    mega.get_address();
    Serial.println(address);
    
    if (!mega_enabled[address-1]){    //stop code if this address disabled
      while(1){}
    }
    
// 
// Serial.println(F("wait for due"));
// pinMode(due_ready_pin, INPUT_PULLUP);
// bool ready= false;
// while(!ready && due_enabled){    //wait until due is ready only if due enabled
//    //ready = !digitalRead(due_ready_pin);    //if pin goes low break loop
//    Serial.println(digitalRead(due_ready_pin));
// }
// Serial.println(F("due_ready, init mega"));
// 
    mega.init_mega(address);
    frame.disp_str.reserve(150);
//    Wire.begin(address);                
//    Wire.onReceive(receiveEvent);       // register event
//  
    matrix.begin();
    matrix.setTextWrap(false); // Allow text to run off right edge
    matrix.setTextSize(2);
    pinMode(13, OUTPUT);
    
    
//    mega.startup_handshake();

//    frame.disp_str = "this is a test string";
   // frame.disp_str.remove(15);
//    Serial.println(mega.text_min());
//    Serial.println((5-address)*matrix.width());
  }
  
  
void mega_loop(){
  
int prev_time = millis();
    while(1){
     receiveEvent();
      if(frame.frame_end){   //if new data arrived
      frame.frame_end = false;
        mega.extract_data();
      }

         if (millis()>prev_time+1000){
          prev_time = millis();
        Serial.print("X pos: ");
        Serial.print(x_pos);
        Serial.print("\t Y pos: ");
        Serial.print(y_pos);
        
        Serial.print("\t String: ");
      Serial.println(frame.disp_str);
    }
//      delay(5);
//      matrix.fillScreen(0);
//      matrix.fillCircle(40, 21, 10, matrix.Color333(7, 0, 7));
  
//      // Draw big scrolly text on top
//      matrix.setTextColor(matrix.ColorHSV(text_colour_hue, 255, 255, true));
//      matrix.setCursor(x_pos-64*(address-1), y_pos);
//      matrix.setTextColor(matrix.Color333(7,7,7));
//      matrix.print(frame.disp_str);
//    
//      // Move text left (w/wrap), increase hue
//      x_pos--;
//      if(x_pos < mega.text_min()) 
//      x_pos = (5-address)*matrix.width();
//      text_colour_hue += 7;
//      if(text_colour_hue >= 1536) text_colour_hue -= 1536;
//    
//      // Update display
   //   matrix.swapBuffers(false);
//      // digitalWrite(13, !digitalRead(13));
//       delay(100);
//       Serial.print(x_pos);
//       Serial.print("\t");
//       Serial.println(y_pos);
    }
   
  }
  
 #endif	//MEGA_H