
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
#include "LUTS.h"
#include "Config_Local.h"
#include "Menu_Tree.h"
#include "Host.h"
// variables

//create these structs
struct Frame text_frame;
struct Frame menu_frame;
struct Frame pos_frame;
struct Frame sensor_data_frame;
struct Frame ping_frame;


struct Frame_History text_frame_history[NUM_SCREENS];
struct Frame_History menu_frame_history[NUM_SCREENS];
struct Frame_History sensor_data_frame_history[NUM_SCREENS];
struct Frame_History pos_frame_history[NUM_SCREENS];




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

extern Encoder encoder;
extern Menu menu;
extern Host host;

bool send_text_now = false;
extern volatile bool send_pos_now;  //variable set in interrupt to trigger send pos function in main loop. (serial doesnt work in interrutps)


byte comms_delay = 0;
byte pos_frame_length = 13;   //length of frame to transmit to update pos

//char menu_frame[FRAME_OVERHEAD+3] ={0};   // initialise menu_frame, overhead +menu numeber + 2 bytes for encoder position
//                                          // should only send references to strings already in megas LUT. Names from files need to be handled seperately

extern char text_str[MAX_NUM_OF_TEXT_OBJECTS][MAX_TWEET_SIZE];



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



void Coms::pack_disp_string_frame(uint16_t frame_num, byte obj_num) {   //function to pack a frame of text to display

  // function to pack a frame based on a given offset (ie this frames_num)

  uint16_t frame_offset = ((frame_num - 1) * (FRAME_DATA_LENGTH)); //if this frame is 1 offset in data set is 0, if 2 offset 26, etc

  for (int i = 0; i < strlen(text_str[obj_num]) - frame_offset; i++) { //loop through string until end or break
    text_frame.frame_buffer[i + HEADER_LENGTH] = (byte)text_str[obj_num][frame_offset + i]; //HEADER_LENGTH+1 for text obj_num
    if (i == FRAME_DATA_LENGTH) break;     //copy string until end or max bytes copied (frame full)
  }


  //incorporate error correcting data
#ifdef DO_ERROR_CHECKING  //do at least minimal error checking using 8 bit checksum
#ifdef DO_HEAVY_ERROR_CHECKING     // parity checking each byte and 11 bit checksum, hamming encoding done inside function
  set_frame_parity_and_checksum(TEXT_FRAME_TYPE, text_frame.frame_length);

#else
#ifdef DO_HEADER_ERROR_CORRECTING    // just do hamming but no parity, then do 8 bit checksum
  hamming_encoder(frame_type);
  text_frame.frame_buffer[text_frame.frame_length - 2] = generate_checksum(TEXT_FRAME_TYPE);
#endif
#endif

#ifndef DO_HEAVY_ERROR_CHECKING //if neither defined, just do 8 bit checksum
#ifndef DO_HEADER_ERROR_CORRECTING
  text_frame.frame_buffer[text_frame.frame_length - 2] = generate_checksum(TEXT_FRAME_TYPE);
#endif
#endif
#endif
  text_frame.frame_buffer[text_frame.frame_length - 1] = ENDBYTE_CHARACTER;
}

void Coms::build_pos_frame(byte obj_num) {
  //bytes 0-2 pre initialised
  pos_frame.frame_buffer[3] = PACK_OBJ_NUM_DATA(obj_num);
  pack_xy_coordinates(obj_num);        //seperate function to bit shift values to correct order.
  pos_frame.frame_buffer[8] =  text_cursor[obj_num].x_pos_dir;
  pos_frame.frame_buffer[9] =  text_cursor[obj_num].y_pos_dir;
  pos_frame.frame_buffer[10] = comms_delay; //maybe implement this to sync up screens if needed

  set_frame_parity_and_checksum(POS_FRAME_TYPE, pos_frame.frame_length);
  pos_frame.frame_buffer[pos_frame.checksum_address + 1] = ENDBYTE_CHARACTER;

}

inline void word_packing(byte *dest, int16_t val) {

  if (val < 0) {
    byte neg_val = abs(val >> 8);
    neg_val |= 0b10000000;
    dest[0] = neg_val;
  }
  else {
    dest[0] = ((val >> 8) & 0x7F);   //write new values to frame
  }
  dest[1] = (val & 0xFF);

}

