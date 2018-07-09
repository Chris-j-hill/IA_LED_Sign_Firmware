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

extern struct Frame text_frame;
extern struct Frame sensor_data_frame;
extern struct Frame menu_frame;
extern struct Frame pos_frame;



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

extern char text_str[MAX_NUM_OF_TEXT_OBJECTS][MAX_TWEET_SIZE];
const byte to_mega_prefix_array[] = {10, 11, 20, 21, 22, 30, 31, 40, 50, 60, 61, 70, 80, 90,
                                     100, 101, 102, 103, 104,
                                     110, 111, 112, 113, 114,
                                     120, 121, 122, 123, 124,
                                     130, 131, 132, 133, 134,
                                     140, 141, 142, 143, 144,
                                     150, 151, 152, 153, 154,
                                     160, 161, 162, 163, 164,
                                     170, 180, 190, 191,
                                     200, 201, 202, 203, 204,
                                     210, 211, 212, 213, 214
                                    };

extern byte time_since_last_sent_text_frame;
extern byte menu_width;

extern bool mega_enabled[4];


#ifdef USE_SERIAL_TO_MEGAS
bool enable_serial = true;
#else
bool enable_serial = false;
#endif

bool serial_enabled = false;


serial_tc5_declaration(RX_BUF_LENGTH, TX_BUF_LENGTH);
auto& Serial_1 = serial_tc5;
serial_tc6_declaration(RX_BUF_LENGTH, TX_BUF_LENGTH);
auto& Serial_2 = serial_tc6;
serial_tc7_declaration(RX_BUF_LENGTH, TX_BUF_LENGTH);
auto& Serial_3 = serial_tc7;
serial_tc8_declaration(RX_BUF_LENGTH, TX_BUF_LENGTH);
auto& Serial_4 = serial_tc8;

Mega_Serial_Parameters mega_parameters;

char ping_string[] PROGMEM = "ping";
char expected_ping_rx PROGMEM = 'p';

extern byte screen_brightness;
extern byte screen_mode;

//methods for Coms_Serial class

#ifdef USE_SERIAL_TO_MEGAS

void Coms_Serial::init_serial() {

  init_software_serial_to_megas(mega_parameters.baud_rate);
  Serial.println("Setup done");
  ping();
  init_frames(); //build constant parts of frames
}


void Coms_Serial::init_software_serial_to_megas(int speed) {   // initialise serial at specified speed, must be standardised speed 115200 or below, otherwise error thrown
  //ensure the speed is a standard baud rate
  if (speed != 300 && speed != 600 && speed != 1200 && speed != 2400 && speed != 4800 && speed != 14400 && speed != 9600 && speed != 14400 && speed != 19200 && speed != 28800 && speed != 38400 && speed != 57600 && speed != 115200)
    return;

  Serial_1.begin(
    RX_PIN_1,
    TX_PIN_1,
    speed,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );

  Serial_2.begin(
    RX_PIN_2,
    TX_PIN_2,
    speed,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );
  Serial_3.begin(
    RX_PIN_3,
    TX_PIN_3,
    speed,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );
  Serial_4.begin(
    RX_PIN_4,
    TX_PIN_4,
    speed,
    soft_uart::data_bit_codes::EIGHT_BITS,
    soft_uart::parity_codes::NO_PARITY,
    soft_uart::stop_bit_codes::ONE_STOP_BIT
  );

}

