
#ifndef COMS_CPP
#define COMS_CPP

#include "Coms.h"
#include "Arduino.h"
#include "Menus.h"
#include "Local_Config.h"
#include "Graphics.h"
#include "LUTS.h"
#include "Host.h"


Frame text_frame;
Frame menu_frame;
Frame pos_frame;
Frame sensor_data_frame;
Frame ping_frame;
extern struct Menu_Struct menu_parameters;
extern struct Cursor_Struct cursor_parameters[MAX_NUM_OF_TEXT_OBJECTS];
extern struct Screen_Struct screen_parameters;
extern struct Text_Struct text_parameters[MAX_NUM_OF_TEXT_OBJECTS];


extern Menu menu;
extern Host host;
extern Graphics graphics;


void Coms::extract_sensor_data(byte *temp_buffer) {

  byte frame_length = APPLY_FRAME_LENGTH_MASK(temp_buffer[0]);
  bool screen_already_not_updated = false;
  if (screen_parameters.updated == false) { //if the screen is already not updated, check if this loop changes that, if so reset update backoff timer
    screen_already_not_updated = true;
    screen_parameters.updated = true;
  }

  for (byte alpha = HEADER_LENGTH; alpha < frame_length - TRAILER_LENGTH - 1; alpha += 2) { //step through frame, identify prefix and extract following data byte

    Serial.print("sensor_data pair: ");
    Serial.print(temp_buffer[alpha]);
    Serial.print("\t");
    Serial.println(temp_buffer[alpha + 1]);
    
    //set the value of a variable based on what the prefix is
    switch (temp_buffer[alpha]) {

      case PREFIX_CURRENT_1:
        menu_parameters.current1 = temp_buffer[alpha + 1];
        break;

      case PREFIX_CURRENT_2:
        menu_parameters.current2 = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEMP_1:
        menu_parameters.temp1 = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEMP_2:
        menu_parameters.temp2 = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEMP_3:
        menu_parameters.temp3 = temp_buffer[alpha + 1];
        break;

      case PREFIX_LDR_1:
        menu_parameters.light1 = temp_buffer[alpha + 1];
        break;

      case PREFIX_LDR_2:
        menu_parameters.light2 = temp_buffer[alpha + 1];
        break;

      case PREFIX_FAN_SPEED:
        menu_parameters.fan_speed = temp_buffer[alpha + 1];
        break;

      case PREFIX_FAN_MINIMUM_SPEED:
        menu_parameters.min_fan_speed = temp_buffer[alpha + 1];
        break;

      case PREFIX_FAN_ENABLED:
        menu_parameters.fan_enabled = temp_buffer[alpha + 1];
        break;

      case PREFIX_LED_STRIP_BRIGHTNESS:
        menu_parameters.led_strip_brightness = temp_buffer[alpha + 1];
        break;

      case PREFIX_LED_STRIP_ENABLED:
        menu_parameters.led_strip_enabled = temp_buffer[alpha + 1];
        break;

      case PREFIX_SD1_DETECTED:
        menu_parameters.sd_card1_detected = (temp_buffer[alpha + 1] == 1) ? true : false; //convert byte to boolean
        break;

      case PREFIX_SD2_DETECTED:
        menu_parameters.sd_card2_detected = (temp_buffer[alpha + 1] == 1) ? true : false;
        break;

      case PREFIX_EHTERNET_CONNECTED:
        menu_parameters.ethernet_connected = (temp_buffer[alpha + 1] == 1) ? true : false;
        break;

      case PREFIX_WIFI_CONNECTED:
        menu_parameters.wifi_connected = (temp_buffer[alpha + 1] == 1) ? true : false;
        break;

      case PREFIX_SCREEN_BRIGHTNESS:
        screen_parameters.brightness = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_SIZE_0:
        text_parameters[0].text_size = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_SIZE_1:
        text_parameters[1].text_size = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_SIZE_2:
        text_parameters[2].text_size = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_SIZE_3:
        text_parameters[3].text_size = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_SIZE_4:
        text_parameters[4].text_size = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_COLOUR_R_0:
        text_parameters[0].colour_r = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_COLOUR_R_1:
        text_parameters[1].colour_r = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_COLOUR_R_2:
        text_parameters[2].colour_r = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_COLOUR_R_3:
        text_parameters[3].colour_r = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_COLOUR_R_4:
        text_parameters[4].colour_r = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_COLOUR_G_0:
        text_parameters[0].colour_g = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_COLOUR_G_1:
        text_parameters[1].colour_g = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_COLOUR_G_2:
        text_parameters[2].colour_g = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_COLOUR_G_3:
        text_parameters[3].colour_g = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_COLOUR_G_4:
        text_parameters[4].colour_g = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_COLOUR_B_0:
        text_parameters[0].colour_b = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_COLOUR_B_1:
        text_parameters[1].colour_b = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_COLOUR_B_2:
        text_parameters[2].colour_b = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_COLOUR_B_3:
        text_parameters[3].colour_b = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_COLOUR_B_4:
        text_parameters[4].colour_b = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_HUE_MSB_0:
        set_hue_colour(temp_buffer[alpha + 1], 0, 1);
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_HUE_LSB_0:
        set_hue_colour(temp_buffer[alpha + 1], 0, 2);
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_HUE_MSB_1:
        set_hue_colour(temp_buffer[alpha + 1], 1, 1);
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_HUE_LSB_1:
        set_hue_colour(temp_buffer[alpha + 1], 1, 2);
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_HUE_MSB_2:
        set_hue_colour(temp_buffer[alpha + 1], 2, 1);
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_HUE_LSB_2:
        set_hue_colour(temp_buffer[alpha + 1], 2, 2);
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_HUE_MSB_3:
        set_hue_colour(temp_buffer[alpha + 1], 3, 1);
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_HUE_LSB_3:
        set_hue_colour(temp_buffer[alpha + 1], 3, 2);
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_HUE_MSB_4:
        set_hue_colour(temp_buffer[alpha + 1], 4, 1);
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_HUE_LSB_4:
        set_hue_colour(temp_buffer[alpha + 1], 4, 2);
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_USE_HUE_0:
        text_parameters[0].use_hue = (temp_buffer[alpha + 1] == 1) ? true : false;
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_USE_HUE_1:
        text_parameters[1].use_hue = (temp_buffer[alpha + 1] == 1) ? true : false;
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_USE_HUE_2:
        text_parameters[2].use_hue = (temp_buffer[alpha + 1] == 1) ? true : false;
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_USE_HUE_3:
        text_parameters[3].use_hue = (temp_buffer[alpha + 1] == 1) ? true : false;
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_USE_HUE_4:
        text_parameters[4].use_hue = (temp_buffer[alpha + 1] == 1) ? true : false;
        screen_parameters.updated = false;
        break;

      case PREFIX_SCREEN_MODE:
        screen_parameters.new_mode = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_SD_MOUNTED_1:
        menu_parameters.sd_card1_detected = temp_buffer[alpha + 1];
        break;

      case PREFIX_SD_MOUNTED_2:
        menu_parameters.sd_card2_detected = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_SCROLL_SPEED_X_0:
        cursor_parameters[0].x_dir = GET_TEXT_DIR(temp_buffer[alpha + 1]);
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_SCROLL_SPEED_Y_0:
        cursor_parameters[0].y_dir = GET_TEXT_DIR(temp_buffer[alpha + 1]);
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_SCROLL_SPEED_X_1:
        cursor_parameters[1].x_dir = GET_TEXT_DIR(temp_buffer[alpha + 1]);
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_SCROLL_SPEED_Y_1:
        cursor_parameters[1].y_dir = GET_TEXT_DIR(temp_buffer[alpha + 1]);
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_SCROLL_SPEED_X_2:
        cursor_parameters[2].x_dir = GET_TEXT_DIR(temp_buffer[alpha + 1]);
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_SCROLL_SPEED_Y_2:
        cursor_parameters[2].y_dir = GET_TEXT_DIR(temp_buffer[alpha + 1]);
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_SCROLL_SPEED_X_3:
        cursor_parameters[3].x_dir = GET_TEXT_DIR(temp_buffer[alpha + 1]);
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_SCROLL_SPEED_Y_3:
        cursor_parameters[3].y_dir = GET_TEXT_DIR(temp_buffer[alpha + 1]);
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_SCROLL_SPEED_X_4:
        cursor_parameters[4].x_dir = GET_TEXT_DIR(temp_buffer[alpha + 1]);
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_SCROLL_SPEED_Y_4:
        cursor_parameters[4].y_dir = GET_TEXT_DIR(temp_buffer[alpha + 1]);
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_OBJ_ENABLED_0:
        text_parameters[0].object_used = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_OBJ_ENABLED_1:
        text_parameters[1].object_used = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_OBJ_ENABLED_2:
        text_parameters[2].object_used = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_OBJ_ENABLED_3:
        text_parameters[3].object_used = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_OBJ_ENABLED_4:
        text_parameters[4].object_used = temp_buffer[alpha + 1];
        screen_parameters.updated = false;
        break;

      case PREFIX_TEXT_OBJ_SELECTED:
        menu_parameters.obj_selected = temp_buffer[alpha + 1];
        break;


      default:  Sprint("Error: recieved prefix not defined. Prefix :");
        Sprintln((byte)temp_buffer[alpha + 1]);


    }
  }

  if (!screen_parameters.updated) { // this function changed this, reset timer
    screen_parameters.time_last_updated = millis();
  }

  else if (screen_already_not_updated)  //function didnt change it but it was already waiting for timer
    screen_parameters.updated = false;

}


