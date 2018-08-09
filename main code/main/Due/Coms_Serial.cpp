#ifndef Coms_serial_CPP
#define Coms_serial_CPP

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


//arrays for current display strings
extern char text_str[MAX_NUM_OF_TEXT_OBJECTS][MAX_TWEET_SIZE];

//menu object
extern Menu menu;

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

const char return_carraige[2] = {'\r', '\n'};



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

#ifdef SERIAL_1_IS_SOFT
  Serial_1.begin(
    RX_PIN_1,
    TX_PIN_1,
    SOFT_COMS_SPEED,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::TWO_STOP_BITS
  );
  NVIC_SetPriority (SERIAL_1_TIMER, SOFT_SERIAL_PRIORITY);  //set priority of interrupt, see priority definitions for details and links
#else
  Serial_1.begin(COMS_SPEED, SERIAL_8E2);
#endif


#ifdef SERIAL_2_IS_SOFT
  Serial_2.begin(
    RX_PIN_2,
    TX_PIN_2,
    SOFT_COMS_SPEED,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::TWO_STOP_BITS
  );

  NVIC_SetPriority (SERIAL_2_TIMER, SOFT_SERIAL_PRIORITY);
#else
  Serial_2.begin(COMS_SPEED, SERIAL_8E2);
#endif


#ifdef SERIAL_3_IS_SOFT
  Serial_3.begin(
    RX_PIN_3,
    TX_PIN_3,
    SOFT_COMS_SPEED,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::TWO_STOP_BITS
  );

  NVIC_SetPriority (SERIAL_3_TIMER, SOFT_SERIAL_PRIORITY);
#else
  Serial_3.begin(COMS_SPEED, SERIAL_8E2);
#endif


#ifdef SERIAL_4_IS_SOFT
  Serial_4.begin(
    RX_PIN_4,
    TX_PIN_4,
    SOFT_COMS_SPEED,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::TWO_STOP_BITS
  );

  NVIC_SetPriority (SERIAL_4_TIMER, SOFT_SERIAL_PRIORITY);
#else
  Serial_4.begin(COMS_SPEED, SERIAL_8E2);
#endif


#ifdef MEGA_SERIAL_CONNECTION_TESTING   // simple code to allow used to see start of transmission
  Serial_1.flush();
  Serial_2.flush();
  Serial_3.flush();
  Serial_4.flush();

  Serial_1.println("Hello 1");
  Serial_2.println("Hello 2");
  Serial_3.println("Hello 3");
  Serial_4.println("Hello 4");
#endif
}

void Coms_Serial::ping() {

  delay(100);
  //  Serial_1.println(ping_string);
  //  Serial_2.println(ping_string);
  //  Serial_3.println(ping_string);
  //Serial_4.println(ping_string);

  uint32_t ping_time = millis();

  if (mega_enabled[0]) {
    write_frame(0, PING_STRING_TYPE);
    //Serial_1.write(ping_string, sizeof(ping_string));
    //Serial_1.write(return_carraige, 2);
  }
  if (mega_enabled[1]) {
    write_frame(1, PING_STRING_TYPE);
    //Serial_2.write(ping_string, sizeof(ping_string));
    //Serial_2.write(return_carraige, 2);
  }
  if (mega_enabled[2]) {
    write_frame(2, PING_STRING_TYPE);
    //Serial_3.write(ping_string, sizeof(ping_string));
    //Serial_3.write(return_carraige, 2);
  }
  if (mega_enabled[3]) {
    write_frame(3, PING_STRING_TYPE);
    //Serial_4.write(ping_string, sizeof(ping_string));
    //Serial_4.write(return_carraige, 2);
  }

  //wait for response
  while (millis() < ping_time + PING_WAIT_PERIOD) {
    char ping_rx = '\0';
    if (mega_enabled[0] && Serial_1.available() != 0) {
      ping_rx = Serial_1.read();
      Serial.println(ping_rx);
      if (ping_rx == expected_ping_rx)
        mega_parameters.detected1 = true;
      else
        mega_parameters.detected1 = false;
    }

    if (mega_enabled[1] && Serial_2.available() != 0) {
      ping_rx = Serial_2.read();
      if (ping_rx == expected_ping_rx)
        mega_parameters.detected2 = true;
      else
        mega_parameters.detected2 = false;
    }

    if (mega_enabled[2] && Serial_3.available() != 0) {
      ping_rx = Serial_3.read();
      if (ping_rx == expected_ping_rx)
        mega_parameters.detected3 = true;
      else
        mega_parameters.detected3 = false;
    }

    if (mega_enabled[3] && Serial_4.available() != 0) {
      ping_rx = Serial_4.read();
      if (ping_rx == expected_ping_rx)
        mega_parameters.detected4 = true;
      else
        mega_parameters.detected4 = false;
    }
    if ((mega_enabled[0] ? mega_parameters.detected1 : true) && (mega_enabled[1] ? mega_parameters.detected2 : true) && (mega_enabled[2] ? mega_parameters.detected3 : true) && (mega_enabled[3] ? mega_parameters.detected4 : true)) {
      Serial.println(F("all megas detected"));
      break;
    }
  }
  if (millis() >= ping_time + PING_WAIT_PERIOD)
    Serial.println("timeout");
}

