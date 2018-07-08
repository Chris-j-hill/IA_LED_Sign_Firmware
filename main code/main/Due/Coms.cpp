
#ifndef Coms_CPP
#define Coms_CPP

#include "Coms.h"
#include "Arduino.h"
#include "function_declarations.h"
#include "Due_Pins.h"
#include "Graphics.h"
#include "Current_Control.h"
#include "Fans.h"
#include "Internet.h"
#include "SD_Cards.h"
#include "Led_Strip.h"
#include "Encoder.h"


// variables

//create these structs
Frame text_frame;
Frame menu_frame;
Frame pos_frame;
Frame sensor_data_frame;

//give access to these structs
extern struct Led_Strip_Struct led_strip_parameters;
extern struct Temp_sensor temp_parameters;
extern struct Fan_Struct fan_parameters;        //create fan struct
extern struct Text text;
extern struct Text_cursor text_cursor;
extern struct Timers timers;
extern struct Encoder_Struct encoder_parameters;     //create encoder struct
extern struct Button_Struct button_parameters;       //create button struct
extern struct LDR_Struct light_sensor_parameters;
extern struct Current_Meter_Struct current_meter_parameters;
extern struct SD_Card card1;
extern struct SD_Card card2;
extern struct Text text_parameters;
extern struct Text_cursor text_cursor;

// list of valid sensor prefix's for sending non string data to the megas.
// append this as required and add to switch statements in due and mega code
const byte to_mega_prefix_array[] = {10, 11, 20, 21, 22, 30, 31, 40, 50, 60, 61, 70, 80, 90, 100, 110, 120, 130, 140, 150, 160, 170, 180};

byte time_since_last_sent_text_frame = 0;
bool send_text_now = false;
extern volatile bool send_pos_now;  //variable set in interrupt to trigger send pos function in main loop. (serial doesnt work in interrutps)


byte comms_delay = 0;
byte pos_frame_length = 13;   //length of frame to transmit to update pos

//char menu_frame[FRAME_OVERHEAD+3] ={0};   // initialise menu_frame, overhead +menu numeber + 2 bytes for encoder position
//                                          // should only send references to strings already in megas LUT. Names from files need to be handled seperately

extern char text_str[MAX_TWEET_SIZE];



extern byte screen_brightness;
extern byte screen_mode;   //mode of operation on startup should be both displaying
//mode0: both on
//mode1: one side on
//mode2: both off
//mode3: other side on



#ifndef ALL_MEGAS_ENABLED
    bool mega_enabled[4] = {MEGA_1_ENABLED, MEGA_2_ENABLED, MEGA_3_ENABLED, MEGA_4_ENABLED};
#else
    bool mega_enabled[4] = {true, true, true, true};  // ignore communication if board is false
#endif



int Coms::startup_handshake() {  //code to delay the due in initialisation and enable mega startup, simply wait until the megas all set ready pin high

#ifdef USING_SIMPLE_BINARY_NODE_READY_PROTOCOL
  pinMode(due_ready_pin, OUTPUT);           //due handshake
  pinMode(mega1_ready_pin, INPUT_PULLUP);
  pinMode(mega2_ready_pin, INPUT_PULLUP);
  pinMode(mega3_ready_pin, INPUT_PULLUP);
  pinMode(mega4_ready_pin, INPUT_PULLUP);

  digitalWrite(due_ready_pin, HIGH);

  bool ready = false;
  int startup_time = millis();
  while (!ready) {

    bool mega1_ready = !digitalRead(mega1_ready_pin);
    bool mega2_ready = !digitalRead(mega2_ready_pin);
    bool mega3_ready = !digitalRead(mega3_ready_pin);
    bool mega4_ready = !digitalRead(mega4_ready_pin);

    if (!mega_enabled[0]) mega1_ready = true;
    if (!mega_enabled[1]) mega2_ready = true;
    if (!mega_enabled[2]) mega3_ready = true;
    if (!mega_enabled[3]) mega4_ready = true;

    //Serial.println(mega1_ready);
    if (mega1_ready && mega2_ready && mega3_ready && mega4_ready)
      ready = true;

    if (millis() > startup_time + 5000) { //provide ample time for mega to start
      Serial.println(F("Startup time exceeded, mega(s) not responding"));
      return (-1);
    }
  }
  return (0);
#else

  delay(STARTUP_DUE_DELAY_PERIOD);

#endif
}