inline int Coms::calc_pos(byte MSB, byte LSB) {
  return (MSB << 8) + LSB;
}

inline int Coms::calc_local_pos(byte MSB, byte LSB) {
  int temp = (MSB << 8) + LSB;
  return  temp - (screen_parameters.node_address << SINGLE_MATRIX_WIDTH_AS_POW_2);
}


bool Coms::validate_checksum(byte *temp_buffer) {
#define DO_ERROR_CHECKING //calculate checksum


#ifdef DO_ERROR_CHECKING
#ifdef DO_HEAVY_ERROR_CHECKING //parity check each byte and 13 bit checksum of entire frame
  //#error ("heavy error checking not implemented")

#else //simple 8 bit checksum of frame

  byte frame_length = temp_buffer[FRAME_LENGTH_LOC];
  uint16_t sum = 0;
  for (byte i = 0; i < frame_length - TRAILER_LENGTH; i++) { //sum elements from checksum
    sum += temp_buffer[i];
  }

  if (sum == temp_buffer[frame_length - 2])  //outer checksum, or only checksum if DO_HEAVY_ERROR_CHECKING not defined
    return true;
  else
    return false;

#endif
#endif

}


void Coms::frame_cpy(byte *temp_buffer, byte frame_type) {

  byte obj_num = APPLY_OBJ_NUM_MASK(temp_buffer[OBJ_NUM_LOC]);
  byte num_frames = APPLY_FRAME_NUM_MASK(temp_buffer[NUM_OF_FRAMES_LOC]);
  byte this_frame = APPLY_THIS_FRAME_MASK(temp_buffer[FRAME_NUMBER_LOC]);
  byte frame_length = APPLY_FRAME_LENGTH_MASK(temp_buffer[FRAME_LENGTH_LOC]);
  uint16_t disp_string_offset = FRAME_DATA_LENGTH * (this_frame - 1);

  byte num_bytes_to_cpy = frame_length - FRAME_OVERHEAD;
  int8_t temp1 = 0;
  int8_t temp2 = 0;
  uint32_t new_x_period = 10000;
  uint32_t new_y_period = 10000;

  switch (frame_type) {
    case TEXT_FRAME_TYPE:

      memcpy(text_parameters[obj_num].string + disp_string_offset, temp_buffer + HEADER_LENGTH, num_bytes_to_cpy);

      if (num_frames == this_frame) { //if this is the last frame, overwrite any remaining string elements with nulls, ensure the previous strings dont persist
        for (byte i = disp_string_offset + num_bytes_to_cpy; i < MAX_TWEET_SIZE; i++)
          text_parameters[obj_num].string[i] = 0;
      }

      screen_parameters.updated = false;  //part of a string arrived, update displayed buffer, update as each part of string arrives, better than not detecting retransmission
      screen_parameters.time_last_updated = millis(); //set timer, waits for a time out before updating screen to ensure all frames arrived
      //
      //      Serial.print(F("Stored string "));
      //      Serial.print(obj_num);
      //      Serial.print(F(" = "));
      //      for (byte i = 0; i < MAX_TWEET_SIZE; i++) {
      //        Serial.print((char)text_parameters[obj_num].string[i]);
      //      }
      //
      //      Serial.println();
      break;

    case POS_FRAME_TYPE:

      // see Coms::pack_xy_coordinates in due code for description of pos value packing

      // decode negative values
      if (temp_buffer[4] > 127) {
        temp1 = temp_buffer[4] & 0x7F;
        temp1 = -temp1;
      }
      else
        temp1 = temp_buffer[4];

      if (temp_buffer[6] > 127) {
        temp2 = temp_buffer[6] & 0x7F;
        temp2 = -temp2;
      }
      else
        temp2 = temp_buffer[6];


      //  host.print_bits(temp_buffer[4], 8, BIN);
      //  Serial.print(" ");
      //  host.println_bits(temp_buffer[5], 8, BIN);
      //
      //
      //  host.print_bits(temp1, 8, BIN);
      //  Serial.print(" ");
      //  host.println_bits(temp_buffer[5], 8, BIN);


      cursor_parameters[obj_num].global_x_pos = GET_GLOBAL_POS(temp1, temp_buffer[5]);  // position as recieved in frame
      cursor_parameters[obj_num].global_y_pos = GET_GLOBAL_POS(temp2, temp_buffer[7]);
//
//      Serial.print("global x = ");
//      Serial.println(cursor_parameters[obj_num].global_x_pos);
//
//      Serial.print("global y = ");
//      Serial.println(cursor_parameters[obj_num].global_y_pos);

      cursor_parameters[obj_num].local_x_pos = cursor_parameters[obj_num].global_x_pos - (screen_parameters.node_address * SINGLE_MATRIX_WIDTH);  // relative position for this matrix
      cursor_parameters[obj_num].local_y_pos = cursor_parameters[obj_num].global_y_pos;

      cursor_parameters[obj_num].x_dir = GET_TEXT_DIR(temp_buffer[8]);
      cursor_parameters[obj_num].y_dir = GET_TEXT_DIR(temp_buffer[9]);

      if (cursor_parameters[obj_num].x_dir != 0)
        new_x_period = (float)(1000 / ((float)(abs(cursor_parameters[obj_num].x_dir)) * XY_SPEED_UNITS)); //1000ms/(x_dir*XY_SPEED_UNITS)

      if (cursor_parameters[obj_num].y_dir != 0)
        new_y_period = (float)(1000 / ((float)(abs(cursor_parameters[obj_num].y_dir)) * XY_SPEED_UNITS));

      graphics.update_pos_isr_period(obj_num, new_x_period, new_y_period);


      //      Serial.print("time between increments: ");
      //      Serial.print(cursor_parameters[obj_num].time_between_increments_x);
      //      Serial.print(", ");
      //      Serial.println(cursor_parameters[obj_num].time_between_increments_y);


      screen_parameters.updated = false;  //pos frame requires screen update always
      screen_parameters.time_last_updated = millis();//for backoff time calculation
      break;

    case SENSOR_FRAME_TYPE:
      extract_sensor_data(temp_buffer); //extract data using massive switch, conditionally update screen depending on specific data
      Serial.println(F("sensor_frame_extracted"));
      break;

    case MENU_FRAME_TYPE:

      temp2 = menu.get_previous_menu(); //to check if we changed menu

      //temp_buffer[3] <= selected object, needed for some menus

      menu.set_current_menu(temp_buffer[4]);

      // decode negative values
      if (temp_buffer[5] < 0) {
        temp1 = temp_buffer[5] & 0x7F;
        temp1 = -temp1;
      }
      else
        temp1 = temp_buffer[5];

      if (temp2 != menu.get_previous_menu()) //just change menu, dont run scroll animation 
      menu_parameters.last_encoder_pos = GET_ENCODER_POS(temp1, temp_buffer[6]);
      else                                    // else scrolling, log encoder pos before move
      menu_parameters.last_encoder_pos = menu_parameters.encoder_position;
      
      menu_parameters.encoder_position = GET_ENCODER_POS(temp1, temp_buffer[6]);  

      screen_parameters.updated = false;  //menu update should casue screen update, menu frame not sent to megas that cant display screen
      screen_parameters.time_last_updated = millis()+SCREEN_UPDATE_BACKOFF_PERIOD;

      Serial.print("menu : ");
      Serial.println(menu.get_current_menu());
      Serial.print("encoder_pos : ");
      Serial.println(menu_parameters.encoder_position);
      Serial.println();
      break;

  }

}
void  Coms::set_hue_colour(byte receivedData, byte obj_num, byte data_loc) { //return the value of hue

  if (data_loc == 1)//1== msb, 2 = lsb
    receivedData = receivedData << 8;
  text_parameters[obj_num].hue = text_parameters[obj_num].hue | receivedData;   //somewhat dangerous, if colour printed to screen before new colour written,
  // not urgent should fix itself on next write, could use temp variable maybe and log when even writes occur?

}

