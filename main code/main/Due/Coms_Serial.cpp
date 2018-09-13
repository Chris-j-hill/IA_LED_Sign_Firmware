#ifndef Coms_serial_CPP
#define Coms_serial_CPP


#include "Due_Libraries.h"
#include "Coms_Serial.h"
#include "Coms.h"
#include "Due_Pins.h"
#include "Graphics.h"
#include "Current_Control.h"
#include "Fans.h"
#include "Internet.h"
#include "SD_Cards.h"
#include "Led_Strip.h"
#include "Encoder.h"
#include "Menu_Tree.h"
#include "Host.h"


//give access to these structs
extern struct Led_Strip_Struct led_strip_parameters;
extern struct Temp_sensor temp_parameters;
extern struct Fan_Struct fan_parameters;        //create fan struct
extern struct Timers timers;
extern struct Encoder_Struct encoder_parameters;     //create encoder struct
extern struct Button_Struct button_parameters;       //create button struct
extern struct LDR_Struct light_sensor_parameters;
extern struct Current_Meter_Struct current_meter_parameters;
extern struct SD_Card card1;
extern struct SD_Card card2;
extern struct Text text_parameters[MAX_NUM_OF_TEXT_OBJECTS];
extern struct Text_cursor text_cursor[MAX_NUM_OF_TEXT_OBJECTS];
extern struct Frame text_frame;
extern struct Frame sensor_data_frame;
extern struct Frame menu_frame;
extern struct Frame pos_frame;
extern struct Frame ping_frame;


extern struct Frame_History text_frame_history[NUM_SCREENS];
extern struct Frame_History menu_frame_history[NUM_SCREENS];
extern struct Frame_History sensor_data_frame_history[NUM_SCREENS];
extern struct Frame_History pos_frame_history[NUM_SCREENS];


//arrays for current display strings
extern char text_str[MAX_NUM_OF_TEXT_OBJECTS][MAX_TWEET_SIZE];

//menu object
extern Menu menu;
extern Host host;
// used when sending all calibration data, is this ever done?
//const byte to_mega_prefix_array[] = {10, 11, 20, 21, 22, 30, 31, 40, 50, 60, 61, 70, 80, 90,
//                                     100, 101, 102, 103, 104,
//                                     110, 111, 112, 113, 114,
//                                     120, 121, 122, 123, 124,
//                                     130, 131, 132, 133, 134,
//                                     140, 141, 142, 143, 144,
//                                     150, 151, 152, 153, 154,
//                                     160, 161, 162, 163, 164,
//                                     170, 180, 190, 191,
//                                     200, 201, 202, 203, 204,
//                                     210, 211, 212, 213, 214,
//                                     220, 221, 222, 223, 224,
//                                     230};


extern bool mega_enabled[4];


#ifdef USE_SERIAL_TO_MEGAS
bool enable_serial = true;
#else
bool enable_serial = false;
#endif

bool serial_enabled = false;


#if RX_PIN_1 == 15              //configure Serial_1 as hardware or software serial depending on defined pin
#define Serial_1 Serial3
#elif RX_PIN_1 == 17
#define Serial_1 Serial2
#elif RX_PIN_1 == 19
#define Serial_1 Serial1
#else
serial_tc5_declaration(RX_BUF_LENGTH, TX_BUF_LENGTH);
auto& Serial_1 = serial_tc5;
#define SERIAL_1_TIMER  TC5_IRQn    //see soft_uart.cpp
#define SERIAL_1_IS_SOFT
#endif

#if RX_PIN_2 == 15
#define Serial_2 Serial3
#elif RX_PIN_2 == 17
#define Serial_2 Serial2
#elif RX_PIN_2 == 19
#define Serial_2 Serial1
#else
serial_tc6_declaration(RX_BUF_LENGTH, TX_BUF_LENGTH);
auto& Serial_2 = serial_tc6;
#define SERIAL_2_TIMER  TC6_IRQn
#define SERIAL_2_IS_SOFT
#endif

#if RX_PIN_3 == 15
#define Serial_3 Serial3
#elif RX_PIN_3 == 17
#define Serial_3 Serial2
#elif RX_PIN_3 == 19
#define Serial_3 Serial1
#else
serial_tc7_declaration(RX_BUF_LENGTH, TX_BUF_LENGTH);
auto& Serial_3 = serial_tc7;
#define SERIAL_3_TIMER  TC7_IRQn
#define SERIAL_3_IS_SOFT
#endif

#if RX_PIN_4 == 15
#define Serial_4 Serial3
#elif RX_PIN_4 == 17
#define Serial_4 Serial2
#elif RX_PIN_4 == 19
#define Serial_4 Serial1
#else
serial_tc8_declaration(RX_BUF_LENGTH, TX_BUF_LENGTH);
auto& Serial_4 = serial_tc8;
#define SERIAL_4_TIMER  TC8_IRQn
#define SERIAL_4_IS_SOFT
#endif

//const char return_carraige[2] = {'\r', '\n'};
const char frame_start_bytes[2] = {251, 252};
const char frame_end_bytes[2] = {253, 254};


Mega_Serial_Parameters mega_parameters;

extern byte screen_brightness;
extern byte screen_mode;

#ifdef USE_SERIAL_TO_MEGAS

void Coms_Serial::init_serial() {

  init_software_serial_to_megas();
  Serial.println("Setup done");

#ifndef MEGA_SERIAL_CONNECTION_TESTING  //ignore ping if testing with serial passthrough code on mega
  ping();
#else
  mega_parameters.detected1 = true;   // temporarily do this for testing
  mega_parameters.detected2 = true;
  mega_parameters.detected3 = true;
  mega_parameters.detected4 = true;
#endif
  init_frames(); //build constant parts of frames
}


void Coms_Serial::init_software_serial_to_megas() {   // initialise serial at specified speed, must be standardised speed 115200 or below, otherwise error thrown


  configure_serial_1(COMS_SPEED); //configure at defualt coms speed initially
  configure_serial_2(COMS_SPEED);
  configure_serial_3(COMS_SPEED);
  configure_serial_4(COMS_SPEED);


  // clear bufers and set timeouts

  Serial_1.flush();
  Serial_2.flush();
  Serial_3.flush();
  Serial_4.flush();

  while (Serial_1.available() > 0)
    Serial_1.read();
  while (Serial_2.available() > 0)
    Serial_2.read();
  while (Serial_3.available() > 0)
    Serial_3.read();
  while (Serial_4.available() > 0)
    Serial_4.read();

  int timeout = ceil((10000 << 2) / COMS_SPEED); //wait for up to four bytes periods before timeout
  Serial_1.setTimeout(timeout);
  Serial_2.setTimeout(timeout);
  Serial_3.setTimeout(timeout);
  Serial_4.setTimeout(timeout);
  //
  //  Serial_1.println("Hello 1");
  //  Serial_2.println("Hello 2");
  //  Serial_3.println("Hello 3");
  //  Serial_4.println("Hello 4");

}


