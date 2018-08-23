

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

#define START_BYTE_1 251
#define START_BYTE_2 252
#define END_BYTE_1 253
#define END_BYTE_2 254



void Coms_Serial::init_serial() {
  Serial_1.begin(COMS_SPEED, COMS_CONGIF);
  int timeout = ceil((10000 << 2) / COMS_SPEED); //wait for up to four bytes periods before timeout
  Serial_1.setTimeout(timeout);
}


inline bool Coms_Serial::byte_queued() {  // function to delay the processor a little if we catch up with current byte while reading

  if (Serial_1.available() == 0) { // wait until byte arrived, return false if not arrived in expected time
    delayMicroseconds(WAIT_FOR_BYTE_PERIOD);
    if (Serial_1.available() == 0)
      return false;
  }
  return true;
}

//void Coms_Serial::read_buffer() {
//
//  TIMSK1 &= ~(1 << TOIE1);  //disable timer 1 interrupt (timer for screen)
//
//  if (Serial_1.available() > 1) { //only create variables if data arrived, require start and first byte of frame
//
//    if (Serial_1.read() == 13) {
//      if (Serial_1.peek() == 10) {
//        Serial_1.read();
//        delayMicroseconds(1000); //wait for some more bytes
//        if (Serial_1.available() == 0) return;
//        else {
//          if (Serial_1.peek() == 13) return;  // if next byte is 13 then shouldnt be a frame lengh byte due to btye length being encoded
//          while (Serial_1.available() < 4) {} // wait until full header arrived
//          delay(1);
//        }
//
//        if (Serial_1.available() > 0) { //just recieved start byte and data imediately after, probably frame
//
//          byte temp_header[HEADER_LENGTH] = {0};
//          byte bytes_read = 0;
//          temp_header[0] = Serial_1.read();
//          temp_header[1] = Serial_1.read();
//          temp_header[2] = Serial_1.read();
//          temp_header[3] = Serial_1.read();
//
//          if (temp_header[0] == 13 && temp_header[1] == 10)
//            return;
//
//          bool encoding_ok = true;
//
//#ifdef DO_HEADER_ERROR_CORRECTING   //if frame is encoded using hamming matrix, cant read data directly, decode first and update header values
//          encoding_ok = error_check_encoded_header(temp_header);
//
//#endif
//          byte frame_type = 0;
//
//          //now sanity check values
//          if (encoding_ok)
//            frame_type = error_check_unencoded_header(temp_header);//<- returns zero if fails sanity checks
//
//          if (frame_type == 0 || !encoding_ok) {  //if either of the above tests failed request retransmission of a bunch of frames, cant trust header data
//            Serial.println(F("frame header error"));
//#ifndef DISABLE_REQUEST_RETRANSMISSION
//            request_frame_retransmission();
//            Serial.println(F("bunch of frames requested"));
//#endif
//          }
//
//          //in some cases we know exactly how long the frame will be,in others we must parse until indicated amount is read
//          else if (frame_type == TEXT_FRAME_TYPE) {
//            byte frame_length = APPLY_FRAME_LENGTH_MASK(temp_header[FRAME_LENGTH_LOC]);
//            byte data[frame_length] = {0};
//
//            bytes_read = Serial_1.readBytes(data, frame_length - HEADER_LENGTH);
//            memmove(data + HEADER_LENGTH, data, bytes_read); //move elements back in frame
//            memcpy(data, temp_header, HEADER_LENGTH);       //copy header to beginning
//
//
//            if (!error_check_frame_body(data, frame_type, text_frame.frame_length)) { //if frame ok, returns false(no errors), save data
//              remove_byte_parity_bit(data, BYTE_PARITY_LOC, text_frame.frame_length - TRAILER_LENGTH, HEADER_LENGTH);
//              frame_cpy(data, TEXT_FRAME_TYPE);
//              Serial.println(F("Text frame recieved"));
//            }
//
//#ifndef DISABLE_REQUEST_RETRANSMISSION
//            else {  //else failed to decode frame, but know header is reasonable, request the specific frame again
//              byte this_frame = APPLY_THIS_FRAME_PARITY_MASK(temp_header[FRAME_NUMBER_LOC]);
//              byte obj_num = APPLY_OBJ_NUM_PARITY_MASK(temp_header[OBJ_NUM_LOC]);
//              request_frame_retransmission(TEXT_FRAME_TYPE, this_frame, obj_num);
//              Serial.println(F("specific frame requested"));
//            }
//#else
//            else
//              Serial.println(F("text frame error"));
//#endif
//          }
//
//          else if (frame_type == POS_FRAME_TYPE) {
//            byte data[POS_FRAME_LENGTH] = {0};
//            bytes_read = Serial_1.readBytes(data, POS_FRAME_LENGTH - HEADER_LENGTH);
//            memmove(data + HEADER_LENGTH, data, bytes_read); //move elements back in frame
//            memcpy(data, temp_header, HEADER_LENGTH);       //copy header to beginning
//
//            if (!error_check_frame_body(data, frame_type, pos_frame.frame_length)) { //if frame ok, save data
//              frame_cpy(data, POS_FRAME_TYPE);
//              Serial.println(F("Pos frame recieved"));
//            }
//
//
//#ifndef DISABLE_REQUEST_RETRANSMISSION
//            else { //do nothing if pos frame recieved in error, new one coming soon
//            }
//#else
//            else
//              Serial.println(F("pos frame error"));
//#endif
//          }
//
//          else if (frame_type == SENSOR_FRAME_TYPE) {
//            byte frame_length = APPLY_FRAME_LENGTH_MASK(temp_header[FRAME_LENGTH_LOC]);
//            byte data[frame_length] = {0};
//            bytes_read = Serial_1.readBytes(data, frame_length - HEADER_LENGTH);
//            memmove(data + HEADER_LENGTH, data, bytes_read); //move elements back in frame
//            memcpy(data, temp_header, HEADER_LENGTH);       //copy header to beginning
//
//            if (!error_check_frame_body(data, frame_type, sensor_data_frame.frame_length)) { //if frame ok, save data
//              frame_cpy(data, SENSOR_FRAME_TYPE);
//              Serial.println(F("Sensor data frame recieved"));
//            }
//
//
//#ifndef DISABLE_REQUEST_RETRANSMISSION
//            else {  //else failed to decode frame, but know header is reasonable, request the specific frame again
//              byte this_frame = APPLY_THIS_FRAME_PARITY_MASK(temp_header[FRAME_NUMBER_LOC]);
//              byte obj_num = APPLY_OBJ_NUM_PARITY_MASK(temp_header[OBJ_NUM_LOC]);
//              request_frame_retransmission(SENSOR_FRAME_TYPE, this_frame, obj_num);
//              Serial.println(F("specific frame requested"));
//            }
//#else
//            else
//              Serial.println(F("sensor frame error"));
//#endif
//          }
//
//          else if (frame_type == MENU_FRAME_TYPE) {
//
//            byte data[MENU_FRAME_LENGTH] = {0};
//            bytes_read = Serial_1.readBytes(data, MENU_FRAME_LENGTH - HEADER_LENGTH);
//            memmove(data + HEADER_LENGTH, data, bytes_read); //move elements back in frame
//            memcpy(data, temp_header, HEADER_LENGTH);       //copy header to beginning
//
//            if (!error_check_frame_body(data, frame_type, menu_frame.frame_length)) { //if frame ok, save data
//              frame_cpy(data, MENU_FRAME_TYPE);
//              Serial.println(F("Menu frame recieved"));
//            }
//
//#ifndef DISABLE_REQUEST_RETRANSMISSION
//            else {  //else failed to decode frame, but know header is reasonable, request the specific frame again
//              byte this_frame = APPLY_THIS_FRAME_PARITY_MASK(temp_header[FRAME_NUMBER_LOC]);
//              byte obj_num = APPLY_OBJ_NUM_PARITY_MASK(temp_header[OBJ_NUM_LOC]);
//              request_frame_retransmission(MENU_FRAME_TYPE, this_frame, obj_num);
//              Serial.println(F("specific frame requested"));
//            }
//
//#else
//            else
//              Serial.println(F("menu frame error"));
//#endif
//          }
//
//          else if (frame_type == PING_STRING_TYPE) {
//            byte data[PING_STRING_TYPE] = {0};
//            bytes_read = Serial_1.readBytes(data, MENU_FRAME_LENGTH - HEADER_LENGTH);
//            memmove(data + HEADER_LENGTH, data, bytes_read); //move elements back in frame
//            memcpy(data, temp_header, HEADER_LENGTH);       //copy header to beginning
//
//            Serial.println(F("Ping frame recieved"));
//            if (!error_check_frame_body(data, frame_type, ping_frame.frame_length)) { //if frame ok, save data
//              ping_good();
//              Serial.println(F("ping good sent"));
//            }
//            else {  //else failed to decode frame, but know header is reasonable, request the specific frame again
//              ping_bad();
//              Serial.println(F("ping bad sent"));
//            }
//          }
//        }
//      }
//    }
//  }
//  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
//
//}

