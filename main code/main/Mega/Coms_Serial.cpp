

#ifndef COMS_SERIAL_CPP
#define COMS_SERIAL_CPP

#include "Coms_Serial.h"
#include "Coms.h"
#include "Graphics.h"
#include "Host.h"
//volatile bool frame_arrived;   // flag from ISR to methods


extern Coms_Serial coms_serial;
extern Host host;


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
  Serial_1.begin(COMS_SPEED, COMS_CONGIF);
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

    if (Serial_1.read() == 13) {
      //Serial_1.read();
      if (Serial_1.peek() == 10) {
        Serial_1.read();
        delayMicroseconds(1000); //wait for some more bytes
        if (Serial_1.available() == 0) return;
        else {
          if (Serial_1.peek() == 13) return;
          while (Serial_1.available() < 4) {}
          delay(1);
        }
        Serial.println();
        if (Serial_1.available() > 0) { //just recieved start byte and data imediately after, probably frame
          //while(Serial_1.available() < 4){}
          //delay(3);
          byte temp_header[HEADER_LENGTH] = {0};
          byte bytes_read = 0;
          temp_header[0] = Serial_1.read();
          temp_header[1] = Serial_1.read();
          temp_header[2] = Serial_1.read();
          temp_header[3] = Serial_1.read();

          if (temp_header[0] == 13 && temp_header[1] == 10)
            return;

          //          //          read_frame
          //          byte bytes_read = Serial_1.readBytes(temp_header, HEADER_LENGTH);

          //          Serial.println("header values = ");
          //          for (byte i = 0; i < 4; i++)
          //            Serial.println(temp_header[i]);
          //          Serial.println();


#ifdef DO_HEADER_ERROR_CORRECTING   //if frame is encoded using hamming matrix, cant read data directly, decode first and update header values
          error_check_encoded_header(temp_header);
#endif
          //now sanity check values
          byte frame_type = error_check_unencoded_header(temp_header);

          //in some cases we know exactly how long the frame will be,in others we must parse until indicated amount is read
          if (frame_type == TEXT_FRAME_TYPE) {
            byte frame_length = APPLY_FRAME_LENGTH_MASK(temp_header[FRAME_LENGTH_LOC]);
            byte data[frame_length] = {0};

            bytes_read = Serial_1.readBytes(data, frame_length - HEADER_LENGTH);
            memmove(data + HEADER_LENGTH, data, bytes_read); //move elements back in frame
            memcpy(data, temp_header, HEADER_LENGTH);       //copy header to beginning

//            for (byte i = 0; i < sizeof(data); i++)
//              Serial.println(data[i]);

            if (!error_check_frame_body(data, frame_type, text_frame.frame_length)) //if frame ok, returns false(no errors), save data
              unpack_pos_frame(data);

            else {  //else failed to decode frame, but know header is reasonable, request the specific frame again
              byte this_frame = APPLY_THIS_FRAME_PARITY_MASK(temp_header[FRAME_NUMBER_LOC]);
              byte obj_num = APPLY_OBJ_NUM_PARITY_MASK(temp_header[OBJ_NUM_LOC]);
              request_frame_retransmission(TEXT_FRAME_TYPE, this_frame, obj_num);
              Serial.println(F("specific frame requested"));
            }
          }

          else if (frame_type == POS_FRAME_TYPE) {
            byte data[POS_FRAME_LENGTH] = {0};
            bytes_read = Serial_1.readBytes(data, POS_FRAME_LENGTH - HEADER_LENGTH);
            memmove(data + HEADER_LENGTH, data, bytes_read); //move elements back in frame
            memcpy(data, temp_header, HEADER_LENGTH);       //copy header to beginning

//            for (byte i = 0; i < sizeof(data); i++)
//              Serial.println(data[i]);

            if (!error_check_frame_body(data, frame_type, pos_frame.frame_length)) //if frame ok, save data
              unpack_pos_frame(data);

            else { //do nothing if pos frame recieved in error, new one coming soon
            }
          }

          else if (frame_type == SENSOR_FRAME_TYPE) {
            byte frame_length = APPLY_FRAME_LENGTH_MASK(temp_header[FRAME_LENGTH_LOC]);
            byte data[frame_length] = {0};
            bytes_read = Serial_1.readBytes(data, frame_length - HEADER_LENGTH);
            memmove(data + HEADER_LENGTH, data, bytes_read); //move elements back in frame
            memcpy(data, temp_header, HEADER_LENGTH);       //copy header to beginning

//            for (byte i = 0; i < sizeof(data); i++) {
//              Serial.print(data[i]);
//              Serial.print("\t");
//              host.println_bits(data[i],8, BIN);
//            }

            if (!error_check_frame_body(data, frame_type, sensor_data_frame.frame_length)) //if frame ok, save data
              unpack_sensor_data_frame(data);

            else {  //else failed to decode frame, but know header is reasonable, request the specific frame again
              byte this_frame = APPLY_THIS_FRAME_PARITY_MASK(temp_header[FRAME_NUMBER_LOC]);
              byte obj_num = APPLY_OBJ_NUM_PARITY_MASK(temp_header[OBJ_NUM_LOC]);
              request_frame_retransmission(SENSOR_FRAME_TYPE, this_frame, obj_num);
              Serial.println(F("specific frame requested"));
            }
          }

          else if (frame_type == MENU_FRAME_TYPE) {

            byte data[MENU_FRAME_LENGTH] = {0};
            bytes_read = Serial_1.readBytes(data, MENU_FRAME_LENGTH - HEADER_LENGTH);
            memmove(data + HEADER_LENGTH, data, bytes_read); //move elements back in frame
            memcpy(data, temp_header, HEADER_LENGTH);       //copy header to beginning

//            for (byte i = 0; i < sizeof(data); i++)
//              Serial.println(data[i]);

            if (!error_check_frame_body(data, frame_type, menu_frame.frame_length)) //if frame ok, save data
              unpack_menu_frame(data);

            else {  //else failed to decode frame, but know header is reasonable, request the specific frame again
              byte this_frame = APPLY_THIS_FRAME_PARITY_MASK(temp_header[FRAME_NUMBER_LOC]);
              byte obj_num = APPLY_OBJ_NUM_PARITY_MASK(temp_header[OBJ_NUM_LOC]);
              request_frame_retransmission(MENU_FRAME_TYPE, this_frame, obj_num);
              Serial.println(F("specific frame requested"));
            }
          }

          else if (frame_type == PING_STRING_TYPE) {
            byte data[PING_STRING_TYPE] = {0};
            bytes_read = Serial_1.readBytes(data, MENU_FRAME_LENGTH - HEADER_LENGTH);
            memmove(data + HEADER_LENGTH, data, bytes_read); //move elements back in frame
            memcpy(data, temp_header, HEADER_LENGTH);       //copy header to beginning

//            for (byte i = 0; i < sizeof(data); i++)
//              Serial.println(data[i]);

            if (!error_check_frame_body(data, frame_type, ping_frame.frame_length)) { //if frame ok, save data
              ping_good();
              Serial.println(F("ping good sent"));
            }
            else {  //else failed to decode frame, but know header is reasonable, request the specific frame again
              ping_bad();
              Serial.println(F("ping bad sent"));
            }
          }

          //          if (Serial_1.peek() == 13) {  //clear out remaining println
          //            Serial_1.read();
          //            if (Serial_1.peek() == 10)
          //              Serial_1.read();
          //          }
        }
      }
    }
  }
}

