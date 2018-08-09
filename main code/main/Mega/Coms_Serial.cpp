

#ifndef COMS_SERIAL_CPP
#define COMS_SERIAL_CPP

#include "Coms_Serial.h"
#include "Coms.h"
#include "Graphics.h"

//volatile bool frame_arrived;   // flag from ISR to methods


extern Coms_Serial coms_serial;

extern struct Frame text_frame;
extern struct Frame menu_frame;
extern struct Frame pos_frame;
extern struct Frame sensor_data_frame;
extern struct Frame ping_frame;
extern struct Text_Struct text_parameters[MAX_NUM_OF_TEXT_OBJECTS];


void serial_recieved_ISR() {


  if (Serial_1.available() > 0) { //if something arrived

    coms_serial.read_buffer();  //loop through buffer and put it in temp_buffer
    //
    //#ifdef DO_ERROR_CHECKING
    //    coms_serial.validate_checksum();
    //#endif
    //
    //    //put the frame into correct location
    //    switch (temp_buffer[FRAME_TYPE_LOC]) {
    //      case TEXT_FRAME:
    //        coms_serial.received_text_frame();
    //        break;
    //      case POS_FRAME:
    //        coms_serial.received_pos_frame();
    //        break;
    //      case SENSOR_FRAME:
    //        coms_serial.received_sensor_frame();
    //        break;
    //      case MENU_FRAME:
    //        coms_serial.received_menu_frame();
    //        break;
    //      default: break;
    //    }

    //  char alpha;
    //  frame.error = 0;      //set error variable to zero for new frame
    //
    //
    //  frame.frame_length = Serial_1.read(); // receive byte as a character
    //
    //  frame.frame_type = Serial_1.read();
    //
    //  frame.num_frames = Serial_1.read();
    //
    //  frame.frame_num = Serial_1.read();
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
    //    alpha = Serial_1.read();
    //    frame.frame_len++;
    //    frame.frame_str[frame.offset + i] = (char)alpha;
    //    frame.error = frame.error + alpha;
    //
    //
    //  }   //if this is the last frame to be received in this batch, set this true, then other functions will be run
    //  if (frame.frame_str[frame.offset + 2] == frame.frame_str[frame.offset + 3]) {   //if frame_num==num_frames exit
    //    frame.frame_end = true;
    //  }
    //  Serial_1.flush();

  }
}

void Coms_Serial::init_serial() {
  Serial_1.begin(COMS_SPEED);
}


inline bool Coms_Serial::byte_queued() {  // function to delay the processor a little if we catch up with current byte while reading

  if (Serial_1.available() == 0) { // wait until byte arrived, return false if not arrived in expected time
    delayMicroseconds(WAIT_FOR_BYTE_PERIOD);
    if (Serial_1.available() == 0)
      return false;
  }
  return true;
}

void Coms_Serial::read_buffer() {

  if (Serial_1.available() > 1) { //only create variables if data arrived, require start and first byte of frame

    if (Serial_1.peek() == 13) {
      Serial_1.read();
      if (Serial_1.peek() == 10) {
        Serial_1.read();
        byte_queued();
        //delay(1); //wait for some more bytes

        if (Serial_1.available() > 0) { //just recieved start byte and data imediately after, probably frame

          byte temp_header[HEADER_LENGTH] = {0};

          //          read_frame
          byte bytes_read = Serial_1.readBytes(temp_header, HEADER_LENGTH);
          if (bytes_read<HEADER_LENGTH)//if we read less than required amount before timeout this must not be a frame
          return;
          
#ifdef DO_HEADER_ERROR_CORRECTING   //if frame is encoded, cant read data directly, decode and sanity check in this funciton
          byte frame_type = error_check_encoded_header(temp_header);
#else
          byte frame_type = error_check_unencoded_header(temp_header);
#endif

          //in some cases we know exactly how long the frame will be,in others we must parse until indicated amount is read
          if (frame_type == TEXT_FRAME_TYPE) {

          }

          else if (frame_type == POS_FRAME_TYPE) {
            byte data[POS_FRAME_LENGTH] = {0};
            bytes_read = Serial_1.readBytes(data, POS_FRAME_LENGTH-HEADER_LENGTH);
            memmove(data+HEADER_LENGTH, data, bytes_read);  //move elements back in frame
            memcpy(data, temp_header, HEADER_LENGTH);       //copy header to beginning
            
            if(error_check_frame_body(data, frame_type, pos_frame.frame_length))  //if frame ok, save data
            unpack_pos_frame(data);
           
            else{//do nothing if pos frame recieved in error, new one coming soon
              
            }
            
            }

          else if (frame_type == SENSOR_FRAME_TYPE) {

          }

          else if (frame_type == MENU_FRAME_TYPE) {

          }

          else if (frame_type == PING_STRING_TYPE) {

          }


        }
      }
    }
  }
}

