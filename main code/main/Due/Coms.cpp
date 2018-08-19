
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
// variables

//create these structs
struct Frame text_frame;
struct Frame menu_frame;
struct Frame pos_frame;
struct Frame sensor_data_frame;
struct Frame ping_frame;

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

  uint16_t frame_offset = ((frame_num - 1) * (FRAME_DATA_LENGTH - 1)); //if this frame is 1 offset in data set is 0, if 2 offset 26, etc

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

inline void Coms::pack_xy_coordinates(byte obj_num) {
  //function to pack the 4 bytes to send the x and y positions of the text cursor
  pos_frame.frame_buffer[4] = ((text_cursor[obj_num].x >> 8) & 0xFF);   //write new values to frame
  pos_frame.frame_buffer[5] = (text_cursor[obj_num].x & 0xFF);
  pos_frame.frame_buffer[6] = ((text_cursor[obj_num].y >> 8) & 0xFF);
  pos_frame.frame_buffer[7] = (text_cursor[obj_num].y & 0xFF);

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

  pos_frame.frame_buffer[0] = (pos_frame.frame_buffer[0] << 1) | (parity_of(pos_frame.frame_buffer[0]));
  pos_frame.frame_buffer[1] = (pos_frame.frame_buffer[1] << 1) | (parity_of(pos_frame.frame_buffer[1]));
  pos_frame.frame_buffer[2] = (pos_frame.frame_buffer[2]       | (parity_of(GET_FRAME_NUM_DATA(pos_frame.frame_buffer[2])) << 4));



#endif

}