void Coms_Serial::configure_serial_1(uint32_t baud) {

#ifdef SERIAL_1_IS_SOFT
  Serial_1.begin(
    RX_PIN_1,
    TX_PIN_1,
    baud,
    soft_uart::data_bit_codes::SOFT_COMS_CONFIG_NUM_BITS,
    soft_uart::parity_codes::SOFT_COMS_CONFIG_NUM_PARITY,
    soft_uart::stop_bit_codes::SOFT_COMS_CONFIG_NUM_STOP
  );
  NVIC_SetPriority (SERIAL_1_TIMER, SOFT_SERIAL_PRIORITY);  //set priority of interrupt, see priority definitions for details and links
#else
  Serial_1.begin(baud, HARD_COMS_CONFIG);
#endif

}
void Coms_Serial::configure_serial_2(uint32_t baud) {

#ifdef SERIAL_2_IS_SOFT
  Serial_2.begin(
    RX_PIN_2,
    TX_PIN_2,
    baud,
    soft_uart::data_bit_codes::SOFT_COMS_CONFIG_NUM_BITS,
    soft_uart::parity_codes::SOFT_COMS_CONFIG_NUM_PARITY,
    soft_uart::stop_bit_codes::SOFT_COMS_CONFIG_NUM_STOP
  );

  NVIC_SetPriority (SERIAL_2_TIMER, SOFT_SERIAL_PRIORITY);
#else
  Serial_2.begin(baud, HARD_COMS_CONFIG);
#endif

}
void Coms_Serial::configure_serial_3(uint32_t baud) {

#ifdef SERIAL_3_IS_SOFT
  Serial_3.begin(
    RX_PIN_3,
    TX_PIN_3,
    baud,
    soft_uart::data_bit_codes::SOFT_COMS_CONFIG_NUM_BITS,
    soft_uart::parity_codes::SOFT_COMS_CONFIG_NUM_PARITY,
    soft_uart::stop_bit_codes::SOFT_COMS_CONFIG_NUM_STOP
  );

  NVIC_SetPriority (SERIAL_3_TIMER, SOFT_SERIAL_PRIORITY);
#else
  Serial_3.begin(baud, HARD_COMS_CONFIG);
#endif

}
void Coms_Serial::configure_serial_4(uint32_t baud) {

#ifdef SERIAL_4_IS_SOFT
  Serial_4.begin(
    RX_PIN_4,
    TX_PIN_4,
    baud,
    soft_uart::data_bit_codes::SOFT_COMS_CONFIG_NUM_BITS,
    soft_uart::parity_codes::SOFT_COMS_CONFIG_NUM_PARITY,
    soft_uart::stop_bit_codes::SOFT_COMS_CONFIG_NUM_STOP
  );

  NVIC_SetPriority (SERIAL_4_TIMER, SOFT_SERIAL_PRIORITY);
#else
  Serial_4.begin(buad, HARD_COMS_CONFIG);
#endif

}

