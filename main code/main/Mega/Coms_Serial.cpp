

#ifndef COMS_SERIAL_CPP
#define COMS_SERIAL_CPP

#include "Coms_Serial.h"
#include "Coms.h"
#include "Graphics.h"

//volatile bool frame_arrived;   // flag from ISR to methods
byte temp_buffer[MEGA_SERIAL_BUFFER_LENGTH];

extern Coms_Serial coms_serial;

extern struct Frame text_frame;
extern struct Frame menu_frame;
extern struct Frame pos_frame;
extern struct Frame sensor_data_frame;

extern struct Text_Struct text_parameters[MAX_NUM_OF_TEXT_OBJECTS];


void serial_recieved_ISR() {


  if (Serial1.available() > 0) { //if something arrived

    coms_serial.read_buffer();  //loop through buffer and put it in temp_buffer

#ifdef DO_ERROR_CHECKING
    coms_serial.validate_checksum();
#endif

    //put the frame into correct location
    switch (temp_buffer[FRAME_TYPE_LOC]) {
      case TEXT_FRAME:
        coms_serial.received_text_frame();
        break;
      case POS_FRAME:
        coms_serial.received_pos_frame();
        break;
      case SENSOR_FRAME:
        coms_serial.received_sensor_frame();
        break;
      case MENU_FRAME:
        coms_serial.received_menu_frame();
        break;
      default: break;
    }

    //  char alpha;
    //  frame.error = 0;      //set error variable to zero for new frame
    //
    //
    //  frame.frame_length = Serial1.read(); // receive byte as a character
    //
    //  frame.frame_type = Serial1.read();
    //
    //  frame.num_frames = Serial1.read();
    //
    //  frame.frame_num = Serial1.read();
    //  frame.offset = (frame.frame_num - 1) * 32;
    //
    //
    //  frame.frame_str[frame.offset + 0] = (char)frame.frame_length;
    //  frame.frame_str[frame.offset + 1] = (char)frame.frame_type;
    //  frame.frame_str[frame.offset + 2] = (char)frame.num_frames;
    //  frame.frame_str[frame.offset + 3] = (char)frame.frame_num;
    //
    //  //calc checksum of first four elements read
    //  frame.error = frame.frame_str[frame.offset + 0] + frame.frame_str[frame.offset + 1] + frame.frame_str[frame.offset + 2] + frame.frame_str[frame.offset + 3];
    //
    //  frame.frame_len = frame.frame_len + 4;
    //  for (int i = 4; i < frame.frame_length; i++) { //loop through remaining data
    //    alpha = Serial1.read();
    //    frame.frame_len++;
    //    frame.frame_str[frame.offset + i] = (char)alpha;
    //    frame.error = frame.error + alpha;
    //
    //
    //  }   //if this is the last frame to be received in this batch, set this true, then other functions will be run
    //  if (frame.frame_str[frame.offset + 2] == frame.frame_str[frame.offset + 3]) {   //if frame_num==num_frames exit
    //    frame.frame_end = true;
    //  }
    //  Serial1.flush();

  }
}

void Coms_Serial::read_buffer() {
  byte counter = 0;
  while (Serial1.available() != 0) {
    temp_buffer[counter] = Serial1.read();
    counter++;
    if (counter == MEGA_SERIAL_BUFFER_LENGTH) break;   //break if read this many bytes, will over run array length next itteration
  }
}


//due to high chance of a frame arriving soon deal with the contents of these inside the isr
void Coms_Serial::received_text_frame() {
  byte obj_num = temp_buffer[4];//text obj number located at address 4
  //copy short temp buffer into correct location in text_parameters buffer
  memcpy(text_parameters[obj_num].string+(FRAME_DATA_LENGTH*(temp_buffer[FRAME_NUMBER_LOC]-1)), temp_buffer+HEADER_LENGTH, temp_buffer[FRAME_LENGTH_LOC]-FRAME_OVERHEAD);
}

void Coms_Serial::received_sensor_frame() {
//  memcpy(sensor_data_frame.frame_buffer, temp_buffer+HEADER_LENGTH, temp_buffer[FRAME_LENGTH_LOC]-FRAME_OVERHEAD);
  coms_serial.extract_sensor_data(&temp_buffer[0]);   //deal with this now
}

// These are resonably infrequenct. copy these into a temporary buffer and deal with data contents outside isr
void Coms_Serial::received_pos_frame() {
  memcpy(pos_frame.frame_buffer, temp_buffer+HEADER_LENGTH, temp_buffer[FRAME_LENGTH_LOC]-FRAME_OVERHEAD);
}

void Coms_Serial::received_menu_frame() {
  memcpy(menu_frame.frame_buffer, temp_buffer+HEADER_LENGTH, temp_buffer[FRAME_LENGTH_LOC]-FRAME_OVERHEAD);
}


#endif // COMS_SERIAL_CPP