void Coms_Serial::ping() {

  delay(100);
  //  Serial_1.println(ping_string);
  //  Serial_2.println(ping_string);
  //  Serial_3.println(ping_string);
  //Serial_4.println(ping_string);

  int ping_time = millis();

  if (mega_enabled[0])
    Serial_1.write(ping_string, sizeof(ping_string));
  if (mega_enabled[1])
    Serial_2.write(ping_string, sizeof(ping_string));
  if (mega_enabled[2])
    Serial_3.write(ping_string, sizeof(ping_string));
  if (mega_enabled[3])
    Serial_4.write(ping_string, sizeof(ping_string));

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


void Coms_Serial::send_all_text_frames(bool send_now) {   // send the text frame to all megas

  if ((send_now = true) || (millis() > time_since_last_sent_text_frame + TEXT_TRANSMIT_PERIOD)) {
    time_since_last_sent_text_frame = millis();
    for (byte i = 0; i < MAX_NUM_OF_TEXT_OBJECTS; i++) {
      if (text_cursor[i].object_used)
        send_text_frame(i); //send this string to all megas
    }
  }
}

void Coms_Serial::send_text_frame(byte obj_num, byte address) {   //function to send strings to display on screen

  // function calculates the number of frames required to send the string, then loops,
  // generates a frame hader and fills up to 27 bytes of the string and calculates the checksum
  // it also calls the send frame function to send it on to the specified address when frame complete



  //text_cursor.x_min = -text_parameters.text_width * strlen(text_str) * 2; // set this based on size of string being sent, will update if string changed

  text_frame.num_frames = 1 + (strlen(text_str[obj_num]) / (FRAME_DATA_LENGTH)); //send this many frames
  text_frame.this_frame = 1;

  do {    //loop to send multiple frames if string is long

    if (text_frame.num_frames != text_frame.this_frame)
      text_frame.frame_buffer[0]  = MEGA_SERIAL_BUFFER_LENGTH;  //if there are more than one frame left to send, this frame is max size

    else
      text_frame.frame_buffer[0]  = strlen(text_str[obj_num]) - ((text_frame.num_frames - 1) * (FRAME_DATA_LENGTH)) + (HEADER_LENGTH + TRAILER_LENGTH) - 1; //remaining frame is (string length - text offset)+ (5 bytes overhead) -1 for obj_num


    text_frame.frame_buffer[1] = (byte) text_frame.frame_type;
    text_frame.frame_buffer[2] = (byte) text_frame.num_frames;
    text_frame.frame_buffer[3] = (byte) text_frame.this_frame;
    text_frame.frame_buffer[4] = obj_num;
    text_frame.checksum = text_frame.frame_buffer[0] + text_frame.frame_buffer[1] + text_frame.frame_buffer[2] + text_frame.frame_buffer[3] + text_frame.frame_buffer[4];

    pack_disp_string_frame(text_frame.this_frame, obj_num);//function to pack the frame with which ever data is relevant
    text_frame.frame_buffer[text_frame.frame_buffer[0] - 1] = (byte)256 - text_frame.checksum;

    write_text_frame(address);

    text_frame.this_frame++;   //increment this_frame after sending, will prepare for next loop or break
    delayMicroseconds(1000);       //small delay, want reciever to read through its buffer, otherwise the buffer may overload when we send next frame

  } while (text_frame.this_frame <= text_frame.num_frames);
}


void Coms_Serial::Serial_write_frame(byte address) {   //function to actually send the frame to given address

  if (address == 1 || address == 0) {
    if (mega_enabled[0] && mega_parameters.detected1) {
      for (int i = 0; i < text_frame.frame_buffer[0]; i++) {
        Serial_1.write(text_frame.frame_buffer[i]);
      }
    }
  }
  else if (address == 2 || address == 0) {
    if (mega_enabled[1] && mega_parameters.detected2) {
      for (int i = 0; i < text_frame.frame_buffer[0]; i++) {
        Serial_2.write(text_frame.frame_buffer[i]);
      }
    }
  }
  else if (address == 3 || address == 0) {
    if (mega_enabled[2] && mega_parameters.detected3) {
      for (int i = 0; i < text_frame.frame_buffer[0]; i++) {
        Serial_3.write(text_frame.frame_buffer[i]);
      }
    }
  }
  else  if (address == 4 || address == 0) {
    if (mega_enabled[3] && mega_parameters.detected4) {
      for (int i = 0; i < text_frame.frame_buffer[0]; i++) {
        Serial_4.write(text_frame.frame_buffer[i]);
      }
    }
  }
  //clear frame from last iteration
  for (int beta = 0; beta < MEGA_SERIAL_BUFFER_LENGTH; beta++) {
    text_frame.frame_buffer[beta] = 0;
  }
}



//void Coms_Serial::write_frame(int address) {
//#if defined(USE_SERIAL_TO_MEGAS)
//  Serial_write_frame(address);
//#else
//#error "I2C coms protocol not defined, cant send frame"
//#endif
//}



void Coms_Serial::send_menu_frame(int menu, int encoder_pos) { // build frame and call write_menu_frame for relevant addresses

  this -> build_menu_data_frame(menu, encoder_pos);

  if (menu_width != 0) {   //not sure why it would be this but include for completeness
    this -> write_menu_frame(3);  //write frame to address 3
  }

  if (menu_width > 64) {
    this -> write_menu_frame(2);
  }

  if (menu_width > 128) {
    this -> write_menu_frame(1);
  }

  if (menu_width > 192) {
    this -> write_menu_frame(0);
  }



}

void Coms_Serial::send_pos_frame() {  //build frame and send position to all megas

}

void write_sensor_data_frame(byte address) {

  if (address == 0 && mega_enabled[0] && mega_parameters.detected1)
    Serial_1.write(sensor_data_frame.frame_buffer, sensor_data_frame.frame_length);

  else if (address == 1 && mega_enabled[1] && mega_parameters.detected2)
    Serial_2.write(sensor_data_frame.frame_buffer, sensor_data_frame.frame_length);

  else if (address == 2 && mega_enabled[2] && mega_parameters.detected3)
    Serial_3.write(sensor_data_frame.frame_buffer, sensor_data_frame.frame_length);

  else if (address == 3 && mega_enabled[3] && mega_parameters.detected4)
    Serial_4.write(sensor_data_frame.frame_buffer, sensor_data_frame.frame_length);
}

void Coms_Serial::write_menu_frame(byte address) {   //function to actually send the frame to given address

  if (address == 0 && mega_enabled[0] && mega_parameters.detected1)
    Serial_1.write(menu_frame.frame_buffer, menu_frame.frame_length);

  else if (address == 1 && mega_enabled[1] && mega_parameters.detected2)
    Serial_2.write(menu_frame.frame_buffer, menu_frame.frame_length);

  else if (address == 2 && mega_enabled[2] && mega_parameters.detected3)
    Serial_3.write(menu_frame.frame_buffer, menu_frame.frame_length);

  else if (address == 3 && mega_enabled[3] && mega_parameters.detected4)
    Serial_4.write(menu_frame.frame_buffer, menu_frame.frame_length);
}

void Coms_Serial::write_pos_frame(byte address) {
  if (!mega_enabled[address]) {

    Sprint(F("Mega disabled, no pos sent \t address: "));
    Sprintln(address);
  }

  if (address == 0 && mega_enabled[0] && mega_parameters.detected1)
    Serial_1.write(pos_frame.frame_buffer, pos_frame.frame_length);

  else if (address == 1 && mega_enabled[1] && mega_parameters.detected2)
    Serial_2.write(pos_frame.frame_buffer, pos_frame.frame_length);

  else if (address == 2 && mega_enabled[2] && mega_parameters.detected3)
    Serial_3.write(pos_frame.frame_buffer, pos_frame.frame_length);

  else  if (address == 3 && mega_enabled[3] && mega_parameters.detected4)
    Serial_4.write(pos_frame.frame_buffer, pos_frame.frame_length);

  else {
    Sprint(F("Address invalid, no pos sent to mega \t attempted address:"));
    Sprintln(address);
  }
}


void Coms_Serial::write_text_frame(byte address) {
  if (!mega_enabled[address]) {

    Sprint(F("Mega disabled, no text sent \t address: "));
    Sprintln(address);
  }

  if (text_frame.send_extended_buffer) {
    this -> send_long_text_frame(address);
  }
  else {
    this -> send_short_text_frame(address);
  }
}


void Coms_Serial::send_short_text_frame(byte address) {
  if (address == 0) {
    for (int i = 0; i < text_frame.frame_length; i++) {
      Serial_1.write(text_frame.frame_buffer[i]);

    }
  }
  else if (address == 1) {
    for (int i = 0; i < text_frame.frame_length; i++) {
      Serial_2.write(text_frame.frame_buffer[i]);

    }
  }
  else if (address == 2) {
    for (int i = 0; i < text_frame.frame_length; i++) {
      Serial_3.write(text_frame.frame_buffer[i]);

    }
  }
  else  if (address == 3) {
    for (int i = 0; i < text_frame.frame_length; i++) {
      Serial_4.write(text_frame.frame_buffer[i]);

    }
  }
  else {
    Sprint(F("Address invalid, no pos sent to mega \t attempted address:"));
    Sprintln(address);
  }
}


void Coms_Serial::send_long_text_frame(byte address) {
  if (address == 0) {
    for (int i = 0; i < text_frame.frame_length; i++) {
      Serial_1.write(text_frame.frame_buffer[i]);

    }
  }
  else if (address == 1) {
    for (int i = 0; i < text_frame.frame_length; i++) {
      Serial_2.write(text_frame.frame_buffer[i]);

    }
  }
  else if (address == 2) {
    for (int i = 0; i < text_frame.frame_length; i++) {
      Serial_3.write(text_frame.frame_buffer[i]);

    }
  }
  else  if (address == 3) {
    for (byte j = 1; j < text_frame.num_frames; j++) {
      int offset = MEGA_SERIAL_BUFFER_LENGTH * (j - 1);
      for (byte i = 0; i < text_frame.frame_length; i++) {
        Serial_4.write(text_frame.frame_buffer[i + offset]);

      }
      delay(5); //short delay after sending each 32 byte frame
    }
    int offset = MEGA_SERIAL_BUFFER_LENGTH * (text_frame.num_frames - 1) - MEGA_SERIAL_BUFFER_LENGTH; // offset of final
    for (byte i = 1; i < text_frame.frame_buffer[offset + 3]; i++) {
      Serial_4.write(text_frame.frame_buffer[i + offset]);

    }
  }
  else {
    Sprint(F("Address invalid, no pos sent to mega \t attempted address:"));
    Sprintln(address);
  }
}

void Coms_Serial::write_text_frame() {}  // send to all at once


void Coms_Serial::check_queues() {

  check_pos_frame_queue();
  //check_sensor_date_frame_queue();
  //check_text_frame_queue();
  //check_menu_frame_queue();

}

inline void Coms_Serial::check_sensor_date_frame_queue() {
  if (sensor_data_frame.frame_queued) {     // check if frame was queued recently, if so send to all megas
    sensor_data_frame.frame_queued = false;
    for (byte i = 0; i < 4; i++) {
      write_sensor_data_frame(i);
    }
  }
}

inline void Coms_Serial::check_text_frame_queue() {
  if (text_frame.frame_queued) {     // check if frame was queued recently, if so send to all megas
    text_frame.frame_queued = false;
    for (byte i = 0; i < 4; i++) {
      write_text_frame(i);
    }
  }
}

inline void Coms_Serial::check_pos_frame_queue() {
  if (pos_frame.frame_queued) {     // check if frame was queued recently, if so send to all megas
    pos_frame.frame_queued = false;
    for (byte i = 0; i < 4; i++) {
      write_pos_frame(i);
    }
  }
}

inline void Coms_Serial::check_menu_frame_queue() {
  if (menu_frame.frame_queued) {     // check if frame was queued recently, if so send to all megas
    menu_frame.frame_queued = false;
    for (byte i = 0; i < 4; i++) {
      write_menu_frame(i);
    }
  }
}



void Coms_Serial::send_all_calibration_data(byte address) {     //function to send all data

  //function to send all the sensor data. loop through all sensor values

  byte frameNum = 1;
  byte numFrames = ((sizeof(to_mega_prefix_array) * 2) / 26) + 1;
  int offset = 0;
  bool frame_to_be_sent = false;

  sensor_data_frame.frame_buffer[1] = sensor_data_frame.frame_type;        //set frame starting bytes
  sensor_data_frame.frame_buffer[2] = numFrames;
  sensor_data_frame.frame_buffer[3] = frameNum;

  for (int alpha = 0; alpha < sizeof(to_mega_prefix_array) + 1; alpha++) {

    if (alpha == sizeof(to_mega_prefix_array)) { //if last byte
      frame_to_be_sent = send_specific_calibration_data(to_mega_prefix_array[alpha],  address, false,  offset);   //indicate this is the last element

    }
    else
      frame_to_be_sent = send_specific_calibration_data(to_mega_prefix_array[alpha],  address, true, offset);    //pack byte and dont send

    if (!frame_to_be_sent)  //if the frame was sent (function returns 1), reset offset otherwise increment
      offset++;

    else if (frame_to_be_sent) {
      frameNum++;     //increment the frame number
      offset = 0;     //reset offset for new frame

      write_sensor_data_frame(1);
      write_sensor_data_frame(2);
      write_sensor_data_frame(3);
      write_sensor_data_frame(4);

      sensor_data_frame.frame_buffer[1] = sensor_data_frame.frame_type;        //set frame starting bytes
      sensor_data_frame.frame_buffer[2] = numFrames;
      sensor_data_frame.frame_buffer[3] = frameNum;
    }
  }
}

bool Coms_Serial::send_specific_calibration_data(byte sensor_prefix, int address, bool more_bytes, int offset) { //sensor to send specific value

  // function to pack a frame with specific sensor data. the bool more_bytes can be used if htis is called as part of a loop to send more than one value
  // in the case that more_bytes is true it will hold off sending the frame until it is called and is false. offset is the number of sensor readings previously
  // written, so the place to write the new data is 4+2*offset for the prefix and 5+2*offset for the data
  byte type = 3;
  int HEADER_PLUS_ONE = HEADER_LENGTH + 1;

  //switch statement to pack the frame;
  switch (sensor_prefix) {
    case PREFIX_CURRENT_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = current_meter_parameters.reading2;
      break;

    case PREFIX_CURRENT_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = current_meter_parameters.reading2;
      break;

    case PREFIX_TEMP_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = temp_parameters.temp1;
      break;

    case PREFIX_TEMP_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = temp_parameters.temp2;
      break;

    case PREFIX_TEMP_3:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = temp_parameters.temp3;
      break;

    case PREFIX_LDR_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = light_sensor_parameters.reading1;
      break;

    case PREFIX_LDR_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = light_sensor_parameters.reading2;
      break;

    case PREFIX_FAN_SPEED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = fan_parameters.target_speed;
      break;

    case PREFIX_LED_STRIP_BRIGHTNESS:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = led_strip_parameters.target_brightness;
      break;

    case PREFIX_SD1_DETECTED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = card1.detected ? (byte) 1 : (byte) 0;   //convert boolean to byte
      break;

    case PREFIX_SD2_DETECTED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = card2.detected ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_EHTERNET_CONNECTED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = ethernet_connected ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_WIFI_CONNECTED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = wifi_connected ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_SCREEN_BRIGHTNESS:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      if (screen_brightness > 100) sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = screen_brightness = 100;
      else sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = screen_brightness;
      break;

    case PREFIX_TEXT_SIZE_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = text_parameters[0].text_size;
      break;

    case PREFIX_TEXT_COLOUR_R_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = text_parameters[0].red;
      break;


    case PREFIX_TEXT_COLOUR_G_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = text_parameters[0].green;
      break;


    case PREFIX_TEXT_COLOUR_B_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = text_parameters[0].blue;
      break;

    case PREFIX_TEXT_HUE_MSB_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = (byte)get_text_colour_hue(1, 0);   //function to geT the MS byte or LS byte, 1 returns MSB, 2 returns LSB
      break;

    case PREFIX_TEXT_HUE_LSB_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = (byte)get_text_colour_hue(2, 0);
      break;

    case PREFIX_TEXT_USE_HUE_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = text_parameters[0].use_hue ? (byte) 1 : (byte) 0;
      break;

    case PREFIX_DEBUG_STATE:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
#ifdef DEBUG
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = (byte) 1;
#else
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = (byte) 0;
#endif
      break;

    case PREFIX_SCREEN_MODE:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = screen_mode;
      break;

    case PREFIX_SD_MOUNTED_1:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = card1.enabled;
      break;

    case PREFIX_SD_MOUNTED_2:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = card2.enabled;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_X_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = text_cursor[0].x_pos_dir;
      break;

    case PREFIX_TEXT_SCROLL_SPEED_Y_0:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = text_cursor[0].y_pos_dir;
      break;

    case PREFIX_FAN_MINIMUM_SPEED:
      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = fan_parameters.fan_minimum;
      break;

    default:  Sprint("Error: Prefix not defined. Prefix :");
      Sprintln(sensor_prefix);

  }


  if (more_bytes && (HEADER_LENGTH + (offset * 2)) <= 29) { //this round element 29 an 30 written (ok), next round 30 and 31 writted then full
    return (false);
  }
  else {
    sensor_data_frame.frame_length = FRAME_OVERHEAD + (offset * 2);
    sensor_data_frame.frame_buffer[0] = sensor_data_frame.frame_length;
    sensor_data_frame.checksum = 0; //calculate checksum
    for (int alpha = 0; alpha < sensor_data_frame.frame_length - 1; alpha++) {
      sensor_data_frame.checksum = sensor_data_frame.checksum + sensor_data_frame.frame_buffer[alpha];
    }
    sensor_data_frame.frame_buffer[sensor_data_frame.frame_length - 1] = sensor_data_frame.checksum;
    sensor_data_frame.frame_queued = true;
    return (true);                    //frame_sent, send notification back
  }

}



#endif //USE_SERIAL_TO_MEGAS
#endif //Sign_coms_serial_CPP