void Coms_Serial::ping() {

  delay(100);

  uint32_t ping_time = millis();

  bool ping1_1_ok = false;  //flag for first ping response
  bool ping2_1_ok = false;
  bool ping3_1_ok = false;
  bool ping4_1_ok = false;

  bool ping1_2_ok = false;  // flag for second ping response
  bool ping2_2_ok = false;
  bool ping3_2_ok = false;
  bool ping4_2_ok = false;

  bool ping1_failed = false;  //transmission failed, stop checking for frame arrival
  bool ping2_failed = false;
  bool ping3_failed = false;
  bool ping4_failed = false;

  uint32_t cur_baud_1 = COMS_SPEED;
  uint32_t cur_baud_2 = COMS_SPEED;
  uint32_t cur_baud_3 = COMS_SPEED;
  uint32_t cur_baud_4 = COMS_SPEED;

  if (mega_enabled[0]) {    // initiate ping sequence
    write_frame(0, PING_STRING_TYPE);
  }
  if (mega_enabled[1]) {
    write_frame(1, PING_STRING_TYPE);
  }
  if (mega_enabled[2]) {
    write_frame(2, PING_STRING_TYPE);
  }
  if (mega_enabled[3]) {
    write_frame(3, PING_STRING_TYPE);
  }


  //wait for response
  while (millis() < ping_time + PING_WAIT_PERIOD) {

    byte ping_rx[PING_FRAME_RESPONSE_LENGTH] = {'\0'};


    //#########################
    //    proceedure for mega 1
    //#########################

    if (mega_enabled[0] && !ping1_1_ok && !ping1_failed && Serial_1.available() >= PING_FRAME_RESPONSE_LENGTH) { //wait until entire frame arrived, if byte dropped already whill timeout, assume bad connection...
      Serial_1.readBytes(ping_rx, PING_FRAME_RESPONSE_LENGTH);

      if (sanity_check_ping_rx(ping_rx)) { // return 1 if no errors, genuine response
        if (ping_rx[4] == PING_GOOD_RESPONSE) { // ok to switch to new speed
          ping1_1_ok = true;

#ifdef SERIAL_1_IS_SOFT
          configure_serial_1(SOFT_COMS_SPEED);
          cur_baud_1 = SOFT_COMS_SPEED;
#else
          configure_serial_1(HARD_COMS_SPEED);
          cur_baud_1 = HARD_COMS_SPEED;
#endif
        }
        else {   // error in transmitting original frame, if cant transmit at default speed set mega as not detected
          ping1_failed = true;
        }
      }
      else {
        ping1_failed = true;
        mega_parameters.detected1 = false;
      }
    }
    if (ping1_1_ok  && !ping1_failed && Serial_1.available() >= PING_FRAME_RESPONSE_LENGTH) { //wait until second frame received at new speed
      Serial_1.readBytes(ping_rx, PING_FRAME_RESPONSE_LENGTH);
      if (sanity_check_ping_rx(ping_rx)) { // return 1 if no errors, genuine response

        if (ping_rx[4] == PING_GOOD_RESPONSE) { // ok to switch to new speed
          ping1_2_ok = true;
          ping1_1_ok = false;
        }
        else {   //why might this happen?
          ping1_failed = true;
        }
      }
      else {
        ping1_failed = true;
        mega_parameters.detected1 = false;
      }
    }
    if (ping1_2_ok && !ping1_failed) {
      //print chars until new frame arrived

      if (Serial_1.available() < PING_FRAME_RESPONSE_LENGTH) {
        Serial_1.write("U", 1); //U == 01010101 good char for detecting shorted pulse length at mega end
        delay(1); //delay to detect edge between chars
      }
      else {
        Serial_1.readBytes(ping_rx, PING_FRAME_RESPONSE_LENGTH);
        if (sanity_check_ping_rx(ping_rx)) { // return 1 if no errors, genuine response

          if (ping_rx[4] == PING_GOOD_RESPONSE) { // ok to switch to new speed
            ping1_2_ok = false;
            ping1_1_ok = false;
          }
          else {
            //bad frame, mismatch between expected speed and measured speed by mega, return to default
            configure_serial_1(COMS_SPEED);
            cur_baud_1 = COMS_SPEED;
          }

          mega_parameters.detected1 = true; //in either case mega has been detected
        }
        else {
          ping1_failed = true;
          mega_parameters.detected1 = false;
        }
      }
    }



    //#########################
    //    proceedure for mega 2
    //#########################


    if (mega_enabled[1] && !ping2_1_ok && !ping1_failed && Serial_2.available() >= PING_FRAME_RESPONSE_LENGTH) { //wait until entire frame arrived, if byte dropped already whill timeout, assume bad connection...
      Serial_2.readBytes(ping_rx, PING_FRAME_RESPONSE_LENGTH);

      if (sanity_check_ping_rx(ping_rx)) { // return 1 if no errors, genuine response
        if (ping_rx[4] == PING_GOOD_RESPONSE) { // ok to switch to new speed
          ping2_1_ok = true;

#ifdef SERIAL_2_IS_SOFT
          configure_serial_2(SOFT_COMS_SPEED);
          cur_baud_2 = SOFT_COMS_SPEED;
#else
          configure_serial_2(HARD_COMS_SPEED);
          cur_baud_2 = HARD_COMS_SPEED;
#endif
        }
        else {   // error in transmitting original frame, if cant transmit at default speed set mega as not detected
          ping2_failed = true;
        }
      }
      else {
        ping2_failed = true;
        mega_parameters.detected2 = false;
      }
    }
    if (ping2_1_ok  && !ping2_failed && Serial_2.available() >= PING_FRAME_RESPONSE_LENGTH) { //wait until second frame received at new speed
      Serial_2.readBytes(ping_rx, PING_FRAME_RESPONSE_LENGTH);
      if (sanity_check_ping_rx(ping_rx)) { // return 1 if no errors, genuine response

        if (ping_rx[4] == PING_GOOD_RESPONSE) { // ok to switch to new speed
          ping2_2_ok = true;
          ping2_1_ok = false;
        }
        else {   //why might this happen?
          ping2_failed = true;
        }
      }
      else {
        ping2_failed = true;
        mega_parameters.detected2 = false;
      }
    }
    if (ping2_2_ok && !ping2_failed) {
      //print chars until new frame arrived

      if (Serial_2.available() < PING_FRAME_RESPONSE_LENGTH) {
        Serial_2.write("U", 1); //U == 01010101 good char for detecting shorted pulse length at mega end
        delay(1); //delay to detect edge between chars
      }
      else {
        Serial_2.readBytes(ping_rx, PING_FRAME_RESPONSE_LENGTH);
        if (sanity_check_ping_rx(ping_rx)) { // return 1 if no errors, genuine response

          if (ping_rx[4] == PING_GOOD_RESPONSE) { // ok to switch to new speed
            ping2_2_ok = false;
            ping2_1_ok = false;
          }
          else {
            //ping bad frame, mismatch between expected speed and measured speed by mega, return to default
            configure_serial_2(COMS_SPEED);
            cur_baud_2 = COMS_SPEED;
          }
          mega_parameters.detected2 = true; //in either case mega has been detected
        }
        else {
          ping2_failed = true;
          mega_parameters.detected2 = false;
        }
      }
    }




    //#########################
    //    proceedure for mega 3
    //#########################


    if (mega_enabled[2] && !ping3_1_ok && !ping3_failed && Serial_3.available() >= PING_FRAME_RESPONSE_LENGTH) { //wait until entire frame arrived, if byte dropped already whill timeout, assume bad connection...
      Serial_1.readBytes(ping_rx, PING_FRAME_RESPONSE_LENGTH);

      if (sanity_check_ping_rx(ping_rx)) { // return 1 if no errors, genuine response
        if (ping_rx[4] == PING_GOOD_RESPONSE) { // ok to switch to new speed
          ping3_1_ok = true;

#ifdef SERIAL_3_IS_SOFT
          configure_serial_3(SOFT_COMS_SPEED);
          cur_baud_3 = SOFT_COMS_SPEED;
#else
          configure_serial_3(HARD_COMS_SPEED);
          cur_baud_3 = HARD_COMS_SPEED;
#endif
        }
        else {   // error in transmitting original frame, if cant transmit at default speed set mega as not detected
          ping3_failed = true;
        }
      }
      else {
        ping3_failed = true;
        mega_parameters.detected3 = false;
      }
    }
    if (ping3_1_ok  && !ping3_failed && Serial_3.available() >= PING_FRAME_RESPONSE_LENGTH) { //wait until second frame received at new speed
      Serial_1.readBytes(ping_rx, PING_FRAME_RESPONSE_LENGTH);
      if (sanity_check_ping_rx(ping_rx)) { // return 1 if no errors, genuine response

        if (ping_rx[4] == PING_GOOD_RESPONSE) { // ok to switch to new speed
          ping3_2_ok = true;
          ping3_1_ok = false;
        }
        else {   //why might this happen?
          ping3_failed = true;
        }
      }
      else {
        ping3_failed = true;
        mega_parameters.detected3 = false;
      }
    }
    if (ping3_2_ok && !ping3_failed) {
      //print chars until new frame arrived

      if (Serial_3.available() < PING_FRAME_RESPONSE_LENGTH) {
        Serial_3.write("U", 1); //U == 01010101 good char for detecting shorted pulse length at mega end
        delay(1); //delay to detect edge between chars
      }
      else {
        Serial_3.readBytes(ping_rx, PING_FRAME_RESPONSE_LENGTH);
        if (sanity_check_ping_rx(ping_rx)) { // return 1 if no errors, genuine response

          if (ping_rx[4] == PING_GOOD_RESPONSE) { // ok to switch to new speed
            ping3_2_ok = false;
            ping3_1_ok = false;
          }
          else {
            //bad frame, mismatch between expected speed and measured speed by mega, return to default
            configure_serial_3(COMS_SPEED);
            cur_baud_3 = COMS_SPEED;
          }

          mega_parameters.detected3 = true; //in either case mega has been detected
        }
        else {
          ping3_failed = true;
          mega_parameters.detected3 = false;
        }
      }
    }




    //#########################
    //    proceedure for mega 4
    //#########################


    if (mega_enabled[3] && !ping4_1_ok && !ping4_failed && Serial_4.available() >= PING_FRAME_RESPONSE_LENGTH) { //wait until entire frame arrived, if byte dropped already whill timeout, assume bad connection...
      Serial_4.readBytes(ping_rx, PING_FRAME_RESPONSE_LENGTH);

      if (sanity_check_ping_rx(ping_rx)) { // return 1 if no errors, genuine response
        if (ping_rx[4] == PING_GOOD_RESPONSE) { // ok to switch to new speed
          ping4_1_ok = true;

#ifdef SERIAL_4_IS_SOFT
          configure_serial_4(SOFT_COMS_SPEED);
          cur_baud_4 = SOFT_COMS_SPEED;
#else
          configure_serial_4(HARD_COMS_SPEED);
          cur_baud_4 = HARD_COMS_SPEED;
#endif
        }
        else {   // error in transmitting original frame, if cant transmit at default speed set mega as not detected
          ping4_failed = true;
        }
      }
      else {
        ping4_failed = true;
        mega_parameters.detected4 = false;
      }
    }
    if (ping4_1_ok  && !ping4_failed && Serial_4.available() >= PING_FRAME_RESPONSE_LENGTH) { //wait until second frame received at new speed
      Serial_4.readBytes(ping_rx, PING_FRAME_RESPONSE_LENGTH);
      if (sanity_check_ping_rx(ping_rx)) { // return 1 if no errors, genuine response

        if (ping_rx[4] == PING_GOOD_RESPONSE) { // ok to switch to new speed
          ping4_2_ok = true;
          ping4_1_ok = false;
        }
        else {   //why might this happen?
          ping4_failed = true;
        }
      }
      else {
        ping4_failed = true;
        mega_parameters.detected4 = false;
      }
    }
    if (ping4_2_ok && !ping4_failed) {
      //print chars until new frame arrived

      if (Serial_4.available() < PING_FRAME_RESPONSE_LENGTH) {
        Serial_4.write("U", 1); //U == 01010101 good char for detecting shorted pulse length at mega end
        delay(1); //delay to detect edge between chars
      }
      else {
        Serial_4.readBytes(ping_rx, PING_FRAME_RESPONSE_LENGTH);
        if (sanity_check_ping_rx(ping_rx)) { // return 1 if no errors, genuine response

          if (ping_rx[4] == PING_GOOD_RESPONSE) { // ok to switch to new speed
            ping4_2_ok = false;
            ping4_1_ok = false;
          }
          else {
            //bad frame, mismatch between expected speed and measured speed by mega, return to default
            configure_serial_4(COMS_SPEED);
            cur_baud_4 = COMS_SPEED;
          }

          mega_parameters.detected4 = true; //in either case mega has been detected
        }
        else {
          ping4_failed = true;
          mega_parameters.detected4 = false;
        }
      }
    }

    //once all megas detected, break
    if (mega_parameters.detected1 && mega_parameters.detected2 && mega_parameters.detected3 && mega_parameters.detected4) {
      break;
    }


  }

  if (millis() >= ping_time + PING_WAIT_PERIOD) {
    Serial.println("ping timeout");

    if (cur_baud_1 != COMS_SPEED && !mega_parameters.detected1) //if timeout and this mega not detected but we changed com speed at some point, set it back to default
      configure_serial_1(COMS_SPEED);

    if (cur_baud_2 != COMS_SPEED && !mega_parameters.detected2)
      configure_serial_2(COMS_SPEED);

    if (cur_baud_3 != COMS_SPEED && !mega_parameters.detected3)
      configure_serial_3(COMS_SPEED);

    if (cur_baud_4 != COMS_SPEED && !mega_parameters.detected4)
      configure_serial_4(COMS_SPEED);

  }
}