byte Coms::parity_of(byte value) {
  return pgm_read_byte_near(parity + value); //LUT of parity given up to 8 bit value
}


bool Coms::error_check_frame_body(byte *buf, byte frame_type, byte frame_length) { //if frame ok return true

  bool byte_parity_error = false;
  bool vertical_parity_error = false;
  bool checksum_error = false;

  //test all error detection algorithms
  if (frame_type == TEXT_FRAME_TYPE)
    byte_parity_error = check_byte_parity(buf, HEADER_LENGTH, frame_length - TRAILER_LENGTH); //check parity included in data byte

  vertical_parity_error = check_vertical_checksum(buf, frame_length);
  checksum_error = check_final_checksum(buf, frame_length);

  return (byte_parity_error | vertical_parity_error | checksum_error);  //if any of these failed

}


bool Coms::check_byte_parity(byte *buf , byte start_byte, byte end_byte) { //run through bytes in frame, checking parity of each byte

  byte errors = 0;  //num row with incorrect parity
  for (byte i = start_byte; i <= end_byte; i++) {
    if (parity_of(buf[i]))  //if parity odd, error found
      return true;          //return true
  }
  return false;
}


bool Coms::check_vertical_checksum(byte *buf, byte frame_length) {

  byte mask = 0x1;
  for (byte j = 0; j < 8; j++) {
    byte sum = 0;
    for (byte i = 0; i < frame_length - TRAILER_LENGTH; i++) {
      if (i != CHECKSUM_3_BIT_LOC)
        sum += ((buf[i] >> j)&mask);
      else
        sum += (((buf[i] & (~CHECKSUM_3_BIT_MASK)) >> j) & mask); //suppress any 3 bit checksum values when calculating parity
    }
    if ((sum & mask) != ((buf[frame_length - TRAILER_LENGTH] >> j)&mask)) return true; //error found, sum of bits is odd
  }
  return false;
}