//int Coms::send_disp_string_frame(int address) {   //function to send strings to display on screen
//
//  // function calculates the number of frames required to send the string, then loops,
//  // generates a frame hader and fills up to 27 bytes of the string and calculates the checksum
//  // it also calls the send frame function to send it on to the specified address when frame complete
//
//
//
//  text_cursor.x_min = -text.text_width*strlen(text_str)*2; // set this based on size of string being sent, will update if string changed
//
//  text_frame.num_frames = 1 + (strlen(text_str) / (FRAME_DATA_LENGTH)); //send this many frames
//  text_frame.this_frame = 1;
//
//  do {    //loop to send multiple frames if string is long
//
//    if (text_frame.num_frames != text_frame.this_frame)
//      text_frame.frame_buffer[0]  = MEGA_SERIAL_BUFFER_LENGTH;  //if there are more than one frame left to send, this frame is max size
//
//    else
//      text_frame.frame_buffer[0]  = strlen(text_str) - ((frame.num_frames - 1) * (FRAME_DATA_LENGTH)) + (HEADER_LENGTH + TRAILER_LENGTH); //remaining frame is string length-text offset+5 bytes overhead
//
//
//    text_frame.frame_buffer[1] = (byte) text_frame.frame_type;
//    text_frame.frame_buffer[2] = (byte) text_frame.num_frames;
//    text_frame.frame_buffer[3] = (byte) text_frame.this_frame;
//    text_frame.checksum = text_frame.frame_buffer[0] + text_frame.frame_buffer[1] + text_frame.frame_buffer[2] + text_frame.frame_buffer[3] ;
//
//    pack_disp_string_frame(type, frame.this_frame);//function to pack the frame with which ever data is relevant
//    text_frame.frame_buffer[text_frame.frame_buffer[0] - 1] = (byte)256 - frame.checksum;
//
//    write_text_frame();
//
//    text_frame.this_frame++;   //increment this_frame after sending, will prepare for next loop or break
//    delayMicroseconds(10000);       //small delay, want reciever to read through its buffer, otherwise the buffer may overload when we send next frame
//
//  } while (text_frame.this_frame <= text_frame.num_frames);
//
//  return (0);
//}

void Coms::pack_disp_string_frame(int frame_type, int frame_offset) {   //function to pack a frame of text to display

  // function to pack a frame based on a given offset (ie this frames number)
  // maybe generalise later to accept calls from multiple frame building methods

  frame_offset = ((frame_offset - 1) * (FRAME_DATA_LENGTH)); //if this frame is 1 offset in data set is 0, if 2 offset 27, etc

  if (frame_type == 1) { //send str variable
    for (int i = 0; i < strlen(text_str) - frame_offset; i++) { //loop through string until end or break
      text_frame.frame_buffer[i + HEADER_LENGTH] = (byte)text_str[frame_offset + i];
      text_frame.checksum = text_frame.checksum + (byte)text_frame.frame_buffer[i + HEADER_LENGTH];
      if (i == FRAME_DATA_LENGTH) break;     //copy string until end or 28 bytes copied
    }
  }
}

void Coms::build_pos_frame() {


  Sprint(F("Sending cursor positions to address "));
  Sprintln(address);

  pack_xy_coordinates();        //seperate function to bit shift values to correct order.
  pos_frame.frame_buffer[8] =  text_cursor.x_pos_dir;
  pos_frame.frame_buffer[9] =  text_cursor.y_pos_dir;
  pos_frame.frame_buffer[10] = (byte) comms_delay;
  pos_frame.frame_buffer[11] = (byte) text_cursor.ISR_freq;

  pos_frame.frame_buffer[pos_frame.checksum_address] = pos_frame.header_checksum; //zero checksum
  for (int alpha = HEADER_LENGTH; alpha < pos_frame.checksum_address; alpha++) { //sum of elements
    pos_frame.frame_buffer[pos_frame.checksum_address] = pos_frame.frame_buffer[pos_frame.checksum_address] + pos_frame.frame_buffer[alpha];
  }
  pos_frame.frame_buffer[pos_frame.checksum_address] = (byte) 256 - (pos_frame.frame_buffer[pos_frame.checksum_address] % 256); //calc checksum

  pos_frame.frame_queued = true; //flag to send frame on new loop

  //return (0);
}