void Coms_Serial::send_text_frame(byte obj_num, int8_t address) {   //function to send strings to display on screen

  // function calculates the number of frames required to send the string, then loops,
  // generates a frame header and fills up to 25 bytes of the string and calculates the checksum
  // it also calls the write_text_frame function to send it on to the specified address when frame populated


  text_frame.num_frames = 1 + ((text_parameters[obj_num].text_str_length - 1) / FRAME_DATA_LENGTH); //send this many frames
  text_frame.this_frame = 1;

  do {    //loop to send multiple frames if string is long

    if (text_frame.num_frames != text_frame.this_frame)
      text_frame.frame_length = MEGA_SERIAL_BUFFER_LENGTH;  //if there are more than one frame left to send, this frame is max size

    else
      text_frame.frame_length = text_parameters[obj_num].text_str_length - ((text_frame.num_frames - 1) * (FRAME_DATA_LENGTH)) + (FRAME_OVERHEAD); //remaining frame is (string length - text offset)+ (6 bytes overhead)

    text_frame.frame_buffer[0] = text_frame.frame_length;
    text_frame.frame_buffer[1] = text_frame.frame_type;
    text_frame.frame_buffer[2] = PACK_FRAME_NUM_DATA(text_frame.num_frames, text_frame.this_frame);
    text_frame.frame_buffer[3] = PACK_OBJ_NUM_DATA(obj_num);

    pack_disp_string_frame(text_frame.this_frame, obj_num);//function to pack the frame with which ever data is relevant

    if (address == -1)
      write_text_frame();  //send to all megas
    else
      write_text_frame(address);  //only send specific one mega

    text_frame.this_frame++;   //increment this_frame after sending, will prepare for next loop or break

    delay(MULTI_FRAME_DELAY_PERIOD);       //small delay, want reciever to read through its buffer, otherwise the buffer may overload when we send next frame
  } while (text_frame.this_frame <= text_frame.num_frames);
}


void Coms_Serial::send_partial_text_frame(byte address, byte obj_num, byte frame_num) {


  text_frame.num_frames = 1 + ((text_parameters[obj_num].text_str_length - 1) / FRAME_DATA_LENGTH); //send this many frames
  text_frame.this_frame = frame_num;

  if (text_frame.num_frames != text_frame.this_frame)
    text_frame.frame_length = MEGA_SERIAL_BUFFER_LENGTH;  //if there are more than one frame left to send, this frame is max size

  else
    text_frame.frame_length = text_parameters[obj_num].text_str_length - ((text_frame.num_frames - 1) * (FRAME_DATA_LENGTH)) + (FRAME_OVERHEAD) + 1; //remaining frame is (string length - text offset)+ (6 bytes overhead) +1 for obj_num

  text_frame.frame_buffer[0] = text_frame.frame_length;
  text_frame.frame_buffer[1] = text_frame.frame_type;
  text_frame.frame_buffer[2] = PACK_FRAME_NUM_DATA(text_frame.num_frames, text_frame.this_frame);
  text_frame.frame_buffer[3] = PACK_OBJ_NUM_DATA(obj_num);

  pack_disp_string_frame(text_frame.this_frame, obj_num);//function to pack the frame with which ever data is relevant
  write_text_frame(address);  //only send specific one mega

}

void Coms_Serial::send_menu_frame(byte cur_menu) { // build frame and call write_menu_frame for relevant addresses

  build_menu_data_frame(cur_menu);
  byte menu_width = menu.get_menu_width();

  if (menu_width >= 0 || menu.is_all_system_menu(cur_menu)) {  //not sure why it would be less than zero width but include for completeness
    write_menu_frame(3);  //write frame to address 3

  }

  if (menu_width > 64 || menu.is_all_system_menu(cur_menu)) {
    write_menu_frame(2);
  }

  if (menu_width > 128 || menu.is_all_system_menu(cur_menu)) {
    write_menu_frame(1);
  }

  if (menu_width > 192 || menu.is_all_system_menu(cur_menu)) {
    write_menu_frame(0);
  }
}

void Coms_Serial::send_pos_frame(byte obj_num) {  //build frame and send position to all megas

  build_pos_frame(obj_num);

  for (byte i = 0; i < NUM_SCREENS ; i++) {
    write_pos_frame(i);
  }
}

void Coms_Serial::write_sensor_data_frame(byte address) {   //function to actually send the frame to given address

  if (mega_status(address)) {                                                          //if enabled and detected
    write_frame(address, SENSOR_FRAME_TYPE);                                           //send frame
    append_frame_history(sensor_data_frame.frame_buffer, address, SENSOR_FRAME_TYPE);  //save the frame to history buffer
  }
}

void Coms_Serial::write_menu_frame(byte address) {

  if (mega_status(address)) {
    write_frame(address, MENU_FRAME_TYPE);
    append_frame_history(menu_frame.frame_buffer, address, MENU_FRAME_TYPE);
  }
}

void Coms_Serial::write_pos_frame(byte address) {

  if (mega_status(address)) {
    write_frame(address, POS_FRAME_TYPE);
    append_frame_history(pos_frame.frame_buffer, address, POS_FRAME_TYPE);
  }
}

void Coms_Serial::write_text_frame(byte address) {

  if (mega_status(address)) {
    write_frame(address, TEXT_FRAME_TYPE);
    append_frame_history(text_frame.frame_buffer, address, TEXT_FRAME_TYPE);
  }
}

void Coms_Serial::write_frame(byte address, byte frame_type, byte * buf, byte frame_length) {

  switch (address) {

    case 0:
#ifdef SERIAL_1_IS_SOFT
      disable_timer_interrupts(); //stop all processes that could cause issues
#endif

      Serial_1.print(frame_start_bytes[0]); //used for start of frame detection, print now write as non blocking
      Serial_1.print(frame_start_bytes[1]);
      switch (frame_type) {
        // NB: use serial write not print as it forces the processor to stop until sent, avoids chance of mega serial buffer overflow
        // maybe implement acknoledge system in future
        case TEXT_FRAME_TYPE:     Serial_1.write(text_frame.frame_buffer, text_frame.frame_length);                 break;
        case POS_FRAME_TYPE:      Serial_1.write(pos_frame.frame_buffer, pos_frame.frame_length);                   break;
        case MENU_FRAME_TYPE:     Serial_1.write(menu_frame.frame_buffer, menu_frame.frame_length);                 break;
        case SENSOR_FRAME_TYPE:   Serial_1.write(sensor_data_frame.frame_buffer, sensor_data_frame.frame_length);   break;
        case PING_STRING_TYPE:    Serial_1.write(ping_frame.frame_buffer, ping_frame.frame_length);                                 break;
        case FRAME_RETRANSMIT:    Serial_1.write(buf, frame_length);                                                break;
      }
      Serial_1.write(frame_end_bytes, 2); //used for end of frame detection

#ifdef SERIAL_1_IS_SOFT
      enable_timer_interrupts(); //stop all processes that could cause issues
#endif

      break;


    case 1:
#ifdef SERIAL_2_IS_SOFT
      disable_timer_interrupts(); //stop all processes that could cause issues
#endif
      Serial_2.print(frame_start_bytes[0]);
      Serial_2.print(frame_start_bytes[1]);
      switch (frame_type) {
        case TEXT_FRAME_TYPE:     Serial_2.write(text_frame.frame_buffer, text_frame.frame_length);                 break;
        case POS_FRAME_TYPE:      Serial_2.write(pos_frame.frame_buffer, pos_frame.frame_length);                   break;
        case MENU_FRAME_TYPE:     Serial_2.write(menu_frame.frame_buffer, menu_frame.frame_length);                 break;
        case SENSOR_FRAME_TYPE:   Serial_2.write(sensor_data_frame.frame_buffer, sensor_data_frame.frame_length);   break;
        case PING_STRING_TYPE:    Serial_2.write(ping_frame.frame_buffer, ping_frame.frame_length);                                 break;
        case FRAME_RETRANSMIT:    Serial_2.write(buf, frame_length);                                                break;
      }
      Serial_2.write(frame_end_bytes, 2);

#ifdef SERIAL_2_IS_SOFT
      enable_timer_interrupts(); //stop all processes that could cause issues
#endif
      break;

    case 2:
#ifdef SERIAL_3_IS_SOFT
      disable_timer_interrupts(); //stop all processes that could cause issues
#endif
      Serial_3.print(frame_start_bytes[0]);
      Serial_3.print(frame_start_bytes[1]);
      switch (frame_type) {
        case TEXT_FRAME_TYPE:     Serial_3.write(text_frame.frame_buffer, text_frame.frame_length);                 break;
        case POS_FRAME_TYPE:      Serial_3.write(pos_frame.frame_buffer, pos_frame.frame_length);                   break;
        case MENU_FRAME_TYPE:     Serial_3.write(menu_frame.frame_buffer, menu_frame.frame_length);                 break;
        case SENSOR_FRAME_TYPE:   Serial_3.write(sensor_data_frame.frame_buffer, sensor_data_frame.frame_length);   break;
        case PING_STRING_TYPE:    Serial_3.write(ping_frame.frame_buffer, ping_frame.frame_length);                                 break;
        case FRAME_RETRANSMIT:    Serial_3.write(buf, frame_length);                                                break;
      }
      Serial_3.write(frame_end_bytes, 2);

#ifdef SERIAL_3_IS_SOFT
      enable_timer_interrupts(); //stop all processes that could cause issues
#endif

      break;

    case 3:
#ifdef SERIAL_4_IS_SOFT
      disable_timer_interrupts(); //stop all processes that could cause issues
#endif

      Serial_4.print(frame_start_bytes[0]);
      Serial_4.print(frame_start_bytes[1]);
      switch (frame_type) {
        case TEXT_FRAME_TYPE:     Serial_4.write(text_frame.frame_buffer, text_frame.frame_length);                 break;
        case POS_FRAME_TYPE:      Serial_4.write(pos_frame.frame_buffer, pos_frame.frame_length);                   break;
        case MENU_FRAME_TYPE:     Serial_4.write(menu_frame.frame_buffer, menu_frame.frame_length);                 break;
        case SENSOR_FRAME_TYPE:
          for (byte i = 0; i < sensor_data_frame.frame_length; i++) {
            Serial.print(sensor_data_frame.frame_buffer[i]);
            Serial.print(" ");
          }
          Serial.println();
          Serial_4.write(sensor_data_frame.frame_buffer, sensor_data_frame.frame_length);
          break;
        case PING_STRING_TYPE:    Serial_4.write(ping_frame.frame_buffer, ping_frame.frame_length);                 break;
        case FRAME_RETRANSMIT:    Serial_4.write(buf, frame_length);                                                break;
      }
      Serial_4.write(frame_end_bytes, 2);

#ifdef SERIAL_4_IS_SOFT
      enable_timer_interrupts(); //stop all processes that could cause issues
#endif
      break;
  }
}

