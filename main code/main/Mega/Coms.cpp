
#ifndef COMS_CPP
#define COMS_CPP

#include "Coms.h"
#include "Arduino.h"
#include "Menus.h"
#include "Local_Config.h"
#include "Graphics.h"

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
//
//int Coms::init_software_serial_to_usb_port() {          // init the serial at 115200 baud rate
//
//  Serial.begin(COMS_SPEED);
//  int alpha = millis();
//  while (!Serial) {
//    if (millis() > alpha + WAIT_TIME_FOR_USB_PORT_CONNECTION) {  //after 5 seconds elapsed, assume serial failed to initialise
//      //      debug = false;
//      return -1;
//    }
//  }
//
//  return 0;
//}
//
//int Coms::init_software_serial_to_usb_port(int speed) { // init the serial at a custom speed
//
//  if (speed != 300 && speed != 600 && speed != 1200 && speed != 2400 && speed != 4800 && speed != 14400 && speed != 9600 && speed != 14400 && speed != 19200 && speed != 28800 && speed != 38400 && speed != 57600 && speed != 115200)
//    return (-2);
//
//  Serial.begin(speed);
//  int alpha = millis();
//  while (!Serial) {
//    if (millis() > alpha + WAIT_TIME_FOR_USB_PORT_CONNECTION) {  //after 5 seconds elapsed, assume serial failed to initialise
//      //      debug = false;
//      return -1;
//    }
//  }
//
//  return 0;
//}