void Coms::pack_xy_coordinates() {       //function to pack the 4 bytes to send the x and y positions of the text cursor

  // Wire.write can only handle bytes (0-256) whereas we will need to use positive and negaitve values potientially significantly larger than 256
  // to accomplish this two sucessive bytes are sent to repersent one number. to deal with positive and negative numbers, the coordinate is split into
  // two bytes, leasat significant 8 bits are the second byte and the most significant 7 bits are in the first byte sent. if the coordinate is negaitve the
  // remaining bit is set 0 and 1 otherwise.

  // NOTE: current implementation will overflow if an out of bounds coordinate is presented (+/-32738 is usable)
  //       I cant see a reason this would be an issue so not fixing it for now

  byte x_pos_LSB = 0;
  byte x_pos_MSB = 0;
  byte y_pos_LSB = 0;
  byte y_pos_MSB = 0;


  if (abs(text_cursor.x) > 32738 || abs(text_cursor.y) > 32738) //print warning that coordinate will be wrong
    Sprintln("WARNING: failed to send correct coordinate, out of bounds, overflow likely to occur");

  if (text_cursor.x > 0) {
    x_pos_MSB = text_cursor.x >>8; //take the multiples 256 and set as MS byte
    x_pos_MSB = x_pos_MSB + 128; //greater than zero, set MSB to 1
    x_pos_LSB = text_cursor.x % 256; //take the modulo to get the LS byte
  }
  else {
    x_pos_MSB = text_cursor.x >>8; //take the multiples 256 and set as MS byte
    x_pos_LSB = text_cursor.x % 256; //take the modulo to get the LS byte
  }

  if (text_cursor.y > 0) {
    y_pos_MSB = text_cursor.y >>8; //take the multiples 256 and set as MS byte
    y_pos_MSB = y_pos_MSB + 128; //greater than zero, set MSB to 1
    y_pos_LSB = text_cursor.y % 256; //take the modulo to get the LS byte
  }
  else {
    y_pos_MSB = text_cursor.y >> 8; //take the multiples 256 and set as MS byte
    y_pos_LSB = text_cursor.y % 256; //take the modulo to get the LS byte
  }

  pos_frame.frame_buffer[4] = x_pos_MSB;   //write new values to frame
  pos_frame.frame_buffer[5] = x_pos_LSB;
  pos_frame.frame_buffer[6] = y_pos_MSB;
  pos_frame.frame_buffer[7] = y_pos_LSB;

}