void Coms_Serial::disable_timer_interrupts() {
  if (timers.led_strip_timer_attached) {
    LED_STRIP_TIMER.stop();
  }

  if (timers.fan_timer_attached) {
    FAN_TIMER.stop();
  }

  if (timers.pos_timer_attached) {
    POS_TIMER.stop();
  }
}

void Coms_Serial::enable_timer_interrupts() {
  if (timers.led_strip_timer_attached) {
    LED_STRIP_TIMER.start();
  }

  if (timers.fan_timer_attached) {
    FAN_TIMER.start();
  }

  if (timers.pos_timer_attached) {
    POS_TIMER.start();
  }
}


inline void Coms_Serial::write_text_frame() {  // send to all at once

  write_text_frame(0);
  delayMicroseconds(3000);
  write_text_frame(1);
  delayMicroseconds(3000);
  write_text_frame(2);
  delayMicroseconds(3000);
  write_text_frame(3);
  delayMicroseconds(3000);

}

inline bool Coms_Serial::mega_status(byte address) {
  switch (address) {
    case 0: return (mega_enabled[0] && mega_parameters.detected1); break;
    case 1: return (mega_enabled[1] && mega_parameters.detected2); break;
    case 2: return (mega_enabled[2] && mega_parameters.detected3); break;
    case 3: return (mega_enabled[3] && mega_parameters.detected4); break;
  }
}