inline void Coms::pack_xy_coordinates(byte obj_num) {
  // function to pack the 4 bytes to send the x and y positions of the text cursor

  /*  due to the frame start and end bytes being 251-254, we want to avoid the possibility of incorrecty identifying
      two bytes as a start or end of frame. using a 16 bit sign value this will be broken up into 2 unsigned bytes
      a position in either axis of -1028 results in the bytes 251 and 252 being packed in the frame. Likewise
      -514 results in 253 and 254 will be packed. To avoid this we cant pack as simple 2s complement values.
      2's complement effectively takes the top half of the number line and moves it to the bottom:

      000 001 010 011 100 101 110 111  becomes  100 101 110 111 000 001 010 011

      what we want is to take the top half of the number line mirror it then add it to the bottom half of the number line

      000 001 010 011 100 101 110 111  becomes  111 110 101 100 000 001 010 011

      this must be done manually as its not a standard operation. if the value is negative this requires first taking the abs
      value then adding a 1 as the most significant bit .this will move the "danger zone" of pos values to the very
      negative values which is fine, its well away from where we'll be operating and the max pos can be reasonably restricted
      to be within these values. restricted range becomes -31743 to 32767
  */

  byte x_vals[2];
  byte y_vals[2];

  word_packing(x_vals, text_cursor[obj_num].x);
  word_packing(y_vals, text_cursor[obj_num].y);

  pos_frame.frame_buffer[4] = x_vals[0];
  pos_frame.frame_buffer[5] = x_vals[1];

  pos_frame.frame_buffer[6] = y_vals[0];
  pos_frame.frame_buffer[7] = y_vals[1];

  //  Serial.print("x pos, obj ");
  //  Serial.print(obj_num);
  //  Serial.print(": ");
  //  Serial.println(text_cursor[obj_num].x);
  //  Serial.print("x dir: ");
  //  Serial.println(text_cursor[obj_num].x_pos_dir);
  //  Serial.print("obj_used: ");
  //  Serial.println(text_cursor[obj_num].object_used);
  //
  //  host.println_bits(text_cursor[obj_num].x, 16, BIN);
  //  host.print_bits(pos_frame.frame_buffer[4], 8, BIN);
  //  host.println_bits(pos_frame.frame_buffer[5], 8, BIN);


  //  if (text_cursor[obj_num].x < 0) {
  //    byte neg_val = abs(text_cursor[obj_num].x >> 8);
  //    neg_val |= 0b10000000;
  //    pos_frame.frame_buffer[4] = neg_val;
  //  }
  //  else {
  //    pos_frame.frame_buffer[4] = ((text_cursor[obj_num].x >> 8) & 0x7F);   //write new values to frame
  //  }
  //  pos_frame.frame_buffer[5] = (text_cursor[obj_num].x & 0xFF);
  //
  //  if (text_cursor[obj_num].y < 0) {
  //    byte neg_val = abs(text_cursor[obj_num].y >> 8);
  //    neg_val |= 0b10000000;
  //    pos_frame.frame_buffer[6] = neg_val;
  //  }
  //  else {
  //    pos_frame.frame_buffer[6] = ((text_cursor[obj_num].y >> 8) & 0x7F);
  //  }
  //  pos_frame.frame_buffer[7] = (text_cursor[obj_num].y & 0xFF);

}


void Coms::calc_delay() {    // function to calculate the dalay in sending frames to the megas
  // this could be useful for syncing up the screen updates. this value is only necessary for the pos data
  //frame as the other ones dont require accurate timing
  int time_to_read_millis = abs(millis() - millis());
  uint32_t round_trip_time = millis();

  //send_all_pos_now();
  round_trip_time = millis() - round_trip_time - time_to_read_millis;
  comms_delay = round_trip_time / (NUM_SCREENS * 2); //time to send one frame

}

byte Coms::get_text_colour_hue(byte byte_number, byte obj_num) { //function to return the MSB or LSB of the current hue value to send

  if (byte_number == 1)
    return ((text_parameters[obj_num].hue >> 8) & 0xFF);   //return msb
  else
    return (text_parameters[obj_num].hue & 0xFF);

}