void Coms::extract_sensor_data(byte *temp_buffer) {

  for (byte alpha = HEADER_LENGTH; alpha < temp_buffer[0] - TRAILER_LENGTH - 2; alpha += 2) { //step through frame, identify prefix and extract following data byte

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
        break;

      case PREFIX_TEXT_SIZE_0:
        text_parameters[0].text_size = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_SIZE_1:
        text_parameters[1].text_size = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_SIZE_2:
        text_parameters[2].text_size = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_SIZE_3:
        text_parameters[3].text_size = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_SIZE_4:
        text_parameters[4].text_size = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_R_0:
        text_parameters[0].colour_r = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_R_1:
        text_parameters[1].colour_r = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_R_2:
        text_parameters[2].colour_r = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_R_3:
        text_parameters[3].colour_r = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_R_4:
        text_parameters[4].colour_r = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_G_0:
        text_parameters[0].colour_g = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_G_1:
        text_parameters[1].colour_g = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_G_2:
        text_parameters[2].colour_g = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_G_3:
        text_parameters[3].colour_g = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_G_4:
        text_parameters[4].colour_g = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_B_0:
        text_parameters[0].colour_b = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_B_1:
        text_parameters[1].colour_b = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_B_2:
        text_parameters[2].colour_b = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_B_3:
        text_parameters[3].colour_b = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_B_4:
        text_parameters[4].colour_b = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_HUE_MSB_0:
        text_parameters[0].hue = set_hue_colour(temp_buffer[alpha + 1], 1); //function to set the value of hue based on the msb or lsb received
        break;

      case PREFIX_TEXT_HUE_LSB_0:
        text_parameters[0].hue = set_hue_colour(temp_buffer[alpha + 1], 2);
        break;

      case PREFIX_TEXT_HUE_MSB_1:
        text_parameters[1].hue = set_hue_colour(temp_buffer[alpha + 1], 1); //function to set the value of hue based on the msb or lsb received
        break;

      case PREFIX_TEXT_HUE_LSB_1:
        text_parameters[1].hue = set_hue_colour(temp_buffer[alpha + 1], 2);
        break;

      case PREFIX_TEXT_HUE_MSB_2:
        text_parameters[2].hue = set_hue_colour(temp_buffer[alpha + 1], 1); //function to set the value of hue based on the msb or lsb received
        break;

      case PREFIX_TEXT_HUE_LSB_2:
        text_parameters[2].hue = set_hue_colour(temp_buffer[alpha + 1], 2);
        break;

      case PREFIX_TEXT_HUE_MSB_3:
        text_parameters[3].hue = set_hue_colour(temp_buffer[alpha + 1], 1); //function to set the value of hue based on the msb or lsb received
        break;

      case PREFIX_TEXT_HUE_LSB_3:
        text_parameters[3].hue = set_hue_colour(temp_buffer[alpha + 1], 2);
        break;

      case PREFIX_TEXT_HUE_MSB_4:
        text_parameters[4].hue = set_hue_colour(temp_buffer[alpha + 1], 1); //function to set the value of hue based on the msb or lsb received
        break;

      case PREFIX_TEXT_HUE_LSB_4:
        text_parameters[4].hue = set_hue_colour(temp_buffer[alpha + 1], 2);
        break;

      case PREFIX_TEXT_USE_HUE_0:
        text_parameters[0].use_hue = (temp_buffer[alpha + 1] == 1) ? true : false;
        break;

      case PREFIX_TEXT_USE_HUE_1:
        text_parameters[1].use_hue = (temp_buffer[alpha + 1] == 1) ? true : false;
        break;

      case PREFIX_TEXT_USE_HUE_2:
        text_parameters[2].use_hue = (temp_buffer[alpha + 1] == 1) ? true : false;
        break;

      case PREFIX_TEXT_USE_HUE_3:
        text_parameters[3].use_hue = (temp_buffer[alpha + 1] == 1) ? true : false;
        break;

      case PREFIX_TEXT_USE_HUE_4:
        text_parameters[4].use_hue = (temp_buffer[alpha + 1] == 1) ? true : false;
        break;

      case PREFIX_SCREEN_MODE:
        screen_parameters.mode = temp_buffer[alpha + 1];
        break;

      case PREFIX_SD_MOUNTED_1:
        menu_parameters.sd_card1_detected = temp_buffer[alpha + 1];
        break;

      case PREFIX_SD_MOUNTED_2:
        menu_parameters.sd_card2_detected = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_SCROLL_SPEED_X_0:
        cursor_parameters[0].x_dir = GET_TEXT_DIR(temp_buffer[alpha + 1]);
        break;

      case PREFIX_TEXT_SCROLL_SPEED_Y_0:
        cursor_parameters[0].y_dir = GET_TEXT_DIR(temp_buffer[alpha + 1]);
        break;

      case PREFIX_TEXT_SCROLL_SPEED_X_1:
        cursor_parameters[1].x_dir = GET_TEXT_DIR(temp_buffer[alpha + 1]);
        break;

      case PREFIX_TEXT_SCROLL_SPEED_Y_1:
        cursor_parameters[1].y_dir = GET_TEXT_DIR(temp_buffer[alpha + 1]);
        break;

      case PREFIX_TEXT_SCROLL_SPEED_X_2:
        cursor_parameters[2].x_dir = GET_TEXT_DIR(temp_buffer[alpha + 1]);
        break;

      case PREFIX_TEXT_SCROLL_SPEED_Y_2:
        cursor_parameters[2].y_dir = GET_TEXT_DIR(temp_buffer[alpha + 1]);
        break;

      case PREFIX_TEXT_SCROLL_SPEED_X_3:
        cursor_parameters[3].x_dir = GET_TEXT_DIR(temp_buffer[alpha + 1]);
        break;

      case PREFIX_TEXT_SCROLL_SPEED_Y_3:
        cursor_parameters[3].y_dir = GET_TEXT_DIR(temp_buffer[alpha + 1]);
        break;

      case PREFIX_TEXT_SCROLL_SPEED_X_4:
        cursor_parameters[4].x_dir = GET_TEXT_DIR(temp_buffer[alpha + 1]);
        break;

      case PREFIX_TEXT_SCROLL_SPEED_Y_4:
        cursor_parameters[4].y_dir = GET_TEXT_DIR(temp_buffer[alpha + 1]);
        break;

      case PREFIX_TEXT_OBJ_ENABLED_0:
        text_parameters[3].object_used = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_OBJ_ENABLED_1:
        text_parameters[3].object_used = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_OBJ_ENABLED_2:
        text_parameters[3].object_used = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_OBJ_ENABLED_3:
        text_parameters[3].object_used = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_OBJ_ENABLED_4:
        text_parameters[3].object_used = temp_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_OBJ_SELECTED:
        menu_parameters.obj_selected = temp_buffer[alpha + 1];
        break;


      default:  Sprint("Error: recieved prefix not defined. Prefix :");
        Sprintln((byte)temp_buffer[alpha + 1]);


    }
  }
}
//
//void Coms::extract_pos_frame(byte obj_num) {
//  pos_frame.frame_arrived = false;
//  cursor_parameters[obj_num].global_x_pos = calc_pos(pos_frame.frame_buffer[4], pos_frame.frame_buffer[5]);
//  cursor_parameters[obj_num].global_y_pos = calc_pos(pos_frame.frame_buffer[6], pos_frame.frame_buffer[7]);
//
//
//  cursor_parameters[obj_num].local_x_pos = calc_local_pos(pos_frame.frame_buffer[4], pos_frame.frame_buffer[5]);
//  cursor_parameters[obj_num].local_y_pos = calc_local_pos(pos_frame.frame_buffer[6], pos_frame.frame_buffer[7]);
//  //  calc_local_pos();
//
//  cursor_parameters[obj_num].x_dir = pos_frame.frame_buffer[8] - 128;
//  cursor_parameters[obj_num].y_dir = pos_frame.frame_buffer[9] - 128;
//}