void Coms_Serial::send_specific_calibration_data(byte sensor_prefix, int address, bool more_bytes, uint16_t offset) { //sensor to send specific value

  // function to pack a frame with specific sensor data. the bool more_bytes can be used if htis is called as part of a loop to send more than one value
  // in the case that more_bytes is true it will hold off sending the frame until it is called and is false. offset is the number of sensor readings previously
  // written, so the place to write the new data is 4+2*offset for the prefix and 5+2*offset for the data

  int HEADER_PLUS_ONE = HEADER_LENGTH + 1;

  while ((FRAME_OVERHEAD + (offset * 2)) > MEGA_SERIAL_BUFFER_LENGTH) { //if trying to place it in location outside available space
    offset -= ((MEGA_SERIAL_BUFFER_LENGTH - FRAME_OVERHEAD) >> 1);     //subtract off full frames and fill from from lowest available index
  }

  offset = offset << 1; //offset *2 becasue data in pairs
  //switch statement to pack the frame;
  switch (sensor_prefix) {
    case PREFIX_CURRENT_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = current_meter_parameters.reading2;
      break;

    case PREFIX_CURRENT_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = current_meter_parameters.reading2;
      break;

    case PREFIX_TEMP_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = temp_parameters.temp1;
      break;

    case PREFIX_TEMP_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = temp_parameters.temp2;
      break;

    case PREFIX_TEMP_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = temp_parameters.temp3;
      break;

    case PREFIX_LDR_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = light_sensor_parameters.reading1;
      break;

    case PREFIX_LDR_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = light_sensor_parameters.reading2;
      break;

    case PREFIX_FAN_SPEED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = fan_parameters.target_speed;
      break;

    case PREFIX_FAN_MINIMUM_SPEED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = fan_parameters.fan_minimum;
      break;

    case PREFIX_FAN_ENABLED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = fan_parameters.enabled;
      break;


    case PREFIX_LED_STRIP_BRIGHTNESS:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = led_strip_parameters.target_brightness;
      break;

    case PREFIX_LED_STRIP_ENABLED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = led_strip_parameters.enabled;
      break;


    case PREFIX_SD1_DETECTED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = card1.detected ? (byte) 1 : (byte) 0;   //convert boolean to byte
      break;

    case PREFIX_SD2_DETECTED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = card2.detected ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_EHTERNET_CONNECTED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = ethernet_connected ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_WIFI_CONNECTED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = wifi_connected ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_SCREEN_BRIGHTNESS:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      if (screen_brightness > 100) sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = screen_brightness = 100;
      else sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = screen_brightness;
      break;

    case PREFIX_TEXT_SIZE_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[0].text_size;
      break;

    case PREFIX_TEXT_SIZE_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[1].text_size;
      break;

    case PREFIX_TEXT_SIZE_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[2].text_size;
      break;

    case PREFIX_TEXT_SIZE_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[3].text_size;
      break;

    case PREFIX_TEXT_SIZE_4:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[4].text_size;
      break;

    case PREFIX_TEXT_COLOUR_R_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[0].red;
      break;

    case PREFIX_TEXT_COLOUR_R_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[1].red;
      break;

    case PREFIX_TEXT_COLOUR_R_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[2].red;
      break;

    case PREFIX_TEXT_COLOUR_R_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[3].red;
      break;

    case PREFIX_TEXT_COLOUR_R_4:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[4].red;
      break;

    case PREFIX_TEXT_COLOUR_G_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[0].green;
      break;

    case PREFIX_TEXT_COLOUR_G_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[1].green;
      break;

    case PREFIX_TEXT_COLOUR_G_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[2].green;
      break;

    case PREFIX_TEXT_COLOUR_G_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[3].green;
      break;

    case PREFIX_TEXT_COLOUR_G_4:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[4].green;
      break;

    case PREFIX_TEXT_COLOUR_B_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[0].blue;
      break;

    case PREFIX_TEXT_COLOUR_B_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[1].blue;
      break;

    case PREFIX_TEXT_COLOUR_B_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[2].blue;
      break;

    case PREFIX_TEXT_COLOUR_B_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[3].blue;
      break;

    case PREFIX_TEXT_COLOUR_B_4:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[4].blue;
      break;

    case PREFIX_TEXT_HUE_MSB_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = (byte)get_text_colour_hue(1, 0);   //function to geT the MS byte or LS byte, 1 returns MSB, 2 returns LSB
      break;

    case PREFIX_TEXT_HUE_LSB_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = (byte)get_text_colour_hue(2, 0);
      break;

    case PREFIX_TEXT_HUE_MSB_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = (byte)get_text_colour_hue(1, 1);   //function to geT the MS byte or LS byte, 1 returns MSB, 2 returns LSB
      break;

    case PREFIX_TEXT_HUE_LSB_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = (byte)get_text_colour_hue(2, 1);
      break;

    case PREFIX_TEXT_HUE_MSB_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = (byte)get_text_colour_hue(1, 2);   //function to geT the MS byte or LS byte, 1 returns MSB, 2 returns LSB
      break;

    case PREFIX_TEXT_HUE_LSB_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = (byte)get_text_colour_hue(2, 2);
      break;

    case PREFIX_TEXT_HUE_MSB_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = (byte)get_text_colour_hue(1, 3);   //function to geT the MS byte or LS byte, 1 returns MSB, 2 returns LSB
      break;

    case PREFIX_TEXT_HUE_LSB_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = (byte)get_text_colour_hue(2, 3);
      break;

    case PREFIX_TEXT_HUE_MSB_4:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = (byte)get_text_colour_hue(1, 4);   //function to geT the MS byte or LS byte, 1 returns MSB, 2 returns LSB
      break;

    case PREFIX_TEXT_HUE_LSB_4:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = (byte)get_text_colour_hue(2, 4);
      break;

    case PREFIX_TEXT_USE_HUE_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[0].use_hue ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_TEXT_USE_HUE_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[1].use_hue ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_TEXT_USE_HUE_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[2].use_hue ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_TEXT_USE_HUE_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[3].use_hue ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_TEXT_USE_HUE_4:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_parameters[4].use_hue ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_DEBUG_STATE:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
#ifdef DEBUG
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = (byte) 1;
#else
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = (byte) 0;
#endif
      break;

    case PREFIX_SCREEN_MODE:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = screen_mode;
      break;

    case PREFIX_SD_MOUNTED_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = card1.enabled;
      break;

    case PREFIX_SD_MOUNTED_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = card2.enabled;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_X_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_cursor[0].x_pos_dir;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_Y_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_cursor[0].y_pos_dir;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_X_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_cursor[1].x_pos_dir;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_Y_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_cursor[1].y_pos_dir;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_X_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_cursor[2].x_pos_dir;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_Y_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_cursor[2].y_pos_dir;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_X_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_cursor[3].x_pos_dir;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_Y_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_cursor[3].y_pos_dir;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_X_4:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_cursor[4].x_pos_dir;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_Y_4:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_cursor[4].y_pos_dir;
      break;

    case PREFIX_TEXT_OBJ_ENABLED_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_cursor[0].object_used ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_TEXT_OBJ_ENABLED_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_cursor[1].object_used ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_TEXT_OBJ_ENABLED_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_cursor[2].object_used ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_TEXT_OBJ_ENABLED_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_cursor[3].object_used ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_TEXT_OBJ_ENABLED_4:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = text_cursor[4].object_used ? (byte) 1 : (byte) 0;
      break;


    case PREFIX_TEXT_OBJ_SELECTED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + offset] = menu.get_selected_object();
      break;

    default:  Sprint("Error: Prefix not defined. Prefix :");
      Sprintln(sensor_prefix);

  }


  if (!more_bytes || ((FRAME_OVERHEAD + offset) > MEGA_SERIAL_BUFFER_LENGTH - 2)) // if more than 30 bytes in frame accounted for, or no byte left to pack then send frame
  { // otherwise well be able to fit at least one more group of data in frame
    sensor_data_frame.frame_length = FRAME_OVERHEAD + offset + 2; //header+content+new+data+trailer
    sensor_data_frame.frame_buffer[0] = sensor_data_frame.frame_length;

    set_frame_parity_and_checksum(SENSOR_FRAME_TYPE, sensor_data_frame.frame_length);
    sensor_data_frame.frame_buffer[sensor_data_frame.frame_length - 1] = ENDBYTE_CHARACTER;

    write_sensor_data_frame(address);

    if (more_bytes) //if more bytes to send, delay to allow mega to catch up
      delay(MULTI_FRAME_DELAY_PERIOD);
  }
  else {} //ehh, do somthing to accommodate frame full condition

}


void Coms_Serial::send_text_calibration_data(byte obj_num) {

  for (byte i = 0; i < NUM_SCREENS; i++) {

    send_specific_calibration_data(PREFIX_TEXT_SIZE_0 + obj_num, i, true, 0);
    send_specific_calibration_data(PREFIX_TEXT_COLOUR_R_0 + obj_num, i, true, 1);
    send_specific_calibration_data(PREFIX_TEXT_COLOUR_G_0 + obj_num, i, true, 2);
    send_specific_calibration_data(PREFIX_TEXT_COLOUR_B_0 + obj_num, i, true, 3);
    send_specific_calibration_data(PREFIX_TEXT_OBJ_ENABLED_0 + obj_num, i, true, 4);

    //pack differently depending if hue is used
    if (text_parameters[obj_num].use_hue) {
      send_specific_calibration_data(PREFIX_TEXT_USE_HUE_0 + obj_num, i, true, 5);
      send_specific_calibration_data(PREFIX_TEXT_HUE_MSB_0 + obj_num, i, true, 6);
      send_specific_calibration_data(PREFIX_TEXT_HUE_LSB_0 + obj_num, i, false, 7);
    }
    else
      send_specific_calibration_data(PREFIX_TEXT_USE_HUE_0 + obj_num, i, false, 5);
  }
}

void Coms_Serial::send_text_disable(byte obj_num) {
  for (byte i = 0; i < NUM_SCREENS; i++)
    send_specific_calibration_data(PREFIX_TEXT_OBJ_ENABLED_0 + obj_num, i, false, 0);
}


void Coms_Serial::check_megas() {
  //  static bool first_run = true;
  byte rx[MEGA_RX_FRAME_LENGTH];

  //  if (first_run) {
  //    while (Serial_1.available() > 0)
  //      Serial_1.read();
  //    while (Serial_2.available() > 0)
  //      Serial_2.read();
  //    while (Serial_3.available() > 0)
  //      Serial_3.read();
  //    while (Serial_4.available() > 0)
  //      Serial_4.read();
  //    first_run = false;
  //  }

  check_mega_1();  //functions to actually check ports for request from mega
  check_mega_2();
  check_mega_3();
  check_mega_4();

}