void Coms::init_frames() {

  // text frame
  text_frame.frame_type = TEXT_FRAME_TYPE;

  // pos frame
  pos_frame.frame_length = POS_FRAME_LENGTH;
  pos_frame.frame_type = POS_FRAME_TYPE;
  pos_frame.frame_buffer[0] = pos_frame.frame_length;
  pos_frame.frame_buffer[1] = pos_frame.frame_type;
  pos_frame.frame_buffer[2] = PACK_FRAME_NUM_DATA(1, 1);

  pos_frame.header_checksum = pos_frame.frame_buffer[0] + pos_frame.frame_buffer[1] + pos_frame.frame_buffer[2];
  pos_frame.checksum_address = pos_frame.frame_length - 2;

  // sensor_data_frame
  sensor_data_frame.frame_type = SENSOR_FRAME_TYPE;
  sensor_data_frame.frame_buffer[1] = sensor_data_frame.frame_type;
  sensor_data_frame.frame_buffer[2] = PACK_FRAME_NUM_DATA(1, 1); //doesnt matter if it thinks theres one frame or many, data not related to eachother

  sensor_data_frame.header_checksum = sensor_data_frame.frame_buffer[1] + sensor_data_frame.frame_buffer[2];


  //menu_frame
  menu_frame.frame_length = MENU_FRAME_LENGTH;  //three pieces of data, current menu + 2 encoder pos bytes
  menu_frame.frame_type = MENU_FRAME_TYPE;

  menu_frame.frame_buffer[0] = menu_frame.frame_length;
  menu_frame.frame_buffer[1] = menu_frame.frame_type;
  menu_frame.frame_buffer[2] = PACK_FRAME_NUM_DATA(1, 1);

  menu_frame.header_checksum = menu_frame.frame_buffer[0] + menu_frame.frame_buffer[1] + menu_frame.frame_buffer[2];
  menu_frame.checksum_address = menu_frame.frame_length - 2;


  //ping frame
  ping_frame.frame_length = PING_FRAME_LENGTH;
  ping_frame.frame_type = PING_STRING_TYPE;
  ping_frame.frame_buffer[0] = ping_frame.frame_length;
  ping_frame.frame_buffer[1] = ping_frame.frame_type;
  ping_frame.frame_buffer[2] = PACK_FRAME_NUM_DATA(1, 1);
  ping_frame.frame_buffer[3] = PACK_OBJ_NUM_DATA(0);
  ping_frame.frame_buffer[4] = baud_LUT(COMS_SPEED);  //set the baud rate based on LUT values

  //if we will do parity checking, set parity here for constants
#ifdef DO_HEAVY_ERROR_CHECKING
  sensor_data_frame.frame_buffer[1] = (sensor_data_frame.frame_buffer[1] << 1) | (parity_of(sensor_data_frame.frame_buffer[1]));
  sensor_data_frame.frame_buffer[2] = (sensor_data_frame.frame_buffer[2]       | (parity_of(GET_FRAME_NUM_DATA(sensor_data_frame.frame_buffer[2])) << 4));
  sensor_data_frame.frame_buffer[2] = (sensor_data_frame.frame_buffer[2]       | (parity_of(GET_THIS_FRAME_DATA(sensor_data_frame.frame_buffer[2] ))));

  menu_frame.frame_buffer[0] = (menu_frame.frame_buffer[0] << 1) | (parity_of(menu_frame.frame_buffer[0]));
  menu_frame.frame_buffer[1] = (menu_frame.frame_buffer[1] << 1) | (parity_of(menu_frame.frame_buffer[1]));
  menu_frame.frame_buffer[2] = (menu_frame.frame_buffer[2]       | (parity_of(GET_FRAME_NUM_DATA(menu_frame.frame_buffer[2])) << 4));
  menu_frame.frame_buffer[2] = (menu_frame.frame_buffer[2]       | (parity_of(GET_THIS_FRAME_DATA( menu_frame.frame_buffer[2] ))));

  ping_frame.frame_buffer[0] = (ping_frame.frame_buffer[0] << 1) | (parity_of(ping_frame.frame_buffer[0]));
  ping_frame.frame_buffer[1] = (ping_frame.frame_buffer[1] << 1) | (parity_of(ping_frame.frame_buffer[1]));
  ping_frame.frame_buffer[2] = (ping_frame.frame_buffer[2]       | (parity_of(GET_FRAME_NUM_DATA(ping_frame.frame_buffer[2])) << 4));
  ping_frame.frame_buffer[2] = (ping_frame.frame_buffer[2]       | (parity_of(GET_THIS_FRAME_DATA( ping_frame.frame_buffer[2] ))));
  ping_frame.frame_buffer[3] = (ping_frame.frame_buffer[3]       | (parity_of(ping_frame.frame_buffer[3])));
  ping_frame.frame_buffer[4] = (ping_frame.frame_buffer[4] << 1) | (parity_of(ping_frame.frame_buffer[4]));


  pos_frame.frame_buffer[0] = (pos_frame.frame_buffer[0] << 1) | (parity_of(pos_frame.frame_buffer[0]));
  pos_frame.frame_buffer[1] = (pos_frame.frame_buffer[1] << 1) | (parity_of(pos_frame.frame_buffer[1]));
  pos_frame.frame_buffer[2] = (pos_frame.frame_buffer[2]       | (parity_of(GET_FRAME_NUM_DATA(pos_frame.frame_buffer[2])) << 4));



#endif

}


void Coms::build_menu_data_frame(byte menu_number) {   //function to build the frame to send menu info

//weird errors of header data getting over written, explicitly set for each menu
//NB: error occurs returning from adjustment menu, header becomes corrupted
 
  menu_frame.frame_buffer[0] = MENU_FRAME_LENGTH;
  menu_frame.frame_buffer[1] = MENU_FRAME_TYPE;
  menu_frame.frame_buffer[2] = PACK_FRAME_NUM_DATA(1, 1);
#ifdef DO_HEAVY_ERROR_CHECKING
  menu_frame.frame_buffer[0] = (menu_frame.frame_buffer[0] << 1) | (parity_of(menu_frame.frame_buffer[0]));
  menu_frame.frame_buffer[1] = (menu_frame.frame_buffer[1] << 1) | (parity_of(menu_frame.frame_buffer[1]));
  menu_frame.frame_buffer[2] = (menu_frame.frame_buffer[2]       | (parity_of(GET_FRAME_NUM_DATA(menu_frame.frame_buffer[2])) << 4));
  menu_frame.frame_buffer[2] = (menu_frame.frame_buffer[2]       | (parity_of(GET_THIS_FRAME_DATA( menu_frame.frame_buffer[2] ))));
#endif

  menu_frame.frame_buffer[3] = (byte) menu.get_selected_object();
  menu_frame.frame_buffer[4] = (byte) menu_number;
  menu_frame.frame_buffer[5] = (byte) encoder.get_text_encoder_position(1);
  menu_frame.frame_buffer[6] = (byte) encoder.get_text_encoder_position(2);

  set_frame_parity_and_checksum(MENU_FRAME_TYPE, menu_frame.frame_length);
  //  menu_frame.frame_buffer[menu_frame.checksum_address] = generate_checksum(MENU_FRAME_TYPE);
  menu_frame.frame_buffer[menu_frame.checksum_address + 1] = ENDBYTE_CHARACTER;


}