//
//int Coms::send_all_calibration_data(int address) {     //function to send all data
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
//      //write_sensor_data_frame();
//      sensor_data_frame.frame_buffer[1] = sensor_data_frame.frame_type;        //set frame starting bytes
//      sensor_data_frame.frame_buffer[2] = numFrames;
//      sensor_data_frame.frame_buffer[3] = frameNum;
//    }
//
//  }
//
//  return (0);
//
//}
//
//bool Coms::send_specific_calibration_data(byte sensor_prefix, int address, bool more_bytes, int offset) { //sensor to send specific value
//
//  // function to pack a frame with specific sensor data. the bool more_bytes can be used if htis is called as part of a loop to send more than one value
//  // in the case that more_bytes is true it will hold off sending the frame until it is called and is false. offset is the number of sensor readings previously
//  // written, so the place to write the new data is 4+2*offset for the prefix and 5+2*offset for the data
//  byte type = 3;
//  int HEADER_PLUS_ONE = HEADER_LENGTH + 1;
//
//  //switch statement to pack the frame;
//  switch (sensor_prefix) {
//    case PREFIX_CURRENT_1:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = current_meter_parameters.reading2;
//      break;
//
//    case PREFIX_CURRENT_2:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = current_meter_parameters.reading2;
//      break;
//
//    case PREFIX_TEMP_1:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = temp_parameters.temp1;
//      break;
//
//    case PREFIX_TEMP_2:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = temp_parameters.temp2;
//      break;
//
//    case PREFIX_TEMP_3:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = temp_parameters.temp3;
//      break;
//
//    case PREFIX_LDR_1:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = light_sensor_parameters.reading1;
//      break;
//
//    case PREFIX_LDR_2:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = light_sensor_parameters.reading2;
//      break;
//
//    case PREFIX_FAN_SPEED:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = fan_parameters.target_speed;
//      break;
//
//    case PREFIX_LED_STRIP_BRIGHTNESS:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = led_strip_parameters.target_brightness;
//      break;
//
//    case PREFIX_SD1_DETECTED:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = card1.detected ? (byte) 1 : (byte) 0;   //convert boolean to byte
//      break;
//
//    case PREFIX_SD2_DETECTED:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = card2.detected ? (byte) 1 : (byte) 0;
//      break;
//
//    case PREFIX_EHTERNET_CONNECTED:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = ethernet_connected ? (byte) 1 : (byte) 0;
//      break;
//
//    case PREFIX_WIFI_CONNECTED:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = wifi_connected ? (byte) 1 : (byte) 0;
//      break;
//
//    case PREFIX_SCREEN_BRIGHTNESS:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      if (screen_brightness > 100) sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = screen_brightness = 100;
//      else sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = screen_brightness;
//      break;
//
//    case PREFIX_TEXT_SIZE:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = text_parameters.text_size;
//      break;
//
//    case PREFIX_TEXT_COLOUR_R:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = text_parameters.red;
//      break;
//
//
//    case PREFIX_TEXT_COLOUR_G:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = text_parameters.green;
//      break;
//
//
//    case PREFIX_TEXT_COLOUR_B:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = text_parameters.blue;
//      break;
//
//    case PREFIX_TEXT_HUE_MSB:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = (byte)get_text_colour_hue(1);   //function to geT the MS byte or LS byte, 1 returns MSB, 2 returns LSB
//      break;
//
//    case PREFIX_TEXT_HUE_LSB:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = (byte)get_text_colour_hue(2);
//      break;
//
//    case PREFIX_TEXT_USE_HUE:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = text_parameters.use_hue ? (byte) 1 : (byte) 0;
//      break;
//
//    case PREFIX_DEBUG_STATE:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//#ifdef DEBUG
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = (byte) 1;
//#else
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = (byte) 0;
//#endif
//      break;
//
//    case PREFIX_SCREEN_MODE:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = screen_mode;
//      break;
//
//    case PREFIX_SD_MOUNTED_1:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = card1.enabled;
//      break;
//
//    case PREFIX_SD_MOUNTED_2:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = card2.enabled;
//      break;
//
//    case PREFIX_TEXT_SCROLL_SPEED_X:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = text_cursor.x_pos_dir;
//      break;
//
//    case PREFIX_TEXT_SCROLL_SPEED_Y:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = text_cursor.y_pos_dir;
//      break;
//
//    case PREFIX_FAN_MINIMUM_SPEED:
//      sensor_data_frame.frame_buffer[HEADER_LENGTH + 2 * offset] = sensor_prefix;
//      sensor_data_frame.frame_buffer[HEADER_PLUS_ONE + 2 * offset] = fan_parameters.fan_minimum;
//      break;
//
//    default:  Sprint("Error: Prefix not defined. Prefix :");
//      Sprintln(sensor_prefix);
//
//  }
//
//
//  if (more_bytes && (HEADER_LENGTH + (offset * 2)) <= 29) { //this round element 29 an 30 written (ok), next round 30 and 31 writted then full
//    return (false);
//  }
//  else {
//    sensor_data_frame.frame_length = FRAME_OVERHEAD + (offset * 2);
//    sensor_data_frame.frame_buffer[0] = sensor_data_frame.frame_length;
//    sensor_data_frame.checksum = 0; //calculate checksum
//    for (int alpha = 0; alpha < sensor_data_frame.frame_length - 1; alpha++) {
//      sensor_data_frame.checksum = sensor_data_frame.checksum + sensor_data_frame.frame_buffer[alpha];
//    }
//    sensor_data_frame.frame_buffer[sensor_data_frame.frame_length - 1] = sensor_data_frame.checksum;
//    sensor_data_frame.frame_queued = true;
//    return (true);                    //frame_sent, send notification back
//  }
//
//}

int Coms::send_all_pos_on_interrupt() {    // function to send pos data to all megas if timer interrupt has indicated they should be sent

  if (!timers.pos_timer_attached) {
    Sprintln(F("Error: Cant send pos, timer not enabled"));
    return (-1);
  }

  if (send_pos_now) {   //send pos based on timer interrupt variable
    send_pos_frame(1);   // send frames
    send_pos_frame(2);
    send_pos_frame(3);
    send_pos_frame(4);
    send_pos_now = false;
  }
  return (0);
}

int Coms::send_all_pos_now() {   //function to send all the positional info now, not wait for timer to trigger flag

  send_pos_frame(1);   // send frames
  send_pos_frame(2);
  send_pos_frame(3);
  send_pos_frame(4);

  return (0);
}

