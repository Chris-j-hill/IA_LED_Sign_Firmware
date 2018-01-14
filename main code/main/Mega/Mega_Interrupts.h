
// File to contain all interrupt related functions, including inits.
// arduino doesnt handle interrupts as class methods well, keep them as seperate functions

#ifndef Mega_Interrupts_H
#define Mega_Interrupts_H

#include "Arduino.h"

#include "due_structs.h"

#include "function_declarations.h"

#include "libraries.h"



void receiveEvent(){


 if (Serial1.available()>31){    //if something arrived

  //Serial.println(F("Data recieved"));
  //digitalWrite(13, !digitalRead(13));

  char alpha;
  frame.error = 0;      //set error variable to zero for new frame


  frame.frame_length = Serial1.read(); // receive byte as a character

  frame.frame_type = Serial1.read();

  frame.num_frames = Serial1.read();

  frame.frame_num = Serial1.read();
  frame.offset = (frame.frame_num - 1) * 32;


  frame.frame_str[frame.offset + 0] = (char)frame.frame_length;
  frame.frame_str[frame.offset + 1] = (char)frame.frame_type;
  frame.frame_str[frame.offset + 2] = (char)frame.num_frames;
  frame.frame_str[frame.offset + 3] = (char)frame.frame_num;

  //calc checksum of first four elements read
  frame.error = frame.frame_str[frame.offset + 0] + frame.frame_str[frame.offset + 1] + frame.frame_str[frame.offset + 2] + frame.frame_str[frame.offset + 3];

  frame.frame_len = frame.frame_len + 4;
  for (int i = 4; i < frame.frame_length; i++) { //loop through remaining data
    alpha = Serial1.read();
    frame.frame_len++;
    frame.frame_str[frame.offset + i] = (char)alpha;
    frame.error = frame.error + alpha;


  }   //if this is the last frame to be received in this batch, set this true, then other functions will be run
  if (frame.frame_str[frame.offset + 2] == frame.frame_str[frame.offset + 3]) {   //if frame_num==num_frames exit
    frame.frame_end = true;
  }
  Serial1.flush();

 }
}

#endif



#endif // Mega_Interupts_H