uint16_t Coms::generate_checksum(byte frame_type, uint16_t modulo_mask) {

  byte *frame_index_zero;  //pointer to address of first element of whichever array
  byte frame_length;

  switch (frame_type) {
    case TEXT_FRAME_TYPE:
      frame_length = text_frame.frame_length;
      frame_index_zero = text_frame.frame_buffer;   //pointer to this buffer element 0
      break;
    case POS_FRAME_TYPE:
      frame_length = pos_frame.frame_length;
      frame_index_zero = pos_frame.frame_buffer;    // pointer to this defferent buffer
      break;
    case SENSOR_FRAME_TYPE:
      frame_length = sensor_data_frame.frame_length;
      frame_index_zero = sensor_data_frame.frame_buffer;
      break;
    case MENU_FRAME_TYPE:
      frame_length = menu_frame.frame_length;
      frame_index_zero = menu_frame.frame_buffer;
      break;
    case PING_STRING_TYPE:
      frame_length = ping_frame.frame_length;
      frame_index_zero = ping_frame.frame_buffer;
      break;
    default:
      Serial.println("checksum calc error");
      return (0);
  }

  //calculate checksum
  uint16_t checksum = 0;  //set checksum as 16 bit number and modulo to fit
  for (byte i = 0; i < frame_length - 2; i++) {
    if ( i != CHECKSUM_3_BIT_LOC) //dont include any prior value in this loc
      checksum += *(frame_index_zero + i);          //sum all elements
    else
      checksum += (*(frame_index_zero + i)) & 0b11110001; //mask out any previous checksum value in this location
  }
  //  Serial.print("Checksum = ");
  //  Serial.println(checksum);
  //  Serial.println(checksum, BIN);
  //  checksum = checksum & modulo_mask;
  //  Serial.println(checksum, BIN);
  return checksum;

}

bool Coms::error_sanity_check(byte frame_num, byte obj_num) {
  if (!text_cursor[obj_num].object_used)
    return false;
  if (frame_num > (1 + (strlen(text_str[obj_num]) / (FRAME_DATA_LENGTH - 1)))) //requested frame num not possible for this obj
    return false;
  return true;
}


void Coms::set_frame_parity_and_checksum(byte frame_type, byte frame_length) {

  //heavy error checking sets parity of all bytes and 11 bit checksum
#ifdef DO_HEAVY_ERROR_CHECKING
  set_header_parity(frame_type);  //same regardless of frame type or length
#endif

  //header error correction applies hamming code
#ifdef DO_HEADER_ERROR_CORRECTING
  hamming_encoder(frame_type);
#endif

#ifdef DO_HEAVY_ERROR_CHECKING
  if (frame_type == TEXT_FRAME_TYPE) {
    CLEAR_HEADER_CHECKSUM(text_frame.frame_buffer[CHECKSUM_3_BIT_LOC]);  //clear these bits from prior frame
    set_buffer_parity_bits(text_frame.frame_buffer, BYTE_PARITY_LOC , text_frame.frame_length - TRAILER_LENGTH, HEADER_LENGTH);
    set_verical_parity_byte(text_frame.frame_buffer , text_frame.frame_length - TRAILER_LENGTH);
    set_checksum_11(generate_checksum_11(frame_type), frame_type); //macro to generate 11 bit checksum
  }

  else if (frame_type == POS_FRAME_TYPE) {

    CLEAR_HEADER_CHECKSUM(pos_frame.frame_buffer[CHECKSUM_3_BIT_LOC]);
    set_verical_parity_byte(pos_frame.frame_buffer , pos_frame.frame_length - TRAILER_LENGTH);
    set_checksum_11(generate_checksum_11(frame_type), frame_type);
  }

  else if (frame_type == SENSOR_FRAME_TYPE) {

    //    CLEAR_HEADER_CHECKSUM(sensor_data_frame.frame_buffer[CHECKSUM_3_BIT_LOC]);// this isnt clearing it...
    sensor_data_frame.frame_buffer[CHECKSUM_3_BIT_LOC] = 0;                     // but obj_num always zero for sensor data frame

    set_verical_parity_byte(sensor_data_frame.frame_buffer, sensor_data_frame.frame_length - TRAILER_LENGTH);
    set_checksum_11(generate_checksum_11(frame_type), frame_type);
  }

  else if (frame_type == MENU_FRAME_TYPE) {

    CLEAR_HEADER_CHECKSUM(menu_frame.frame_buffer[CHECKSUM_3_BIT_LOC]);
    set_verical_parity_byte(menu_frame.frame_buffer , menu_frame.frame_length - TRAILER_LENGTH);
    set_checksum_11(generate_checksum_11(frame_type), frame_type);
  }

  else if (frame_type == PING_STRING_TYPE) {

    CLEAR_HEADER_CHECKSUM(ping_frame.frame_buffer[CHECKSUM_3_BIT_LOC]);
    set_buffer_parity_bits(ping_frame.frame_buffer, BYTE_PARITY_LOC, pos_frame.frame_length - TRAILER_LENGTH, HEADER_LENGTH);
    set_verical_parity_byte(ping_frame.frame_buffer , ping_frame.frame_length - TRAILER_LENGTH);
    set_checksum_11(generate_checksum_11(frame_type), frame_type);
  }
  else return;
#endif
}