void Coms_Serial::read_buffer() {

  TIMSK1 &= ~(1 << TOIE1);  //disable timer 1 interrupt (timer for screen)

  bool seen_byte_1 = false;
  bool seen_byte_2 = false;
  byte break_condition = 0;
  uint16_t delay_period = ceil(40000000 / COMS_SPEED); //wait long enough for 4 bytes to arrive

  //  if (Serial_1.available() > 1) {
  //    byte val = Serial_1.read();
  //    Serial.print(val);
  //    Serial.print(F(" "));
  //    if (val == 253 && Serial_1.peek() == 254)
  //      Serial.println(Serial_1.read());
  //
  //  }
  //  return;

  if (Serial_1.available() > 1) {
    if (Serial_1.available() > 0 && Serial_1.peek() == START_BYTE_1) {
      Serial_1.read();
      seen_byte_1 = true;
    }

seen_byte_1:    //jump label if we discover were lost after reading above byte

    //  if (Serial_1.available() == 0)
    //    delayMicroseconds(delay_period);

    if (Serial_1.available() > 0 && Serial_1.peek() == START_BYTE_2 && seen_byte_1) { //expect to see second start byte, cant recover if missed first byte
      Serial_1.read();
      seen_byte_2 = true;
    }

seen_byte_2:

    if (Serial_1.available() > 0 && !seen_byte_1 && !seen_byte_2) {
      // byte avialable and neither above conditions caught it
      // could be endbyte, or middle of frame, either way, just discard char

      Serial.print(Serial_1.read());
      Serial.print(" ");
    }

    if (seen_byte_1 && seen_byte_2) {
      //in this funciton pass we have found both bytes 1 and 2
      //almost certainly a frame start
      Serial.println();
      byte header[HEADER_LENGTH];
      seen_byte_1 = false;
      seen_byte_2 = false;
      break_condition = 0;

      for (byte i = 0; i < HEADER_LENGTH; i++) {
        if (Serial_1.available() == 0)  //if no char available, delay a bit
          delayMicroseconds(delay_period);

        if (Serial_1.available() == 0) { //if still no char available, something wrong, return
          Serial.println(F("no header byte arrived"));
          return;
        }
        byte temp = Serial_1.read();

        if (temp == START_BYTE_1 && Serial_1.peek() == START_BYTE_2) {
          seen_byte_1 = true;
          break_condition = 1;
          break;
        }
        else if (temp == END_BYTE_1 && Serial_1.peek() == END_BYTE_2) {
          break_condition = 2;
          break;
        }

        header[i] = temp;
      }


      if (break_condition != 0) { //finished loop early, why...
        if (break_condition == 1) goto seen_byte_1; //seen start bytes
        else if (break_condition == 2) {
          Serial.println(F("endbytes found in header"));
          return;   //seen endbytes, leave whole function, prepare to start again
        }
      }

      else { // read header ok, decode and sanity check

        bool encoding_ok = true;
        //#ifdef DO_HEADER_ERROR_CORRECTING   //if frame is encoded using hamming matrix, cant read data directly, decode first and update header values
        encoding_ok = error_check_encoded_header(header);
        //#endif

        byte frame_type = 0;
        //now sanity check values
        if (encoding_ok)
          frame_type = error_check_unencoded_header(header);//<- returns zero if fails sanity checks

        if (frame_type == 0 || !encoding_ok) {
          Serial.println(F("frame header error"));
//          Serial.print(F("frame_type : "));
//          Serial.println(frame_type);
//          Serial.print(F("encoding ok : "));
//          Serial.println(encoding_ok);

#ifndef DISABLE_REQUEST_RETRANSMISSION
          request_frame_retransmission();
          Serial.println(F("bunch of frames requested"));
#endif

          for (byte i = 0; i < HEADER_LENGTH; i++) {
            Serial.print(header[i]);
            Serial.print(" ");
          }
        }

        else { //header ok read frame

          break_condition = 0;
          byte frame_length = APPLY_FRAME_LENGTH_MASK(header[FRAME_LENGTH_LOC]);
          byte this_frame = APPLY_THIS_FRAME_PARITY_MASK(header[FRAME_NUMBER_LOC]);
          byte obj_num = APPLY_OBJ_NUM_PARITY_MASK(header[OBJ_NUM_LOC]);
          byte data[frame_length] = {0};
          byte byte_read = 0;

          //loop to read frame data, checking for start of end frame char pairs
          for (byte i = HEADER_LENGTH; i < frame_length; i++) {

            if (Serial_1.available() == 0)  //if nothing available, wait
              delayMicroseconds(delay_period);

            if (Serial_1.available() == 0) {//nothing arrived, request frame again then exit
              Serial.println(F("no frame data arrived"));
              break_condition = 3;
              break;
            }

            else {//byte available, read
              byte_read = Serial_1.read();
            }
            //now check for start or end bytes
            if (byte_read == START_BYTE_1 && Serial_1.peek() == START_BYTE_2) { // probaly start of a frame, abandon this one and jump back
              seen_byte_1 = true;
              break_condition = 1;
              break;
            }
            else if (byte_read == END_BYTE_1 && Serial_1.peek() == END_BYTE_2) {
              break_condition = 2;
              Serial.println(F("end bytes found in frame data"));
              break;
            }

            data[i] = byte_read; //think byte ok, save byte
          }

          if (break_condition != 0) { //failed to read frame fully, but know header so can request specific frame
#ifndef DISABLE_REQUEST_RETRANSMISSION
            request_frame_retransmission(frame_type, this_frame, obj_num);
            Serial.println(F("dropped byte, frame requested"));
#endif
            //process break condition
            if (break_condition == 1) goto seen_byte_1; // seen start bytes
            else if (break_condition == 2) return;      // seen endbytes, leave whole function, prepare to start again
            else if (break_condition == 3) return;      // byte didnt arrive
          }

          else if (frame_type == TEXT_FRAME_TYPE) { //frame read fully and is text frame
            memcpy(data, header, HEADER_LENGTH);       //copy header to beginning of frame

            if (!error_check_frame_body(data, TEXT_FRAME_TYPE, frame_length)) { //if frame ok, returns false(no errors), save data
              remove_byte_parity_bit(data, BYTE_PARITY_LOC, text_frame.frame_length - TRAILER_LENGTH, HEADER_LENGTH);
              frame_cpy(data, TEXT_FRAME_TYPE);
              Serial.println(F("Text frame recieved"));
            }
#ifndef DISABLE_REQUEST_RETRANSMISSION
            else { //failed parity checks
              request_frame_retransmission(TEXT_FRAME_TYPE, this_frame, obj_num);
              Serial.println(F("specific text frame requested"));
            }
#else
            else
              Serial.println(F("text frame error"));
#endif
          }

          else if (frame_type == POS_FRAME_TYPE) {
            memcpy(data, header, HEADER_LENGTH);

            if (!error_check_frame_body(data, POS_FRAME_TYPE, frame_length)) {
              frame_cpy(data, POS_FRAME_TYPE);
              Serial.println(F("Pos frame recieved"));
            }
#ifndef DISABLE_REQUEST_RETRANSMISSION
            else { //failed parity checks
              request_frame_retransmission(POS_FRAME_TYPE, this_frame, obj_num);
              Serial.println(F("specific pos frame requested"));
            }
#else
            else
              Serial.println(F("pos frame error"));
#endif
          }

          else if (frame_type == SENSOR_FRAME_TYPE) { //frame read fully and is text frame
            memcpy(data, header, HEADER_LENGTH);       //copy header to beginning of frame

            if (!error_check_frame_body(data, SENSOR_FRAME_TYPE, frame_length)) { //if frame ok, returns false(no errors), save data
              frame_cpy(data, SENSOR_FRAME_TYPE);
              Serial.println(F("Sensor data frame recieved"));
            }
#ifndef DISABLE_REQUEST_RETRANSMISSION
            else { //failed parity checks
              request_frame_retransmission(SENSOR_FRAME_TYPE, this_frame, obj_num);
              Serial.println(F("specific sensor data frame requested"));
            }
#else
            else
              Serial.println(F("sensor data frame error"));
#endif
          }




          else if (frame_type == MENU_FRAME_TYPE) { //frame read fully and is text frame
            memcpy(data, header, HEADER_LENGTH);       //copy header to beginning of frame

            if (!error_check_frame_body(data, MENU_FRAME_TYPE, frame_length)) { //if frame ok, returns false(no errors), save data
              frame_cpy(data, MENU_FRAME_TYPE);
              Serial.println(F("menu frame recieved"));
            }
#ifndef DISABLE_REQUEST_RETRANSMISSION
            else { //failed parity checks
              request_frame_retransmission(MENU_FRAME_TYPE, this_frame, obj_num);
              Serial.println(F("specific menu frame requested"));
            }
#else
            else
              Serial.println(F("menu frame error"));
#endif
          }

          else if (frame_type == PING_STRING_TYPE) { //frame read fully and is text frame
            memcpy(data, header, HEADER_LENGTH);       //copy header to beginning of frame

            if (!error_check_frame_body(data, PING_STRING_TYPE, frame_length)) { //if frame ok, returns false(no errors), save data
              ping_good();
              Serial.println(F("ping good sent"));
            }
#ifndef DISABLE_REQUEST_RETRANSMISSION
            else { //failed parity checks
              ping_bad();
              Serial.println(F("ping bad sent"));
            }
#else
            else
              Serial.println(F("ping frame error"));
#endif
          }
        }
      }
    }
  }
  TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
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
      //      Serial.println(F("Frame not recognised"));
      goto badframe;
    }
    //    Serial.println(F("good frame recieved"));
    return frame_type;    //only return non false value if frame header reasonable and frame type is defined
  }

  //do this when a bad frame if recieved