void Coms::build_menu_data_frame(byte menu_number) {   //function to build the frame to send menu info

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
    CLEAR_HEADER_CHECKSUM(text_frame.frame_buffer[3]);  //clear these bits from prior frame
    set_buffer_parity_bits(text_frame.frame_buffer, BYTE_PARITY_LOC , text_frame.frame_length - TRAILER_LENGTH, HEADER_LENGTH);
    set_verical_parity_byte(text_frame.frame_buffer , text_frame.frame_length - 3);
    set_checksum_11(generate_checksum_11(frame_type), frame_type); //macro to generate 11 bit checksum
  }

  else if (frame_type == POS_FRAME_TYPE) {

    CLEAR_HEADER_CHECKSUM(pos_frame.frame_buffer[3]);
    set_verical_parity_byte(pos_frame.frame_buffer , pos_frame.frame_length - 3);
    set_checksum_11(generate_checksum_11(frame_type), frame_type);
  }

  else if (frame_type == SENSOR_FRAME_TYPE) {

    CLEAR_HEADER_CHECKSUM(sensor_data_frame.frame_buffer[3]);
    set_verical_parity_byte(sensor_data_frame.frame_buffer, sensor_data_frame.frame_length - 3);
    set_checksum_11(generate_checksum_11(frame_type), frame_type);
  }

  else if (frame_type == MENU_FRAME_TYPE) {

    CLEAR_HEADER_CHECKSUM(menu_frame.frame_buffer[3]);
    set_verical_parity_byte(menu_frame.frame_buffer , menu_frame.frame_length - 3);
    set_checksum_11(generate_checksum_11(frame_type), frame_type);
  }

  else if (frame_type == PING_STRING_TYPE) {

    CLEAR_HEADER_CHECKSUM(ping_frame.frame_buffer[3]);
    set_verical_parity_byte(ping_frame.frame_buffer , ping_frame.frame_length - 3);
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
      ping_frame.frame_buffer[3] = (ping_frame.frame_buffer[3]       | (parity_of(ping_frame.frame_buffer[3])));
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
  byte disp_from_frame_end = 2;
  switch (frame_type) {
    case TEXT_FRAME_TYPE:
      text_frame.frame_buffer[CHECKSUM_3_BIT_LOC] = (text_frame.frame_buffer[CHECKSUM_3_BIT_LOC] | (three_bit));
      text_frame.frame_buffer[text_frame.frame_length - disp_from_frame_end] = eight_bit;
      break;
    case POS_FRAME_TYPE:
      pos_frame.frame_buffer[CHECKSUM_3_BIT_LOC] = (pos_frame.frame_buffer[CHECKSUM_3_BIT_LOC] | (three_bit));
      pos_frame.frame_buffer[pos_frame.frame_length - disp_from_frame_end] = eight_bit;
      break;
    case SENSOR_FRAME_TYPE:
      sensor_data_frame.frame_buffer[CHECKSUM_3_BIT_LOC] = (sensor_data_frame.frame_buffer[CHECKSUM_3_BIT_LOC] | (three_bit));
      sensor_data_frame.frame_buffer[sensor_data_frame.frame_length - disp_from_frame_end] = eight_bit;
      break;
    case MENU_FRAME_TYPE:
      menu_frame.frame_buffer[CHECKSUM_3_BIT_LOC] = (menu_frame.frame_buffer[CHECKSUM_3_BIT_LOC] | (three_bit));
      menu_frame.frame_buffer[menu_frame.frame_length - disp_from_frame_end] = eight_bit;
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



  switch (frame_type) {
    case TEXT_FRAME_TYPE:
      text_frame_history[address].history_index = (text_frame_history[address].history_index + 1) % FRAME_HISTORY_MEMORY_DEPTH;     //increment index
      memcpy(text_frame_history[address].frame_content[text_frame_history[address].history_index], buf, buf[1]);                    //copy frame buffer into ring buffer fro storage
      text_frame_history[address].num_populated_buffers++;                                                                      //increment number of frames sent
      break;

    case POS_FRAME_TYPE:
      pos_frame_history[address].history_index = (pos_frame_history[address].history_index + 1) % FRAME_HISTORY_MEMORY_DEPTH;
      memcpy(pos_frame_history[address].frame_content[pos_frame_history[address].history_index], buf, buf[1]);
      pos_frame_history[address].num_populated_buffers++;
      break;

    case SENSOR_FRAME_TYPE:
      sensor_data_frame_history[address].history_index = (sensor_data_frame_history[address].history_index + 1) % FRAME_HISTORY_MEMORY_DEPTH;
      memcpy(sensor_data_frame_history[address].frame_content[sensor_data_frame_history[address].history_index], buf, buf[1]);
      sensor_data_frame_history[address].num_populated_buffers++;
      break;

    case MENU_FRAME_TYPE:
      menu_frame_history[address].history_index = (menu_frame_history[address].history_index + 1) % FRAME_HISTORY_MEMORY_DEPTH;
      memcpy(menu_frame_history[address].frame_content[menu_frame_history[address].history_index], buf, buf[1]);
      menu_frame_history[address].num_populated_buffers++;
      break;
  }

}


byte Coms::find_in_frame_history(byte address, byte frame_type, byte frame_num, byte obj_num) {

  bool frame_num_found = false;
  bool obj_num_found = false;

  byte history_index = 0;
  switch (frame_type) {
    case TEXT_FRAME_TYPE:   history_index = text_frame_history[address].history_index;          break;
    case POS_FRAME_TYPE:    history_index = pos_frame_history[address].history_index;           break;
    case SENSOR_FRAME_TYPE: history_index = sensor_data_frame_history[address].history_index;   break;
    case MENU_FRAME_TYPE:   history_index = menu_frame_history[address].history_index;          break;
  }


  for (int i = history_index; i != history_index + 1; i--) {
    if (i == -1) i = FRAME_HISTORY_MEMORY_DEPTH - 1; // loop back index

    switch (frame_type) {
      case TEXT_FRAME_TYPE:
        if (EXTRACT_THIS_FRAME_DATA(text_frame_history[address].frame_content[i][2]) == frame_num) frame_num_found = true;
        if (EXTRACT_OBJ_NUM_DATA(text_frame_history[address].frame_content[i][3]) == obj_num)      obj_num_found = true;
        break;
      case POS_FRAME_TYPE:
        if (EXTRACT_THIS_FRAME_DATA(pos_frame_history[address].frame_content[i][2]) == frame_num) frame_num_found = true;
        if (EXTRACT_OBJ_NUM_DATA(pos_frame_history[address].frame_content[i][3]) == obj_num)      obj_num_found = true;
        break;
      case SENSOR_FRAME_TYPE:
        if (EXTRACT_THIS_FRAME_DATA(sensor_data_frame_history[address].frame_content[i][2]) == frame_num) frame_num_found = true;
        if (EXTRACT_OBJ_NUM_DATA(sensor_data_frame_history[address].frame_content[i][3]) == obj_num)      obj_num_found = true;
        break;
      case MENU_FRAME_TYPE:
        if (EXTRACT_THIS_FRAME_DATA(menu_frame_history[address].frame_content[i][2]) == frame_num) frame_num_found = true;
        if (EXTRACT_OBJ_NUM_DATA(menu_frame_history[address].frame_content[i][3]) == obj_num)      obj_num_found = true;
        break;
    }

    if (frame_num_found && obj_num_found) return (i);

    frame_num_found = false;
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




#endif // Coms_CPP