void Coms_Serial::receive_frame(byte * temp_buffer) { //header read, checks done, now read remaining bytes

  for (byte i = HEADER_LENGTH; i < MEGA_SERIAL_BUFFER_LENGTH; i++) {
    if (!byte_queued()) return;
    temp_buffer[i] = Serial_1.read();
  }
}

byte Coms_Serial::error_check_unencoded_header(byte * temp_buffer) {

  // do some sanity checks on the data, dont assume its correct
  // no encoding, other than checksum at the end, in this case
  // so just have to check if the values are reasonable


  //isolate data in case encoded but encoding not enabled at this end
  byte frame_length = APPLY_FRAME_LENGTH_MASK(temp_buffer[FRAME_LENGTH_LOC]);
  byte frame_type = APPLY_FRAME_TYPE_MASK(temp_buffer[FRAME_TYPE_LOC]);
  byte num_frames = APPLY_FRAME_NUM_MASK(temp_buffer[NUM_OF_FRAMES_LOC]); //do these here to save multiple calulations
  byte this_frame = APPLY_THIS_FRAME_MASK(temp_buffer[FRAME_NUMBER_LOC]);
  byte obj_num = APPLY_OBJ_NUM_MASK(temp_buffer[OBJ_NUM_LOC]);


  //  Serial.print(F("frame length = "));
  //  Serial.println(frame_length);
  //
  //  Serial.print(F("frame type = "));
  //  Serial.println(frame_type);
  //
  //  Serial.print(F("num frame = "));
  //  Serial.println(num_frames);
  //
  //  Serial.print(F("this frame = "));
  //  Serial.println(this_frame);
  //
  //  Serial.print(F("obj num = "));
  //  Serial.println(obj_num);
  //  Serial.println();

  // list of tests of header to check if its reasonable

  if (frame_length > MEGA_SERIAL_BUFFER_LENGTH || frame_length < FRAME_OVERHEAD)                          goto badframe; //out of bounds frame length
  else if (frame_type > NUM_ALLOWED_FRAME_TYPES)                                                          goto badframe; //out of bounds frame type
  else if (num_frames < this_frame)                                                                       goto badframe; //check if this frame is greater than num frames

  else if (frame_type == POS_FRAME_TYPE && frame_length != POS_FRAME_LENGTH)                              goto badframe; //if ping, menu or pos frame, we know how long they should be
  else if (frame_type == MENU_FRAME_TYPE && frame_length != MENU_FRAME_LENGTH)                            goto badframe;
  else if (frame_type == PING_STRING_TYPE && frame_length != PING_FRAME_LENGTH)                           goto badframe;

  else if (frame_type == POS_FRAME_TYPE && num_frames > EXPECTED_NUM_POS_FRAMES)                         goto badframe; // we know there should only be one of these frames
  else if (frame_type == MENU_FRAME_TYPE && num_frames > EXPECTED_NUM_MENU_FRAMES)                       goto badframe;
  else if (frame_type == PING_STRING_TYPE && num_frames > EXPECTED_NUM_PING_FRAMES)                      goto badframe;

  else if (frame_type == TEXT_FRAME_TYPE && num_frames > EXPECTED_MAX_TEXT_FRAMES)                       goto badframe;  //if text frame, we know the max size the text can be
  else if (frame_type == SENSOR_FRAME_TYPE && num_frames > EXPECTED_MAX_SENSOR_DATA_FRAMES)              goto badframe;  //if sensor frame, we know how many pieces of data there are
  else if (obj_num >= MAX_NUM_OF_TEXT_OBJECTS)                                                           goto badframe;  //check the obj num is reasonable

#ifdef DO_HEAVY_ERROR_CHECKING  //parity bits included, confirm these are right too

#endif

  //TESTS PASSED: frame header data plausable, proceed to save data
  else {
    if (frame_type == TEXT_FRAME_TYPE ) {
      text_frame.num_frames = num_frames;
      text_frame.num_frames = this_frame;
      text_frame.frame_length = frame_length;
    }
    else if (frame_type == POS_FRAME_TYPE ) {
      pos_frame.num_frames = num_frames;
      pos_frame.num_frames = this_frame;
      pos_frame.frame_length = frame_length;
    }
    else if (frame_type == SENSOR_FRAME_TYPE ) {
      sensor_data_frame.num_frames = num_frames;
      sensor_data_frame.num_frames = this_frame;
      sensor_data_frame.frame_length = frame_length;
    }
    else if (frame_type == MENU_FRAME_TYPE ) {
      menu_frame.num_frames = num_frames;
      menu_frame.num_frames = this_frame;
      menu_frame.frame_length = frame_length;
    }
    else if (frame_type == PING_STRING_TYPE ) {
      ping_frame.num_frames = num_frames;
      ping_frame.num_frames = this_frame;
      ping_frame.frame_length = frame_length;
    }
    else {  //frame type not accounted for, handle as bad frame
      Serial.println(F("Frame not recognised assume bad frame"));
      goto badframe;
    }

    return frame_type;    //only return non false value if frame header reasonable and frame type is defined
  }

  //do this when a bad frame if recieved
badframe:
  Serial.println(F("bad frame recieved"));
  request_frame_retransmission();
  Serial.println(F("frame requested"));
  return 0;
}