bool Coms::check_final_checksum(byte *buf, byte frame_length) {

  uint16_t lower_8 = buf[frame_length - 2];
  uint16_t upper_3 = APPLY_CHECKSUM_THREE_BIT_MASK(buf[3]);
  uint16_t checksum = (upper_3 << 7) | lower_8; //expected value
  uint16_t sum = 0;                             //read value

  sum = sum_header(buf[0], buf[1], buf[2], buf[3]); //sum header ignoring bits involved in expected checksum

  for (byte i = HEADER_LENGTH; i < frame_length - 2; i++) {
    sum += buf[i];
  }
  sum = sum & 0x7FF; //mod 2^11

  if (sum != checksum) return true;
  else return false;
}


inline uint16_t Coms::sum_header(byte a, byte b, byte c, byte d) {

  uint16_t total = (a + b + c + (d & 0b11110001));
  return total;

}

void Coms::remove_byte_parity_bit(byte *buf, byte parity_loc,  byte end_address, byte start_address) {

  //need to generalise this to any parity bit location

  //  byte retain_mask = 0;
  //  switch (parity_loc) {
  //    case 0:
  //      retain_mask = 0x7F;
  //      break;
  //    case 1:
  //      retain_mask = 0x3F;
  //      break;
  //    case 2:
  //      retain_mask = 0x1F;
  //      break;
  //    case 3:
  //      retain_mask = 0xF;
  //      break;
  //    case 4:
  //      retain_mask = 0x7;
  //      break;
  //    case 5:
  //      retain_mask = 0x3;
  //      break;
  //    case 6:
  //      retain_mask = 0x1;
  //      break;
  //    case 7:
  //      retain_mask = 0x0;
  //      break;
  //  }
  //

  //  for (byte i = start_address; i < end_address; i++) {
  //    buf[i] = ((buf[i] >> 1) & ~retain_mask) & (buf[i] & retain_mask);
  //  }
  for (byte i = start_address; i < end_address; i++) {
    buf[i] = buf[i] >> 1;
  }
}


#endif // COMS_CPP