void Coms_Serial::check_mega_1() {

  if (Serial_1.available() > 6  && mega_parameters.detected1) {

    bool seen_byte_1 = false;
    bool seen_byte_2 = false;
    byte break_condition = 0;
    byte rx[MEGA_RX_FRAME_LENGTH];

    if (Serial_1.peek() == 250) {
      Serial_1.read();
      seen_byte_1 = true;
    }

seen_byte_1:
    if (Serial_1.peek() == 251) {
      Serial_1.read();
      seen_byte_2 = true;
    }

seen_byte_2:
    byte i = 0;
    if (!seen_byte_1 && !seen_byte_2) { //byte here but no idea what it is

      if ((Serial_1.peek() >> 4) == 1) {  //if this is first data byte in frame, skip first iteration of while loop
        rx[0] = Serial_1.read() & 0xF;
        i = 1;
        seen_byte_2 = true;
      }

      else if ((Serial_1.peek() == 252 || Serial_1.peek() == 253))  //if this is an endbyte character
        //        Serial.println(Serial_1.read());
        Serial_1.read();
      else {
        //        Serial.print("lost :");
        //        Serial.println(Serial_1.read());
        Serial_1.read();
      }
    }

    if (seen_byte_2) { //in this funciton pass we have found both bytes 1 and 2

      byte temp = 0;
      seen_byte_1 = false;
      seen_byte_2 = false;
      break_condition = 0;
      uint32_t start_time = millis();
      while (i < MEGA_RX_FRAME_LENGTH && millis() < start_time + 50) {

        temp = Serial_1.read();

        if (temp == 250) { //just read a start byte when we shouldnt, stop reading, jump back to check for next byte
          break_condition = 1;
          seen_byte_1 = true;
          break;
        }
        else if (temp == 251) { //just read a start byte when we shouldnt, stop reading, jump back to check for next byte
          break_condition = 2;
          seen_byte_1 = true;
          seen_byte_2 = true;
          break;
        }

        else if (temp == 252) {
          break_condition = 3;
          break;
        }
        else if (temp == 253) {
          break_condition = 4;
          break;
        }
        rx[i] = temp & 0xF; //mask out byte number
        i++;
      }
      if (break_condition == 0 && millis() < start_time + 50) { //read frame without issue, decode
        decode_serial_rx(rx, 0);
      }
    }

    if (break_condition != 0) { //some error occured, jump to correct location, assume dropped byte
      switch (break_condition) {
        case 1: goto seen_byte_1; break;
        case 2: goto seen_byte_2; break;
      }
    }
  }
}
void Coms_Serial::check_mega_2() {

  if (Serial_2.available() > 6  && mega_parameters.detected2) {

    bool seen_byte_1 = false;
    bool seen_byte_2 = false;
    byte break_condition = 0;
    byte rx[MEGA_RX_FRAME_LENGTH];

    if (Serial_2.peek() == 250) {
      Serial_2.read();
      seen_byte_1 = true;
    }

seen_byte_1:
    if (Serial_2.peek() == 251) {
      Serial_2.read();
      seen_byte_2 = true;
    }

seen_byte_2:
    byte i = 0;
    if (!seen_byte_1 && !seen_byte_2) { //byte here but no idea what it is

      if ((Serial_2.peek() >> 4) == 1) {
        rx[0] = Serial_2.read() & 0xF;
        i = 1;
        seen_byte_2 = true;
      }

      else if ((Serial_2.peek() == 252 || Serial_2.peek() == 253))
        //        Serial.println(Serial_2.read());
        Serial_2.read();
      else {
        //        Serial.print("lost :");
        //        Serial.println(Serial_2.read());
        Serial_2.read();
      }
    }

    if (seen_byte_2) { //in this funciton pass we have found both bytes 1 and 2

      byte temp = 0;
      seen_byte_1 = false;
      seen_byte_2 = false;
      break_condition = 0;
      uint32_t start_time = millis();
      while (i < MEGA_RX_FRAME_LENGTH && millis() < start_time + 50) {

        temp = Serial_2.read();

        if (temp == 250) { //just read a start byte when we shouldnt, stop reading, jump back to check for next byte
          break_condition = 1;
          seen_byte_1 = true;
          break;
        }
        else if (temp == 251) { //just read a start byte when we shouldnt, stop reading, jump back to check for next byte
          break_condition = 2;
          seen_byte_1 = true;
          seen_byte_2 = true;
          break;
        }

        else if (temp == 252) {
          break_condition = 3;
          break;
        }
        else if (temp == 253) {
          break_condition = 4;
          break;
        }
        rx[i] = temp & 0xF; //mask out byte number
        i++;
      }
      if (break_condition == 0 && millis() < start_time + 50) { //read frame without issue, decode
        decode_serial_rx(rx, 1);
      }
    }

    if (break_condition != 0) { //some error occured, jump to correct location, assume dropped byte
      switch (break_condition) {
        case 1: goto seen_byte_1; break;
        case 2: goto seen_byte_2; break;
      }
    }
  }
}

void Coms_Serial::check_mega_3() {

  if (Serial_3.available() > 6  && mega_parameters.detected3) {

    bool seen_byte_1 = false;
    bool seen_byte_2 = false;
    byte break_condition = 0;
    byte rx[MEGA_RX_FRAME_LENGTH];


    if (Serial_3.peek() == 250) {
      Serial_3.read();
      seen_byte_1 = true;
    }

seen_byte_1:
    if (Serial_3.peek() == 251) {
      Serial_3.read();
      seen_byte_2 = true;
    }

seen_byte_2:
    byte i = 0;
    if (!seen_byte_1 && !seen_byte_2) { //byte here but no idea what it is

      if ((Serial_3.peek() >> 4) == 1) {
        rx[0] = Serial_3.read() & 0xF;
        i = 1;
        seen_byte_2 = true;
      }


      else if ((Serial_3.peek() == 252 || Serial_3.peek() == 253))
        //        Serial.println(Serial_3.read());
        Serial_3.read();
      else {
        //        Serial.print("lost :");
        //        Serial.println(Serial_3.read());
        Serial_3.read();
      }

    }

    if (seen_byte_2) { //in this funciton pass we have found both bytes 1 and 2

      byte temp = 0;
      seen_byte_1 = false;
      seen_byte_2 = false;
      break_condition = 0;
      uint32_t start_time = millis();
      while (i < MEGA_RX_FRAME_LENGTH && millis() < start_time + 50) {

        temp = Serial_3.read();

        if (temp == 250) { //just read a start byte when we shouldnt, stop reading, jump back to check for next byte
          break_condition = 1;
          seen_byte_1 = true;
          break;
        }
        else if (temp == 251) { //just read a start byte when we shouldnt, stop reading, jump back to check for next byte
          break_condition = 2;
          seen_byte_1 = true;
          seen_byte_2 = true;
          break;
        }

        else if (temp == 252) {
          break_condition = 3;
          break;
        }
        else if (temp == 253) {
          break_condition = 4;
          break;
        }
        rx[i] = temp & 0xF;
        i++;
      }
      if (break_condition == 0 && millis() < start_time + 50) { //read frame without issue, decode
        decode_serial_rx(rx, 2);
      }
    }

    if (break_condition != 0) { //some error occured, jump to correct location, assume dropped byte
      switch (break_condition) {
        case 1: goto seen_byte_1; break;
        case 2: goto seen_byte_2; break;
      }
    }
  }
}


void Coms_Serial::check_mega_4() {

  if (Serial_4.available() > 6  && mega_parameters.detected4) {

    bool seen_byte_1 = false;
    bool seen_byte_2 = false;
    byte break_condition = 0;
    byte rx[MEGA_RX_FRAME_LENGTH];

    if (Serial_4.peek() == 250) {
      Serial_4.read();
      seen_byte_1 = true;
    }

seen_byte_1:
    if (Serial_4.peek() == 251) {
      Serial_4.read();
      seen_byte_2 = true;
    }

seen_byte_2:
    byte i = 0;
    if (!seen_byte_1 && !seen_byte_2) { //byte here but no idea what it is

      if ((Serial_4.peek() >> 4) == 1) {
        rx[0] = Serial_4.read() & 0xF;
        i = 1;
        seen_byte_2 = true;
      }

      else if ((Serial_4.peek() == 252 || Serial_4.peek() == 253))
        //        Serial.println(Serial_4.read());
        Serial_4.read();
      else {
        //        Serial.print("lost :");
        //        Serial.println(Serial_4.read());
        Serial_4.read();
      }
    }

    if (seen_byte_2) { //in this funciton pass we have found both bytes 1 and 2

      byte temp = 0;
      seen_byte_1 = false;
      seen_byte_2 = false;
      break_condition = 0;
      uint32_t start_time = millis();
      while (i < MEGA_RX_FRAME_LENGTH && millis() < start_time + 50) {

        temp = Serial_4.read();

        if (temp == 250) { //just read a start byte when we shouldnt, stop reading, jump back to check for next byte
          break_condition = 1;
          seen_byte_1 = true;
          break;
        }
        else if (temp == 251) { //just read a start byte when we shouldnt, stop reading, jump back to check for next byte
          break_condition = 2;
          seen_byte_1 = true;
          seen_byte_2 = true;
          break;
        }

        else if (temp == 252) {
          break_condition = 3;
          break;
        }
        else if (temp == 253) {
          break_condition = 4;
          break;
        }
        rx[i] = temp & 0xF; //mask out byte number
        i++;
      }
      if (break_condition == 0 && millis() < start_time + 50) { //read frame without issue, decode
        decode_serial_rx(rx, 3);
      }
    }

    if (break_condition != 0) { //some error occured, jump to correct location, assume dropped byte
      switch (break_condition) {
        case 1: goto seen_byte_1; break;
        case 2: goto seen_byte_2; break;
      }
    }
  }
}