void Coms::set_header_parity(byte frame_type) {

  switch (frame_type) {
    case TEXT_FRAME_TYPE:
      text_frame.frame_buffer[0] = (text_frame.frame_buffer[0] << 1) | (parity_of(text_frame.frame_buffer[0]));
      text_frame.frame_buffer[1] = (text_frame.frame_buffer[1] << 1) | (parity_of(text_frame.frame_buffer[1]));
      text_frame.frame_buffer[2] = (text_frame.frame_buffer[2]       | (parity_of(GET_FRAME_NUM_DATA(text_frame.frame_buffer[2])) << 4)); //isolate three data bits, get parity, move parity in correct loc
      text_frame.frame_buffer[2] = (text_frame.frame_buffer[2]       | (parity_of(GET_THIS_FRAME_DATA( text_frame.frame_buffer[2] ))));
      text_frame.frame_buffer[3] = (text_frame.frame_buffer[3]       | (parity_of(text_frame.frame_buffer[3])));
      break;
    case POS_FRAME_TYPE:
      pos_frame.frame_buffer[2] = (pos_frame.frame_buffer[2]       | (parity_of(GET_THIS_FRAME_DATA( pos_frame.frame_buffer[2] ))));
      pos_frame.frame_buffer[3] = (pos_frame.frame_buffer[3]       | (parity_of(pos_frame.frame_buffer[3])));
      break;

    case SENSOR_FRAME_TYPE:
      sensor_data_frame.frame_buffer[0] = (sensor_data_frame.frame_buffer[0] << 1) | (parity_of(sensor_data_frame.frame_buffer[0]));
      //      sensor_data_frame.frame_buffer[1] = (sensor_data_frame.frame_buffer[1] << 1) | (parity_of(sensor_data_frame.frame_buffer[1]));
      //      sensor_data_frame.frame_buffer[2] = (sensor_data_frame.frame_buffer[2]       | (parity_of(GET_FRAME_NUM_DATA(sensor_data_frame.frame_buffer[2])) << 4));
      //      sensor_data_frame.frame_buffer[2] = (sensor_data_frame.frame_buffer[2]       | (parity_of(GET_THIS_FRAME_DATA(sensor_data_frame.frame_buffer[2] ))));
      //      sensor_data_frame.frame_buffer[3] = (sensor_data_frame.frame_buffer[3]       | (parity_of(sensor_data_frame.frame_buffer[3]))); //<- this is irrelevant, no obj num on sensor frame
      break;

    case MENU_FRAME_TYPE:
      //      menu_frame.frame_buffer[0] = (menu_frame.frame_buffer[0] << 1) | (parity_of(menu_frame.frame_buffer[0]));
      //      menu_frame.frame_buffer[1] = (menu_frame.frame_buffer[1] << 1) | (parity_of(menu_frame.frame_buffer[1]));
      //      menu_frame.frame_buffer[2] = (menu_frame.frame_buffer[2]       | (parity_of(GET_FRAME_NUM_DATA(menu_frame.frame_buffer[2])) << 4));
      //      menu_frame.frame_buffer[2] = (menu_frame.frame_buffer[2]       | (parity_of(GET_THIS_FRAME_DATA( menu_frame.frame_buffer[2] ))));
      menu_frame.frame_buffer[3] = (menu_frame.frame_buffer[3]       | (parity_of(menu_frame.frame_buffer[3])));
      break;

    case PING_STRING_TYPE:
      //      ping_frame.frame_buffer[0] = (ping_frame.frame_buffer[0] << 1) | (parity_of(ping_frame.frame_buffer[0]));
      //      ping_frame.frame_buffer[1] = (ping_frame.frame_buffer[1] << 1) | (parity_of(ping_frame.frame_buffer[1]));
      //      ping_frame.frame_buffer[2] = (ping_frame.frame_buffer[2]       | (parity_of(GET_FRAME_NUM_DATA(ping_frame.frame_buffer[2])) << 4));
      //      ping_frame.frame_buffer[2] = (ping_frame.frame_buffer[2]       | (parity_of(GET_THIS_FRAME_DATA( ping_frame.frame_buffer[2] ))));
      //      ping_frame.frame_buffer[3] = (ping_frame.frame_buffer[3]       | (parity_of(ping_frame.frame_buffer[3])));
      break;

    default:
      Serial.println("header parity calc error");
      return;
  }
}