void Coms_Serial::send_text_frame(byte obj_num, int8_t address) {   //function to send strings to display on screen

  // function calculates the number of frames required to send the string, then loops,
  // generates a frame header and fills up to 25 bytes of the string and calculates the checksum
  // it also calls the write_text_frame function to send it on to the specified address when frame populated

  byte space_available_in_frame = FRAME_DATA_LENGTH - 1; // going to transmit obj num and data in same frame, so one byte less for string

  text_frame.num_frames = 1 + (strlen(text_str[obj_num]) / space_available_in_frame); //send this many frames
  text_frame.this_frame = 1;

  do {    //loop to send multiple frames if string is long

    if (text_frame.num_frames != text_frame.this_frame)
      text_frame.frame_length = MEGA_SERIAL_BUFFER_LENGTH;  //if there are more than one frame left to send, this frame is max size

    else
      text_frame.frame_length = strlen(text_str[obj_num]) - ((text_frame.num_frames - 1) * (space_available_in_frame)) + (FRAME_OVERHEAD); //remaining frame is (string length - text offset)+ (6 bytes overhead)

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
    //delayMicroseconds(1000);       //small delay, want reciever to read through its buffer, otherwise the buffer may overload when we send next frame
  } while (text_frame.this_frame <= text_frame.num_frames);
}