void Coms_Serial::decode_serial_rx(byte * char_array, byte address) {

  //calc checksum
  byte check_sum = 0;
  for (byte i = 0; i < MEGA_RX_FRAME_LENGTH - 1; i++) {
    check_sum += char_array[i];
  }

  if (check_sum != char_array[MEGA_RX_FRAME_LENGTH - 1]) { // might be corrupted request, retransmit last N frames?
    write_all_frame_history(address);
    Serial.println("checksum fail");
  }
  else if (char_array[0] == UNKNOWN_RETRANSMIT_TYPE) {     // case where mega knows header was corrupted
    write_all_frame_history(address);
    Serial.println("request unknown frame");
  }

  else {
    byte frame_type = char_array[0];
    byte frame_num = char_array[1];
    byte obj_num = char_array[2];

    if (request_error_sanity_check(frame_type, frame_num, obj_num)) { //case where mega knows what data is missing, and frame not corrupted, send exact data required

      byte history_loc = find_in_frame_history(address, frame_type, frame_num, obj_num);  //identify location

      if (history_loc != FRAME_HISTORY_MEMORY_DEPTH) {                                    //if frame in frame history buffer
        write_frame_history(address, frame_type, history_loc);                            //write the frame at this location
        Serial.print("send specific frame : ");
        Serial.println(frame_type);
      }
      else {
        write_all_frame_history(address);                     // assume something went wrong, frame forgotten or mega mistaken, send a bunch of frames and hope for the best
        Serial.println("specific frame doesnt exist");
      }
    }
    else {
      write_all_frame_history(address);                       //error in request, send a bunch of frames
      Serial.println("insane retransmit request");

    }
  }
}


void Coms_Serial::write_frame_history(byte address, byte frame_type, byte loc) { //write specific frame

  //  Serial.print("Address : ");
  //  Serial.println(address);
  //  Serial.print("loc : ");
  //  Serial.println(loc);
  //  Serial.print("frame type : ");
  //  Serial.println(frame_type);
  //
  //  for (byte i = 0; i < MEGA_SERIAL_BUFFER_LENGTH; i++) {
  //    Serial.print(text_frame_history[address].frame_content[loc][i]);
  //    Serial.print(" ");
  //  }
  //  Serial.println();
  //  Serial.print("frame length ");
  //  Serial.println(EXTRACT_FRAME_LENGTH(text_frame_history[address].frame_content[loc][0]));
  //
  switch (frame_type) {
    case TEXT_FRAME_TYPE:
      write_frame(address, FRAME_RETRANSMIT, text_frame_history[address].frame_content[loc], EXTRACT_FRAME_LENGTH(text_frame_history[address].frame_content[loc][0]));
      break;

    case POS_FRAME_TYPE:
      write_frame(address, FRAME_RETRANSMIT, pos_frame_history[address].frame_content[loc], EXTRACT_FRAME_LENGTH(pos_frame_history[address].frame_content[loc][0]));
      break;

    case SENSOR_FRAME_TYPE:
      write_frame(address, FRAME_RETRANSMIT, sensor_data_frame_history[address].frame_content[loc], EXTRACT_FRAME_LENGTH(sensor_data_frame_history[address].frame_content[loc][0]));
      break;

    case MENU_FRAME_TYPE:
      write_frame(address, FRAME_RETRANSMIT, menu_frame_history[address].frame_content[loc], EXTRACT_FRAME_LENGTH(menu_frame_history[address].frame_content[loc][0]));
      break;
  }
}


void Coms_Serial::write_all_frame_history(byte address) { //write a bunch of frames to this address, hopefully one is right


  byte start_loc = 0;
  byte num_sent_frames = 0;

  Serial.print("Num text frames : ");
  Serial.println(text_frame_history[address].num_populated_buffers);
  Serial.print("Num sensor frames : ");
  Serial.println(sensor_data_frame_history[address].num_populated_buffers);
  Serial.print("Num menu frames : ");
  Serial.println(menu_frame_history[address].num_populated_buffers);
  Serial.print("Num pos frames : ");
  Serial.println(pos_frame_history[address].num_populated_buffers);


  for (byte frame_type = 1; frame_type <= 4; frame_type++) { //loop through frame types

    switch (frame_type) {
      case TEXT_FRAME_TYPE:

        start_loc = text_frame_history[address].history_index;
        num_sent_frames = 0;

        for (int loc = start_loc; loc != start_loc + 1; loc--) {
          if (loc < 0)
            loc = FRAME_HISTORY_MEMORY_DEPTH - 1; //loop back index
          if (num_sent_frames == text_frame_history[address].num_populated_buffers)
            break;
          Serial.println("resend text frame");
          write_frame(address, FRAME_RETRANSMIT, text_frame_history[address].frame_content[loc], EXTRACT_FRAME_LENGTH(text_frame_history[address].frame_content[loc][0]));
          num_sent_frames++;

          if (num_sent_frames == MAX_NUM_TEXT_FRAME_RETRASMIT)
            break; //once max sent, finish inner loop
        }

        break;

      case POS_FRAME_TYPE:
        start_loc = pos_frame_history[address].history_index;
        num_sent_frames = 0;

        for (int loc = start_loc; loc != start_loc + 1; loc--) {
          if (loc < 0)
            loc = FRAME_HISTORY_MEMORY_DEPTH - 1; //loop back index

          if (num_sent_frames == pos_frame_history[address].num_populated_buffers)
            break;

          write_frame(address, FRAME_RETRANSMIT, pos_frame_history[address].frame_content[loc], EXTRACT_FRAME_LENGTH(pos_frame_history[address].frame_content[loc][0]));
          num_sent_frames++;

          if (num_sent_frames == MAX_NUM_POS_FRAME_RETRASMIT)
            break; //once max sent, finish inner loop
        }

        break;

      case SENSOR_FRAME_TYPE:
        start_loc = sensor_data_frame_history[address].history_index;
        num_sent_frames = 0;

        for (int loc = start_loc; loc != start_loc + 1; loc--) {

          if (loc < 0)
            loc = FRAME_HISTORY_MEMORY_DEPTH - 1; //loop back index

          if (num_sent_frames == sensor_data_frame_history[address].num_populated_buffers)
            break;

          write_frame(address, FRAME_RETRANSMIT, sensor_data_frame_history[address].frame_content[loc], EXTRACT_FRAME_LENGTH(sensor_data_frame_history[address].frame_content[loc][0]));
          num_sent_frames++;

          if (num_sent_frames == MAX_NUM_SENSOR_DATA_FRAME_RETRASMIT)
            break; //once max sent, finish inner loop
        }
        break;

      case MENU_FRAME_TYPE:
        start_loc = menu_frame_history[address].history_index;
        num_sent_frames = 0;

        for (int loc = start_loc; loc != start_loc + 1; loc--) {
          if (loc < 0)
            loc = FRAME_HISTORY_MEMORY_DEPTH - 1; //loop back index
          if (num_sent_frames == menu_frame_history[address].num_populated_buffers)
            break;

          write_frame(address, FRAME_RETRANSMIT, menu_frame_history[address].frame_content[loc], EXTRACT_FRAME_LENGTH(menu_frame_history[address].frame_content[loc][0]));
          num_sent_frames++;

          if (num_sent_frames == MAX_NUM_MENU_FRAME_RETRASMIT )
            break; //once max sent, finish inner loop
        }
        break;
    }
  }
}

#endif //USE_SERIAL_TO_MEGAS
#endif //Sign_coms_serial_CPP