void Coms_Serial::receive_frame(byte *temp_buffer) { //header read, checks done, now read remaining bytes

  for (byte i = HEADER_LENGTH; i < MEGA_SERIAL_BUFFER_LENGTH; i++) {
    if (!byte_queued()) return;
    temp_buffer[i] = Serial_1.read();
  }
}

byte Coms_Serial::error_check_unencoded_header(byte *temp_buffer) {

  // do some sanity checks on the data, dont assume its correct
  // no encoding, other than checksum at the end, in this case
  // so just have to check if the values are reasonable


  //isolate data in case encoded but encoding not enabled at this end
  byte frame_length = APPLY_FRAME_LENGTH_MASK(temp_buffer[FRAME_LENGTH_LOC]);
  byte frame_type = APPLY_FRAME_TYPE_MASK(temp_buffer[FRAME_TYPE_LOC]);
  byte num_frames = APPLY_FRAME_NUM_MASK(temp_buffer[NUM_OF_FRAMES_LOC]); //do these here to save multiple calulations
  byte this_frame = APPLY_THIS_FRAME_MASK(temp_buffer[FRAME_NUMBER_LOC]);
  byte obj_num = APPLY_OBJ_NUM_MASK(temp_buffer[OBJ_NUM_LOC]);


  Serial.print("frame length = ");
  Serial.println(frame_length);

  Serial.print("frame type = ");
  Serial.println(frame_type);

  Serial.print("num frame = ");
  Serial.println(num_frames);

  Serial.print("this frame = ");
  Serial.println(this_frame);

  Serial.print("obj num = ");
  Serial.println(obj_num);
  Serial.println();


  // list of tests of header to check if its reasonable

  if (temp_buffer[FRAME_LENGTH_LOC] > MEGA_SERIAL_BUFFER_LENGTH || temp_buffer[FRAME_LENGTH_LOC] < FRAME_OVERHEAD)        goto badframe; //out of bounds frame length
  else if (temp_buffer[FRAME_TYPE_LOC] > NUM_ALLOWED_FRAME_TYPES)                                                         goto badframe; //out of bounds frame type
  else if (num_frames < this_frame)           goto badframe; //check if this frame is greater than num frames

  else if (temp_buffer[FRAME_TYPE_LOC] == POS_FRAME_TYPE && temp_buffer[FRAME_LENGTH_LOC] != POS_FRAME_LENGTH)            goto badframe; //if ping, menu or pos frame, we know how long they should be
  else if (temp_buffer[FRAME_TYPE_LOC] == MENU_FRAME_TYPE && temp_buffer[FRAME_LENGTH_LOC] != MENU_FRAME_LENGTH)          goto badframe;
  else if (temp_buffer[FRAME_TYPE_LOC] == PING_STRING_TYPE && temp_buffer[FRAME_LENGTH_LOC] != PING_FRAME_LENGTH)         goto badframe;

  else if (temp_buffer[FRAME_TYPE_LOC] == POS_FRAME_TYPE && num_frames > EXPECTED_NUM_POS_FRAMES)                         goto badframe; // we know there should only be one of these frames
  else if (temp_buffer[FRAME_TYPE_LOC] == MENU_FRAME_TYPE && num_frames > EXPECTED_NUM_MENU_FRAMES)                       goto badframe;
  else if (temp_buffer[FRAME_TYPE_LOC] == PING_STRING_TYPE && num_frames > EXPECTED_NUM_PING_FRAMES)                      goto badframe;

  else if (temp_buffer[FRAME_TYPE_LOC] == TEXT_FRAME_TYPE && num_frames > EXPECTED_MAX_TEXT_FRAMES)                       goto badframe;  //if text frame, we know the max size the text can be
  else if (temp_buffer[FRAME_TYPE_LOC] == SENSOR_FRAME_TYPE && num_frames > EXPECTED_MAX_SENSOR_DATA_FRAMES)              goto badframe;  //if sensor frame, we know how many pieces of data there are
  else if (obj_num >= MAX_NUM_OF_TEXT_OBJECTS)                                                                            goto badframe;  //check the obj num is reasonable

  //TESTS PASSED: frame header data plausable, proceed to save data
  else {
    if (temp_buffer[FRAME_TYPE_LOC] == TEXT_FRAME_TYPE ) {
      text_frame.num_frames = num_frames;
      text_frame.num_frames = this_frame;
      text_frame.frame_length = temp_buffer[FRAME_LENGTH_LOC];
    }
    else if (temp_buffer[FRAME_TYPE_LOC] == POS_FRAME_TYPE ) {
      pos_frame.num_frames = num_frames;
      pos_frame.num_frames = this_frame;
      pos_frame.frame_length = temp_buffer[FRAME_LENGTH_LOC];
    }
    else if (temp_buffer[FRAME_TYPE_LOC] == SENSOR_FRAME_TYPE ) {
      sensor_data_frame.num_frames = num_frames;
      sensor_data_frame.num_frames = this_frame;
      sensor_data_frame.frame_length = temp_buffer[FRAME_LENGTH_LOC];
    }
    else if (temp_buffer[FRAME_TYPE_LOC] == MENU_FRAME_TYPE ) {
      menu_frame.num_frames = num_frames;
      menu_frame.num_frames = this_frame;
      menu_frame.frame_length = temp_buffer[FRAME_LENGTH_LOC];
    }
    else if (temp_buffer[FRAME_TYPE_LOC] == PING_STRING_TYPE ) {
      ping_frame.num_frames = num_frames;
      ping_frame.num_frames = this_frame;
      ping_frame.frame_length = temp_buffer[FRAME_LENGTH_LOC];
    }
    else {  //frame type not accounted for, handle as bad frame
      Serial.println(F("Frame not recognised assume bas frame"));
      goto badframe;
    }

    return temp_buffer[FRAME_TYPE_LOC];    //only return non false value if frame header reasonable and frame type is defined
  }

  //do this when a bad frame if recieved
badframe:
  Serial.println(F("bad frame recieved"));
  return 0;
}