inline byte Coms::parity_of(byte value) {
  return parity[value];   //LUT of parity given up to 8 bit value
}



void Coms::set_buffer_parity_bits(byte *buf, byte bit_loc, int buf_length, int start_from) { // set parity of last bit for all bytes excpet last two(ie the checksums, which is dependant on the value of the bytes)

  //loc = 0 parity bit is MSB
  //loc = 7 parity bit is LSB


  //NB:  logic not working for non MSB parity bit

  byte loc_bit_suppress_mask = 0;
  switch (bit_loc) {
    case 0:
      loc_bit_suppress_mask = 0x7F;
      break;
    case 1:
      loc_bit_suppress_mask = 0xBF;
      break;
    case 2:
      loc_bit_suppress_mask = 0xDF;
      break;
    case 3:
      loc_bit_suppress_mask  = 0xEF;
      break;
    case 4:
      loc_bit_suppress_mask = 0xF7;
      break;
    case 5:
      loc_bit_suppress_mask = 0xFB;
      break;
    case 6:
      loc_bit_suppress_mask = 0xFD;
      break;
    case 7:
      loc_bit_suppress_mask = 0xFE;
      break;
    default: return;
  }
  byte suppress_data_mask = ~ loc_bit_suppress_mask; //is inverse of suppress mask
  byte shift_bit_by = 7 - bit_loc;  //locate bit in byte for parity
  byte shift_data_by = 1; //hard code data shift, change this to split data and shift in two parts to allow parity to be located anywhere

  for (int i = start_from; i < buf_length; i++) {
    buf[i] = ((buf[i] << shift_data_by)  & loc_bit_suppress_mask) | (parity_of(buf[i]) & suppress_data_mask);
  }
}



void Coms::set_verical_parity_byte(byte *buf, int checksum_loc, int start_byte) {


  byte mask = 0x1;
  buf[checksum_loc] = 0;
  for (byte j = 0; j < 8 ; j++) {

    byte count = 0; //total set bits for this column
    for (int i = start_byte; i < checksum_loc; i++) {
      if (i != CHECKSUM_3_BIT_LOC)
        count += ((buf[i] >> j) & mask);
      else
        count += (((buf[i] & (~CHECKSUM_3_BIT_MASK)) >> j) & mask); //suppress and prior value in 3 bit checksum location
    }
    buf[checksum_loc] = buf[checksum_loc] | ((count & mask) << j);
  }
}


void Coms::set_checksum_11(uint16_t checksum, byte frame_type) {

  byte three_bit = ((checksum >> 7) & 0b00001110);  //shift by 7 and ignore lowest bit
  byte eight_bit = (checksum & 0xFF);
  byte dist_from_frame_end = 2;
  switch (frame_type) {
    case TEXT_FRAME_TYPE:
      text_frame.frame_buffer[CHECKSUM_3_BIT_LOC] = (text_frame.frame_buffer[CHECKSUM_3_BIT_LOC] | (three_bit));
      text_frame.frame_buffer[text_frame.frame_length - dist_from_frame_end] = eight_bit;
      break;
    case POS_FRAME_TYPE:
      pos_frame.frame_buffer[CHECKSUM_3_BIT_LOC] = (pos_frame.frame_buffer[CHECKSUM_3_BIT_LOC] | (three_bit));
      pos_frame.frame_buffer[pos_frame.frame_length - dist_from_frame_end] = eight_bit;
      break;
    case SENSOR_FRAME_TYPE:
      sensor_data_frame.frame_buffer[CHECKSUM_3_BIT_LOC] = (sensor_data_frame.frame_buffer[CHECKSUM_3_BIT_LOC] | (three_bit));
      sensor_data_frame.frame_buffer[sensor_data_frame.frame_length - dist_from_frame_end] = eight_bit;
      break;
    case MENU_FRAME_TYPE:
      menu_frame.frame_buffer[CHECKSUM_3_BIT_LOC] = (menu_frame.frame_buffer[CHECKSUM_3_BIT_LOC] | (three_bit));
      menu_frame.frame_buffer[menu_frame.frame_length - dist_from_frame_end] = eight_bit;
      break;
    case PING_STRING_TYPE:
      ping_frame.frame_buffer[CHECKSUM_3_BIT_LOC] = (ping_frame.frame_buffer[CHECKSUM_3_BIT_LOC] | (three_bit));
      ping_frame.frame_buffer[ping_frame.frame_length - 2] = eight_bit;
      break;
    default:
      Serial.println("set checksum error");
      return;

  }
}