badframe:
  //  Serial.println(F("bad frame recieved"));
  return 0;
}

byte Coms_Serial::error_check_encoded_header(byte *temp_buffer) {

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

  //Serial.println("check for errors");
  for ( byte i = 0; i < HEADER_LENGTH; i++) {

    switch (i) {
      case OBJ_NUM_LOC:
        if (parity_of(APPLY_THREE_BIT_CHECKSUM_SUPPRESSION(temp_buffer[i]))) {
          return 0; //found error in obj_num byte
        }
        break;

      case NUM_OF_FRAMES_LOC:
        if (parity_of(APPLY_FRAME_NUM_PARITY_CHECKING_MASK(temp_buffer[i]))) {
          return 0;
        }
        if (parity_of(APPLY_THIS_FRAME_PARITY_CHECKING_MASK(temp_buffer[i]))) {
          return 0;
        }
        break;

      default:
        if (parity_of(temp_buffer[i])) {
          return 0; //error found
        }
        break;
    }
  }
  return (frame_type);

#endif


  return temp_buffer[FRAME_TYPE_LOC]; //case of no parity encoding, can read directly, return frame type, sanity check later
}



void Coms_Serial::request_frame_retransmission(byte frame_type, byte this_frame, byte obj_num) {

  // we know what frame we need send specific details in frame
  byte request_frame_length = MEGA_RX_FRAME_LENGTH + 4;
  byte nack_frame[request_frame_length];  //num data bytes and return carraige for reading

  nack_frame[0] = 250;
  nack_frame[1] = 251;

  nack_frame[2] = frame_type;
  nack_frame[3] = this_frame;
  nack_frame[4] = obj_num;

  nack_frame[5] = nack_frame[2] + nack_frame[3] + nack_frame[4];

  nack_frame[2] |= (0x10);//encode byte numbers
  nack_frame[3] |= (0x20);
  nack_frame[4] |= (0x30);
  nack_frame[5] |= (0x40);

  nack_frame[6] = 252;
  nack_frame[7] = 253;
  // serial.write stalls processor until sent, serial.print does not
  // so just add to buffer and move on

  //  for (byte i = 0; i < request_frame_length; i++) {
  //    Serial.print(nack_frame[i]);
  //    Serial.print(" ");
  //  }
  //  Serial.println();

  Serial_1.write(nack_frame, request_frame_length);
}

void Coms_Serial::request_frame_retransmission() {

  //  dont know anything about frame we need
  byte request_frame_length = MEGA_RX_FRAME_LENGTH + 4;
  byte nack_frame[request_frame_length];

  nack_frame[0] = 250;
  nack_frame[1] = 251;
  nack_frame[2] = UNKNOWN_RETRANSMIT_TYPE;
  nack_frame[3] = 0;  //irrelevant
  nack_frame[4] = 0;

  nack_frame[5] = UNKNOWN_RETRANSMIT_TYPE;

  nack_frame[2] |= (0x10);//encode byte numbers
  nack_frame[3] |= (0x20);
  nack_frame[4] |= (0x30);
  nack_frame[5] |= (0x40);

  nack_frame[6] = 252;
  nack_frame[7] = 253;

  //  for (byte i = 0; i < request_frame_length; i++) {
  //    Serial_1.print(nack_frame[i]);
  //  }

  Serial_1.write(nack_frame, request_frame_length);




}

#endif // COMS_SERIAL_CPP