int Coms::calc_delay() {    // function to calculate the dalay in sending frames to the megas
  // this could be useful for syncing up the screen updates. this value is only necessary for the pos data
  //frame as the other ones dont require accurate timing

  int beta = millis();

  send_all_pos_now();
  beta = millis() - beta;
  comms_delay = beta / 4; //time to send one frame

  return (0);
}

//int Coms::send_all_text() {   // send the text frame to all megas
//
//  if (millis()>time_since_last_sent_text_frame+TEXT_TRANSMIT_PERIOD){
//    time_since_last_sent_text_frame = millis();
//    for (int alpha = 1; alpha <= NUM_SCREENS; alpha++) {
//
//        Sprint(F("Sending text frame to address "));
//        Sprintln(alpha);
//
//      int fail = send_disp_string_frame(alpha);
//      if (fail != 0) {
//        Sprint(F("Failed to send string to mega"));
//        Sprintln(alpha);
//        return (-1);
//      }
//    }
//  }
//  return (0);
//}

void Coms::print_frame() {
  for (int alpha = 0; alpha < text_frame.frame_buffer[0]; alpha++) {
    Sprint((char)text_frame.frame_buffer[alpha]);
  }
  Sprintln("");
}

int get_text_colour_hue(int byte_number) {  //function to return the MSB or LSB of the current hue value to send

  if (byte_number == 1) { //looking for MSB
    if (text_parameters.hue < 0)
      return (abs(text_parameters.hue) / 256);    //get quotient of absolute value and 256 rounded down

    else
      return (abs(text_parameters.hue) / 256 + 128); //add 128 to indicate positve number
  }
  else if (byte_number == 2) { //LSB
    return (abs(text_parameters.hue) % 256);    //get modulo of value and 256;
  }
  else {
    Sprintln("Error, cant get hue MSB/LSB, invalid byte number presented");
    return (-1);
  }
}

int Coms::init_frames() {
  //menu_frame
  menu_frame.frame_length = FRAME_OVERHEAD + 3;
  menu_frame.frame_type = 4;

  menu_frame.frame_buffer[0] = menu_frame.frame_length;
  menu_frame.frame_buffer[1] = menu_frame.frame_type;
  menu_frame.frame_buffer[2] = 1;
  menu_frame.frame_buffer[3] = 1;
  menu_frame.header_checksum = menu_frame.frame_buffer[0] + menu_frame.frame_buffer[1] + menu_frame.frame_buffer[2] + menu_frame.frame_buffer[3];
  menu_frame.checksum_address = menu_frame.frame_length - 1;

  // pos frame
  pos_frame.frame_length = FRAME_OVERHEAD + 8;
  pos_frame.frame_type = 3;
  pos_frame.frame_buffer[0] = pos_frame.frame_length;
  pos_frame.frame_buffer[1] = pos_frame.frame_type;
  pos_frame.frame_buffer[2] = 1;
  pos_frame.frame_buffer[3] = 1;
  pos_frame.header_checksum = pos_frame.frame_buffer[0] + pos_frame.frame_buffer[1] + pos_frame.frame_buffer[2] + pos_frame.frame_buffer[3];
  pos_frame.checksum_address = pos_frame.frame_length - 1;

  // text frame
  text_frame.frame_type = 1;

  // sensor_data_frame
  sensor_data_frame.frame_type = 3;
}


int Coms::build_menu_data_frame(int menu_number, int encoder_position) {   //function to build the frame to send menu info
  int type = 4;

  Sprint(F("Building menu frame: Menu"));
  Sprint(menu_number);
  Sprint("\t Encoder Pos:");
  Sprintln(encoder_position);

  menu_frame.frame_buffer[4] = (byte) menu_number;
  menu_frame.frame_buffer[5] = (byte) get_text_encoder_position(1);
  menu_frame.frame_buffer[6] = (byte) get_text_encoder_position(2);

  menu_frame.frame_buffer[menu_frame.checksum_address] = menu_frame.header_checksum; //initial checksum

  for (int alpha = HEADER_LENGTH; alpha < menu_frame.checksum_address; alpha++) { //sum of elements
    menu_frame.frame_buffer[menu_frame.checksum_address] = menu_frame.frame_buffer[menu_frame.checksum_address] + menu_frame.frame_buffer[alpha];
  }
  menu_frame.frame_buffer[menu_frame.checksum_address] = (byte) 256 - (menu_frame.frame_buffer[menu_frame.checksum_address] % 256); //calc checksum

  menu_frame.frame_queued = true;   //flag to send frame on new loop

  return (0);
}


#endif // Coms_CPP