void Coms::extract_menu_frame() {
  menu_frame.frame_arrived = false;
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
#error ("heavy error checking not implemented")

#else //simple 8 bit checksum of frame

  byte frame_length = temp_buffer[FRAME_LENGTH_LOC];
  byte sum = 0;
  for (byte i = 0; i < frame_length - TRAILER_LENGTH; i++) { //sum elements from checksum
    sum += temp_buffer[i];
  }
  if (sum == temp_buffer[frame_length - 2])  //outer checksum, only checksum if DO_HEAVY_ERROR_CHECKING not defined
    return true;
  else
    return false;

#endif
#endif

}


void Coms::frame_cpy(byte *temp_buffer, byte frame_type) {

  byte obj_num = temp_buffer[3];
  byte data_end_index = text_frame.frame_length - TRAILER_LENGTH;
  byte this_frame = APPLY_THIS_FRAME_MASK(temp_buffer[FRAME_NUMBER_LOC]);
  
  switch (frame_type) {
    case TEXT_FRAME_TYPE:
      memcpy(text_parameters[obj_num].string[this_frame * FRAME_DATA_LENGTH], temp_buffer[HEADER_LENGTH + 1], data_end_index - (HEADER_LENGTH - 1));
      break;
      
    case POS_FRAME_TYPE:
      cursor_parameters[obj_num].global_x_pos = GET_GLOBAL_POS(temp_buffer[4],temp_buffer[5]);   // position as recieved in frame
      cursor_parameters[obj_num].global_y_pos = GET_GLOBAL_POS(temp_buffer[6],temp_buffer[7]);   
     
      cursor_parameters[obj_num].local_x_pos = (screen_parameters.node_address)*(SINGLE_MATRIX_WIDTH) - cursor_parameters[obj_num].global_x_pos;     // relative position for this matrix
      cursor_parameters[obj_num].local_y_pos = cursor_parameters[obj_num].global_y_pos;
      
      cursor_parameters[obj_num].x_dir = GET_TEXT_DIR(temp_buffer[8]);
      cursor_parameters[obj_num].y_dir = GET_TEXT_DIR(temp_buffer[9]);            
      break;
      
    case SENSOR_FRAME_TYPE:
      extract_sensor_data(temp_buffer); //extract data using massive switch 
      break;
      
    case MENU_FRAME_TYPE:
      menu.set_current_menu(temp_buffer[3]);
      menu_parameters.encoder_position = GET_ENCODER_POS(temp_buffer[4],temp_buffer[5]);
      break;
      
  }

}

#endif // COMS_CPP