void Coms::append_frame_history(byte *buf, byte address, byte frame_type) {

  static bool first_run = true;

  if (first_run) {
    first_run = false;
    for (byte i = 0; i < NUM_SCREENS; i++) {
      text_frame_history[i].num_populated_buffers = 0;
      pos_frame_history[i].num_populated_buffers = 0;
      sensor_data_frame_history[i].num_populated_buffers = 0;
      menu_frame_history[i].num_populated_buffers = 0;
    }
  }

  byte frame_length = EXTRACT_FRAME_LENGTH(buf[0]);

  byte padded_buf[MEGA_SERIAL_BUFFER_LENGTH] = {'\0'};
  memcpy(padded_buf, buf, frame_length); //make sure frame will overwrite all of previous content, so make length max


  switch (frame_type) {

    case TEXT_FRAME_TYPE:
      memcpy(text_frame_history[address].frame_content[text_frame_history[address].history_index], padded_buf, MEGA_SERIAL_BUFFER_LENGTH); //copy frame buffer into ring buffer for storage
      text_frame_history[address].history_index = (text_frame_history[address].history_index + 1) % FRAME_HISTORY_MEMORY_DEPTH;            //increment index
      text_frame_history[address].num_populated_buffers++;                                                                                 //increment number of frames sent

      break;

    case POS_FRAME_TYPE:
      memcpy(pos_frame_history[address].frame_content[pos_frame_history[address].history_index], padded_buf, MEGA_SERIAL_BUFFER_LENGTH);
      pos_frame_history[address].history_index = (pos_frame_history[address].history_index + 1) % FRAME_HISTORY_MEMORY_DEPTH;
      pos_frame_history[address].num_populated_buffers++;
      break;

    case SENSOR_FRAME_TYPE:
      memcpy(sensor_data_frame_history[address].frame_content[sensor_data_frame_history[address].history_index], padded_buf, MEGA_SERIAL_BUFFER_LENGTH);
      sensor_data_frame_history[address].history_index = (sensor_data_frame_history[address].history_index + 1) % FRAME_HISTORY_MEMORY_DEPTH;
      sensor_data_frame_history[address].num_populated_buffers++;
      break;

    case MENU_FRAME_TYPE:
      memcpy(menu_frame_history[address].frame_content[menu_frame_history[address].history_index], padded_buf, MEGA_SERIAL_BUFFER_LENGTH);
      menu_frame_history[address].history_index = (menu_frame_history[address].history_index + 1) % FRAME_HISTORY_MEMORY_DEPTH;
      menu_frame_history[address].num_populated_buffers++;
      break;
  }

}


byte Coms::find_in_frame_history(byte address, byte frame_type, byte frame_num, byte obj_num) {

  bool frame_num_found = false;
  bool obj_num_found = false;

  byte history_index = 0; //get starting index as most recently added frame, then scan back through stored frames
  switch (frame_type) {
    case TEXT_FRAME_TYPE:   history_index = text_frame_history[address].history_index;          break;
    case POS_FRAME_TYPE:    history_index = pos_frame_history[address].history_index;           break;
    case SENSOR_FRAME_TYPE: history_index = sensor_data_frame_history[address].history_index;   break;
    case MENU_FRAME_TYPE:   history_index = menu_frame_history[address].history_index;          break;
  }

  byte frames_checked = 1;  //exit if num frames checks is more than stored
  bool break_loop = false;

  for (int i = history_index; i != history_index + 1; i--) {
    if (i == -1) i = FRAME_HISTORY_MEMORY_DEPTH - 1; // loop back index

    switch (frame_type) {

      case TEXT_FRAME_TYPE:
        if (frames_checked > text_frame_history[address].num_populated_buffers) break_loop = true;
        if (EXTRACT_THIS_FRAME_DATA(text_frame_history[address].frame_content[i][2]) == frame_num) frame_num_found = true;
        if (EXTRACT_OBJ_NUM_DATA(text_frame_history[address].frame_content[i][3]) == obj_num)      obj_num_found = true;

        break;

      case POS_FRAME_TYPE:
        if (frames_checked > pos_frame_history[address].num_populated_buffers) break_loop = true;
        if (EXTRACT_THIS_FRAME_DATA(pos_frame_history[address].frame_content[i][2]) == frame_num) frame_num_found = true;
        if (EXTRACT_OBJ_NUM_DATA(pos_frame_history[address].frame_content[i][3]) == obj_num)      obj_num_found = true;
        break;

      case SENSOR_FRAME_TYPE:
        if (frames_checked > sensor_data_frame_history[address].num_populated_buffers) break_loop = true;
        if (EXTRACT_THIS_FRAME_DATA(sensor_data_frame_history[address].frame_content[i][2]) == frame_num) frame_num_found = true;
        if (EXTRACT_OBJ_NUM_DATA(sensor_data_frame_history[address].frame_content[i][3]) == obj_num)      obj_num_found = true;
        break;

      case MENU_FRAME_TYPE:
        if (frames_checked > menu_frame_history[address].num_populated_buffers) break_loop = true;
        if (EXTRACT_THIS_FRAME_DATA(menu_frame_history[address].frame_content[i][2]) == frame_num) frame_num_found = true;
        if (EXTRACT_OBJ_NUM_DATA(menu_frame_history[address].frame_content[i][3]) == obj_num)      obj_num_found = true;
        break;
    }

    if (break_loop) break;

    if (frame_num_found && obj_num_found) {
      return (i);
    }
    frame_num_found = false;  //reset for checking next stored frame
    obj_num_found = false;

  }

  return FRAME_HISTORY_MEMORY_DEPTH; //return impossible value if not found

}