void Coms_Serial::send_partial_text_frame(byte address, byte obj_num, byte frame_num) {

  byte space_available_in_frame = FRAME_DATA_LENGTH - 1; // going to transmit obj num and data in same frame, so one byte less for string

  text_frame.num_frames = 1 + (strlen(text_str[obj_num]) / space_available_in_frame); //send this many frames
  text_frame.this_frame = frame_num;

  if (text_frame.num_frames != text_frame.this_frame)
    text_frame.frame_length = MEGA_SERIAL_BUFFER_LENGTH;  //if there are more than one frame left to send, this frame is max size

  else
    text_frame.frame_length = strlen(text_str[obj_num]) - ((text_frame.num_frames - 1) * (space_available_in_frame)) + (FRAME_OVERHEAD) + 1; //remaining frame is (string length - text offset)+ (6 bytes overhead) +1 for obj_num

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

void Coms_Serial::write_sensor_data_frame(byte address) {

  if (mega_status(address))       write_frame(address, SENSOR_FRAME_TYPE);
  else if (mega_status(address))  write_frame(address, SENSOR_FRAME_TYPE);
  else if (mega_status(address))  write_frame(address, SENSOR_FRAME_TYPE);
  else if (mega_status(address))  write_frame(address, SENSOR_FRAME_TYPE);
}

void Coms_Serial::write_menu_frame(byte address) {   //function to actually send the frame to given address

  if (mega_status(address))       write_frame(address, MENU_FRAME_TYPE);
  else if (mega_status(address))  write_frame(address, MENU_FRAME_TYPE);
  else if (mega_status(address))  write_frame(address, MENU_FRAME_TYPE);
  else if (mega_status(address))  write_frame(address, MENU_FRAME_TYPE);
}

void Coms_Serial::write_pos_frame(byte address) {

  if (mega_status(address))       write_frame(address, POS_FRAME_TYPE);
  else if (mega_status(address))  write_frame(address, POS_FRAME_TYPE);
  else if (mega_status(address))  write_frame(address, POS_FRAME_TYPE);
  else if (mega_status(address))  write_frame(address, POS_FRAME_TYPE);
}

void Coms_Serial::write_text_frame(byte address) {

  if (mega_status(address))       write_frame(address, TEXT_FRAME_TYPE);
  else if (mega_status(address))  write_frame(address, TEXT_FRAME_TYPE);
  else if (mega_status(address))  write_frame(address, TEXT_FRAME_TYPE);
  else if (mega_status(address))  write_frame(address, TEXT_FRAME_TYPE);
}

void Coms_Serial::write_frame(byte address, byte frame_type) {

  switch (address) {
    case 0:
#ifdef SERIAL_1_IS_SOFT
      disable_timer_interrupts(); //stop all processes that could cause issues
#endif
      Serial_1.write(return_carraige, 2); //used for start of frame detection

      switch (frame_type) {
        // NB: use serial write not print as it forces the processor to stop until sent, avoids chance of mega serial buffer overflow
        // maybe implement acknoledge system in future
        case TEXT_FRAME_TYPE:     Serial_1.write(text_frame.frame_buffer, text_frame.frame_length);                 break;
        case POS_FRAME_TYPE:      Serial_1.write(pos_frame.frame_buffer, pos_frame.frame_length);                   break;
        case MENU_FRAME_TYPE:     Serial_1.write(menu_frame.frame_buffer, menu_frame.frame_length);                 break;
        case SENSOR_FRAME_TYPE:   Serial_1.write(sensor_data_frame.frame_buffer, sensor_data_frame.frame_length);   break;
        case PING_STRING_TYPE:    Serial_1.write(ping_string, sizeof(ping_string));                                 break;
      }
      Serial_1.write(return_carraige, 2); //used for end of frame detection

#ifdef SERIAL_1_IS_SOFT
      enable_timer_interrupts(); //stop all processes that could cause issues
#endif

      break;
    case 1:
#ifdef SERIAL_2_IS_SOFT
      disable_timer_interrupts(); //stop all processes that could cause issues
#endif
      Serial_1.write(return_carraige, 2);
      switch (frame_type) {
        case TEXT_FRAME_TYPE:     Serial_2.write(text_frame.frame_buffer, text_frame.frame_length);                 break;
        case POS_FRAME_TYPE:      Serial_2.write(pos_frame.frame_buffer, pos_frame.frame_length);                   break;
        case MENU_FRAME_TYPE:     Serial_2.write(menu_frame.frame_buffer, menu_frame.frame_length);                 break;
        case SENSOR_FRAME_TYPE:   Serial_2.write(sensor_data_frame.frame_buffer, sensor_data_frame.frame_length);   break;
        case PING_STRING_TYPE:    Serial_2.write(ping_string, sizeof(ping_string));                                 break;
      }
      Serial_2.write(return_carraige, 2);

#ifdef SERIAL_2_IS_SOFT
      enable_timer_interrupts(); //stop all processes that could cause issues
#endif

      break;
    case 2:
#ifdef SERIAL_3_IS_SOFT
      disable_timer_interrupts(); //stop all processes that could cause issues
#endif
      Serial_1.write(return_carraige, 2);
      switch (frame_type) {
        case TEXT_FRAME_TYPE:     Serial_3.write(text_frame.frame_buffer, text_frame.frame_length);                 break;
        case POS_FRAME_TYPE:      Serial_3.write(pos_frame.frame_buffer, pos_frame.frame_length);                   while(1){}break; 
        case MENU_FRAME_TYPE:     Serial_3.write(menu_frame.frame_buffer, menu_frame.frame_length);                 break;
        case SENSOR_FRAME_TYPE:   Serial_3.write(sensor_data_frame.frame_buffer, sensor_data_frame.frame_length);   break;
        case PING_STRING_TYPE:    Serial_3.write(ping_string, sizeof(ping_string));                                 break;
      }
      Serial_3.write(return_carraige, 2);

#ifdef SERIAL_3_IS_SOFT
      enable_timer_interrupts(); //stop all processes that could cause issues
#endif

      break;
    case 3:
#ifdef SERIAL_4_IS_SOFT
      disable_timer_interrupts(); //stop all processes that could cause issues
#endif
      Serial_1.write(return_carraige, 2);
      switch (frame_type) {
        case TEXT_FRAME_TYPE:     Serial_4.write(text_frame.frame_buffer, text_frame.frame_length);                 break;
        case POS_FRAME_TYPE:      Serial_4.write(pos_frame.frame_buffer, pos_frame.frame_length);                   break;
        case MENU_FRAME_TYPE:     Serial_4.write(menu_frame.frame_buffer, menu_frame.frame_length);                 break;
        case SENSOR_FRAME_TYPE:   Serial_4.write(sensor_data_frame.frame_buffer, sensor_data_frame.frame_length);   break;
        case PING_STRING_TYPE:    Serial_4.write(ping_string, sizeof(ping_string));                                 break;
      }
      Serial_4.write(return_carraige, 2);

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

//
//void Coms_Serial::send_all_calibration_data(byte address) {     //function to send all data
//
//  //function to send all the sensor data. loop through all sensor values
//
//  byte frameNum = 1;
//  byte numFrames = ((sizeof(to_mega_prefix_array) * 2) / 26) + 1;
//  int offset = 0;
//  bool frame_to_be_sent = false;
//
//  sensor_data_frame.frame_buffer[1] = sensor_data_frame.frame_type;        //set frame starting bytes
//  sensor_data_frame.frame_buffer[2] = numFrames;
//  sensor_data_frame.frame_buffer[3] = frameNum;
//
//  for (int alpha = 0; alpha < sizeof(to_mega_prefix_array) + 1; alpha++) {
//
//    if (alpha == sizeof(to_mega_prefix_array)) { //if last byte
//      frame_to_be_sent = send_specific_calibration_data(to_mega_prefix_array[alpha],  address, false,  offset);   //indicate this is the last element
//
//    }
//    else
//      frame_to_be_sent = send_specific_calibration_data(to_mega_prefix_array[alpha],  address, true, offset);    //pack byte and dont send
//
//    if (!frame_to_be_sent)  //if the frame was sent (function returns 1), reset offset otherwise increment
//      offset++;
//
//    else if (frame_to_be_sent) {
//      frameNum++;     //increment the frame number
//      offset = 0;     //reset offset for new frame
//
//      write_sensor_data_frame(1);
//      write_sensor_data_frame(2);
//      write_sensor_data_frame(3);
//      write_sensor_data_frame(4);
//
//      sensor_data_frame.frame_buffer[1] = sensor_data_frame.frame_type;        //set frame starting bytes
//      sensor_data_frame.frame_buffer[2] = numFrames;
//      sensor_data_frame.frame_buffer[3] = frameNum;
//    }
//  }
//}

void Coms_Serial::send_specific_calibration_data(byte sensor_prefix, int address, bool more_bytes, uint16_t offset) { //sensor to send specific value

  // function to pack a frame with specific sensor data. the bool more_bytes can be used if htis is called as part of a loop to send more than one value
  // in the case that more_bytes is true it will hold off sending the frame until it is called and is false. offset is the number of sensor readings previously
  // written, so the place to write the new data is 4+2*offset for the prefix and 5+2*offset for the data

  int HEADER_PLUS_ONE = HEADER_LENGTH + 1;

  while ((FRAME_OVERHEAD + (offset * 2)) > MEGA_SERIAL_BUFFER_LENGTH) { //if trying to place it in location outside available space
    offset -= ((MEGA_SERIAL_BUFFER_LENGTH - FRAME_OVERHEAD) >> 2);     //subtract off full frames and fill from from lowest available index
  }

  //switch statement to pack the frame;
  switch (sensor_prefix) {
    case PREFIX_CURRENT_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = current_meter_parameters.reading2;
      break;

    case PREFIX_CURRENT_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = current_meter_parameters.reading2;
      break;

    case PREFIX_TEMP_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = temp_parameters.temp1;
      break;

    case PREFIX_TEMP_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = temp_parameters.temp2;
      break;

    case PREFIX_TEMP_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = temp_parameters.temp3;
      break;

    case PREFIX_LDR_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = light_sensor_parameters.reading1;
      break;

    case PREFIX_LDR_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = light_sensor_parameters.reading2;
      break;

    case PREFIX_FAN_SPEED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = fan_parameters.target_speed;
      break;

    case PREFIX_FAN_MINIMUM_SPEED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = fan_parameters.fan_minimum;
      break;

    case PREFIX_FAN_ENABLED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = fan_parameters.enabled;
      break;


    case PREFIX_LED_STRIP_BRIGHTNESS:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = led_strip_parameters.target_brightness;
      break;

    case PREFIX_LED_STRIP_ENABLED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = led_strip_parameters.enabled;
      break;



    case PREFIX_SD1_DETECTED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = card1.detected ? (byte) 1 : (byte) 0;   //convert boolean to byte
      break;

    case PREFIX_SD2_DETECTED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = card2.detected ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_EHTERNET_CONNECTED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = ethernet_connected ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_WIFI_CONNECTED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = wifi_connected ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_SCREEN_BRIGHTNESS:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      if (screen_brightness > 100) sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = screen_brightness = 100;
      else sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = screen_brightness;
      break;

    case PREFIX_TEXT_SIZE_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[0].text_size;
      break;

    case PREFIX_TEXT_SIZE_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[1].text_size;
      break;

    case PREFIX_TEXT_SIZE_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[2].text_size;
      break;

    case PREFIX_TEXT_SIZE_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[3].text_size;
      break;

    case PREFIX_TEXT_SIZE_4:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[4].text_size;
      break;

    case PREFIX_TEXT_COLOUR_R_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[0].red;
      break;

    case PREFIX_TEXT_COLOUR_R_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[1].red;
      break;

    case PREFIX_TEXT_COLOUR_R_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[2].red;
      break;

    case PREFIX_TEXT_COLOUR_R_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[3].red;
      break;

    case PREFIX_TEXT_COLOUR_R_4:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[4].red;
      break;

    case PREFIX_TEXT_COLOUR_G_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[0].green;
      break;

    case PREFIX_TEXT_COLOUR_G_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[1].green;
      break;

    case PREFIX_TEXT_COLOUR_G_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[2].green;
      break;

    case PREFIX_TEXT_COLOUR_G_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[3].green;
      break;

    case PREFIX_TEXT_COLOUR_G_4:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[4].green;
      break;

    case PREFIX_TEXT_COLOUR_B_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[0].blue;
      break;

    case PREFIX_TEXT_COLOUR_B_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[1].blue;
      break;

    case PREFIX_TEXT_COLOUR_B_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[2].blue;
      break;

    case PREFIX_TEXT_COLOUR_B_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[3].blue;
      break;

    case PREFIX_TEXT_COLOUR_B_4:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[4].blue;
      break;

    case PREFIX_TEXT_HUE_MSB_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = (byte)get_text_colour_hue(1, 0);   //function to geT the MS byte or LS byte, 1 returns MSB, 2 returns LSB
      break;

    case PREFIX_TEXT_HUE_LSB_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = (byte)get_text_colour_hue(2, 0);
      break;

    case PREFIX_TEXT_HUE_MSB_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = (byte)get_text_colour_hue(1, 1);   //function to geT the MS byte or LS byte, 1 returns MSB, 2 returns LSB
      break;

    case PREFIX_TEXT_HUE_LSB_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = (byte)get_text_colour_hue(2, 1);
      break;

    case PREFIX_TEXT_HUE_MSB_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = (byte)get_text_colour_hue(1, 2);   //function to geT the MS byte or LS byte, 1 returns MSB, 2 returns LSB
      break;

    case PREFIX_TEXT_HUE_LSB_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = (byte)get_text_colour_hue(2, 2);
      break;

    case PREFIX_TEXT_HUE_MSB_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = (byte)get_text_colour_hue(1, 3);   //function to geT the MS byte or LS byte, 1 returns MSB, 2 returns LSB
      break;

    case PREFIX_TEXT_HUE_LSB_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = (byte)get_text_colour_hue(2, 3);
      break;

    case PREFIX_TEXT_HUE_MSB_4:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = (byte)get_text_colour_hue(1, 4);   //function to geT the MS byte or LS byte, 1 returns MSB, 2 returns LSB
      break;

    case PREFIX_TEXT_HUE_LSB_4:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = (byte)get_text_colour_hue(2, 4);
      break;

    case PREFIX_TEXT_USE_HUE_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[0].use_hue ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_TEXT_USE_HUE_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[1].use_hue ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_TEXT_USE_HUE_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[2].use_hue ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_TEXT_USE_HUE_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[3].use_hue ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_TEXT_USE_HUE_4:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_parameters[4].use_hue ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_DEBUG_STATE:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
#ifdef DEBUG
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = (byte) 1;
#else
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = (byte) 0;
#endif
      break;

    case PREFIX_SCREEN_MODE:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = screen_mode;
      break;

    case PREFIX_SD_MOUNTED_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = card1.enabled;
      break;

    case PREFIX_SD_MOUNTED_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = card2.enabled;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_X_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_cursor[0].x_pos_dir;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_Y_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_cursor[0].y_pos_dir;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_X_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_cursor[1].x_pos_dir;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_Y_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_cursor[1].y_pos_dir;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_X_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_cursor[2].x_pos_dir;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_Y_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_cursor[2].y_pos_dir;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_X_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_cursor[3].x_pos_dir;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_Y_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_cursor[3].y_pos_dir;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_X_4:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_cursor[4].x_pos_dir;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_Y_4:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = text_cursor[4].y_pos_dir;
      break;

    case PREFIX_TEXT_OBJ_SELECTED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + (2 * offset)] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + (2 * offset)] = menu.get_selected_object();
      break;

    default:  Sprint("Error: Prefix not defined. Prefix :");
      Sprintln(sensor_prefix);

  }


  if (!more_bytes || ((FRAME_OVERHEAD + (offset * 2)) > MEGA_SERIAL_BUFFER_LENGTH - 2)) // if more than 30 bytes in frame accounted for, or no byte left to pack then send frame
  { // otherwise well be able to fit at least one more group of data in frame
    sensor_data_frame.frame_length = FRAME_OVERHEAD + (offset * 2) + 2; //header+content+new+data+trailer
    sensor_data_frame.frame_buffer[0] = sensor_data_frame.frame_length;

    set_frame_parity_and_checksum(SENSOR_FRAME_TYPE, sensor_data_frame.frame_length);
    sensor_data_frame.frame_buffer[sensor_data_frame.frame_length - 1] = ENDBYTE_CHARACTER;

    write_sensor_data_frame(address);
  }
  else {} //ehh, do somthing to accommodate frame full condition

}


void Coms_Serial::send_text_calibration_data(byte obj_num) {

  for (byte i = 0; i < NUM_SCREENS; i++) {

    send_specific_calibration_data(PREFIX_TEXT_SIZE_0 + obj_num, i, true, 0);
    send_specific_calibration_data(PREFIX_TEXT_COLOUR_R_0 + obj_num, i, true, 1);
    send_specific_calibration_data(PREFIX_TEXT_COLOUR_G_0 + obj_num, i, true, 2);
    send_specific_calibration_data(PREFIX_TEXT_COLOUR_B_0 + obj_num, i, true, 3);
    if (text_parameters[obj_num].use_hue) {
      send_specific_calibration_data(PREFIX_TEXT_USE_HUE_0 + obj_num, i, true, 4);
      send_specific_calibration_data(PREFIX_TEXT_HUE_MSB_0 + obj_num, i, true, 5);
      send_specific_calibration_data(PREFIX_TEXT_HUE_LSB_0 + obj_num, i, false, 6);
    }
    else
      send_specific_calibration_data(PREFIX_TEXT_USE_HUE_0 + obj_num, i, false, 4);
  }
}

void Coms_Serial::check_megas() {

  if (Serial_1.available() > 0 && mega_parameters.detected1) {
    String rx = Serial_1.readString(); //read until '\0' recieved
    decode_serial_rx(rx, 0);
  }

  if (Serial_2.available() > 0 && mega_parameters.detected2) {
    String rx = Serial_2.readString();
    decode_serial_rx(rx, 1);
  }

  if (Serial_3.available() > 0 && mega_parameters.detected3) {
    String rx = Serial_3.readString();
    decode_serial_rx(rx, 2);
  }

  if (Serial_4.available() > 0 && mega_parameters.detected4) {
    String rx = Serial_4.readString();
    decode_serial_rx(rx, 3);
  }
}

void Coms_Serial::decode_serial_rx(String rx, byte address) {

  if (rx.length() > MEGA_RX_FRAME_LENGTH + 1) // if more than expected content + null char
    rx = rx.substring(rx.length() - MEGA_RX_FRAME_LENGTH);  //get substring

  char char_array[MEGA_RX_FRAME_LENGTH] = {'\0'};
  strcpy(char_array, rx.c_str());

  //verify checksum
  byte check_sum = 0;
  for (byte i = 0; i < MEGA_RX_FRAME_LENGTH - 1; i++) {
    check_sum = char_array[i];
  }

  if (!(check_sum == char_array[MEGA_RX_FRAME_LENGTH - 1])) //probably not a request, mega will request again after timout if it was corrupted
    return;
  else {
    byte frame_num = char_array[0];
    byte obj_num = char_array[1];

    if (error_sanity_check(frame_num, obj_num)) { //case where mega knows what data is missing, send exact data required
      send_partial_text_frame(address, obj_num, frame_num);
    }
    else { // resend all objects to this mega, hard to be sure what went wrong
      for (byte i = 0; i < MAX_NUM_OF_TEXT_OBJECTS; i++) {
        if (text_cursor[obj_num].object_used) {
          send_text_frame(i, address);
        }
      }
    }
  }
}


#endif //USE_SERIAL_TO_MEGAS
#endif //Sign_coms_serial_CPP