byte Coms_Serial::error_check_encoded_header(byte * temp_buffer) {

#ifdef DO_HEADER_ERROR_CORRECTING
  // run hamming decoding
  //check syndrome


#endif

  byte frame_length = APPLY_FRAME_LENGTH_MASK(temp_buffer[FRAME_LENGTH_LOC]);
  byte frame_type = APPLY_FRAME_TYPE_MASK(temp_buffer[FRAME_TYPE_LOC]);
  byte num_frames = APPLY_FRAME_NUM_MASK(temp_buffer[NUM_OF_FRAMES_LOC]); //do these here to save multiple calulations
  byte this_frame = APPLY_THIS_FRAME_MASK(temp_buffer[FRAME_NUMBER_LOC]);
  byte obj_num = APPLY_OBJ_NUM_MASK(temp_buffer[OBJ_NUM_LOC]);

#ifdef DO_HEAVY_ERROR_CHECKING
  //  check parity bits of header
  //  if error in more than two lines discard, cant recover
  //  else if error in frame_length, get code to read until endbyte,\r,\n found or MEGA_SERIAL_BUFFER_LENGTH reached
  //  else return frame_type
  Serial.println("check for errors");
  for ( byte i = 0; i < HEADER_LENGTH; i++) {
    if (parity_of(temp_buffer[i])) {
      Serial.println("odd parity");
      return 0;
    }
  }
  return (frame_type);

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