byte Coms_Serial::error_check_encoded_header(byte *temp_buffer) {

#ifdef DO_HEADER_ERROR_CORRECTING
  // run hamming decoding
  //check syndrome


#endif

#ifdef DO_HEAVY_ERROR_CHECKING
  //  check parity bits of header
  //  if error in more than two lines discard, cant recover
  //  else if error in frame_length, get code to read until endbyte,\r,\n found or MEGA_SERIAL_BUFFER_LENGTH reached
  //  else return frame_type

  for ( byte i = 0; i < HEADER_LENGTH; i++) {

  }

#endif


}



//
////due to high chance of a frame arriving soon deal with the contents of these inside the isr
//void Coms_Serial::received_text_frame(byte *temp_buffer) {
//  byte obj_num = temp_buffer[4];//text obj number located at address 4
//  //copy short temp buffer into correct location in text_parameters buffer
//  memcpy(text_parameters[obj_num].string + (FRAME_DATA_LENGTH * (temp_buffer[FRAME_NUMBER_LOC] - 1)), temp_buffer + HEADER_LENGTH, temp_buffer[FRAME_LENGTH_LOC] - FRAME_OVERHEAD);
//}
//
//void Coms_Serial::received_sensor_frame(byte *temp_buffer) {
//  //  memcpy(sensor_data_frame.frame_buffer, temp_buffer+HEADER_LENGTH, temp_buffer[FRAME_LENGTH_LOC]-FRAME_OVERHEAD);
//  coms_serial.extract_sensor_data(&temp_buffer[0]);   //deal with this now
//}
//
//// These are resonably infrequenct. copy these into a temporary buffer and deal with data contents outside isr
//void Coms_Serial::received_pos_frame(byte *temp_buffer) {
//  memcpy(pos_frame.frame_buffer, temp_buffer + HEADER_LENGTH, temp_buffer[FRAME_LENGTH_LOC] - FRAME_OVERHEAD);
//}
//
//void Coms_Serial::received_menu_frame(byte *temp_buffer) {
//  memcpy(menu_frame.frame_buffer, temp_buffer + HEADER_LENGTH, temp_buffer[FRAME_LENGTH_LOC] - FRAME_OVERHEAD);
//}


#endif // COMS_SERIAL_CPP