bool Coms::request_error_sanity_check(byte frame_type, byte frame_num, byte obj_num) { //sanity check returned frame to make sure retransmit request is reasonable


  if (frame_type == UNKNOWN_RETRANSMIT_TYPE)                                                                                                      goto bad_frame;

  else if (frame_type != TEXT_FRAME_TYPE && frame_type != POS_FRAME_TYPE && frame_type != SENSOR_FRAME_TYPE && frame_type != MENU_FRAME_TYPE)     goto bad_frame;

  else if (frame_type == TEXT_FRAME_TYPE && frame_num > EXPECTED_MAX_TEXT_FRAMES)                                                                 goto bad_frame;
  else if (frame_type == POS_FRAME_TYPE && frame_num > 1)                                                                                         goto bad_frame;
  else if (frame_type == SENSOR_FRAME_TYPE && frame_num > 1)                                                                                      goto bad_frame;
  else if (frame_type == MENU_FRAME_TYPE && frame_num > 1)                                                                                        goto bad_frame;

  else if (frame_type == TEXT_FRAME_TYPE && obj_num >= MAX_NUM_OF_TEXT_OBJECTS)                                                                   goto bad_frame;
  else if (frame_type == POS_FRAME_TYPE && obj_num >= MAX_NUM_OF_TEXT_OBJECTS)                                                                    goto bad_frame;
  else if (frame_type == SENSOR_FRAME_TYPE && obj_num >= 1)                                                                                       goto bad_frame;
  else if (frame_type == MENU_FRAME_TYPE && obj_num > 1)                                                                                          goto bad_frame;

  //all tests passed

  Serial.println("request frame passed checks");
  return true;

  //  some test failed, return false
bad_frame:
  Serial.println("request frame failed checks");
  return false;

}


bool Coms::sanity_check_ping_rx(byte *buf) {

  /*
     good ping frame should be

     8  : 00001000
     6  : 00000110
     34 : 00100010
     0  : 00000000  <- obj num is zero
     1  : 00000001
     45 : 00101101  <- vertical parity
     94 : 01011110  <- checksum
     255: 11111111  <- end byte
  */

  /*
     bad ping frame should be

     8  : 00001000
     6  : 00000110
     34 : 00100010
     0  : 00000000
     0  : 00000000
     44 : 00101100  <- vertical parity
     92 : 01011100  <- checksum
     255: 11111111  <- end byte
  */


  byte frame_length = buf[0];
  byte frame_type = buf[1];
  byte num_frames = (buf[2] >> 5) & 0x7;
  byte this_frame = (buf[2] >> 1) & 7;
  byte obj_num = buf[3];
  byte ping_response = buf[4];

  byte vert_parity = buf[5];
  byte checksum = buf[6];
  byte endbyte = buf[7];

  if (ping_response != PING_GOOD_RESPONSE) {  //if neither good or bad frame
    if (ping_response != PING_BAD_RESPONSE)                 goto bad_ping_response;
  }
  else if (frame_length != PING_FRAME_RESPONSE_LENGTH)      goto bad_ping_response;
  else if (frame_type != PING_RESPONSE_TYPE)                goto bad_ping_response;
  else if (num_frames != 1)                                 goto bad_ping_response;
  else if (this_frame != 1)                                 goto bad_ping_response;
  else if (obj_num != 0)                                    goto bad_ping_response;
  else if (vert_parity != 45)                               goto bad_ping_response;
  else if (checksum != 94)                                  goto bad_ping_response;

  else {
    Serial.println("ping received ok");
    return 0;
  }



bad_ping_response:
  Serial.println("bad ping received");
  return 1;
}

byte Coms::baud_LUT(uint32_t coms_speed) {

  byte i = 1; //scan through index's of array until matching speed or end of array
  while (i < NUM_COMS_SPEEDS) {
    if (baud_rate_LUT[i] == coms_speed) return i;
    i++;
  }
  if (i < NUM_COMS_SPEEDS) return 0; //index zero is default coms speed, so just stay at that if speed not defined
}



//void Coms::fill_frame_row(byte *dest, byte *src, int row, int num_bytes, int start_loc) {
//
//  for (byte i = start_loc; i < num_bytes; i++) {
//
//    dest[row][i] = src[i];
//
//  }
//
//}

#endif // Coms_CPP
