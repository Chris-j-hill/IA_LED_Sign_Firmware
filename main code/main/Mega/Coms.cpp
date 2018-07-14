
#ifndef COMS_CPP
#define COMS_CPP

#include "Coms.h"
#include "Arduino.h"
#include "Menus.h"
#include "Local_Config.h"
#include "Graphics.h"

//Frame text_frame;   // <- do i need this
Frame menu_frame;
Frame pos_frame;
Frame sensor_data_frame;
extern struct Menu_Struct menu_parameters;
extern struct Cursor_Struct cursor_parameters[MAX_NUM_OF_TEXT_OBJECTS];
extern struct Screen_Struct screen_parameters;
extern struct Text_Struct text_parameters[MAX_NUM_OF_TEXT_OBJECTS];



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



int Coms::extract_sensor_data(byte *address) {

  //  int frameLength = sensor_data_frame.frame_buffer[0];
  byte frameLength = *address;

  for (int alpha = HEADER_LENGTH; alpha < frameLength - TRAILER_LENGTH - 2; alpha = alpha + 2) { //step through frame, identify prefix and extract following data byte

    //set the value of a variable based on what the prefix is
    switch (sensor_data_frame.frame_buffer[alpha]) {

      case PREFIX_CURRENT_1:
        menu_parameters.current1 = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_CURRENT_2:
        menu_parameters.current2 = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEMP_1:
        menu_parameters.temp1 = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEMP_2:
        menu_parameters.temp2 = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEMP_3:
        menu_parameters.temp3 = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_LDR_1:
        menu_parameters.light1 = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_LDR_2:
        menu_parameters.light2 = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_FAN_SPEED:
        menu_parameters.fan_speed = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_LED_STRIP_BRIGHTNESS:
        menu_parameters.led_strip_brightness = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_SD1_DETECTED:
        menu_parameters.sd_card1_detected = (sensor_data_frame.frame_buffer[alpha + 1] == 1) ? true : false; //convert byte to boolean
        break;

      case PREFIX_SD2_DETECTED:
        menu_parameters.sd_card2_detected = (sensor_data_frame.frame_buffer[alpha + 1] == 1) ? true : false;
        break;

      case PREFIX_EHTERNET_CONNECTED:
        menu_parameters.ethernet_connected = (sensor_data_frame.frame_buffer[alpha + 1] == 1) ? true : false;
        break;

      case PREFIX_WIFI_CONNECTED:
        menu_parameters.wifi_connected = (sensor_data_frame.frame_buffer[alpha + 1] == 1) ? true : false;
        break;

      case PREFIX_SCREEN_BRIGHTNESS:
        screen_parameters.brightness = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_SIZE_0:
        text_parameters[0].text_size = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_SIZE_1:
        text_parameters[1].text_size = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_SIZE_2:
        text_parameters[2].text_size = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_SIZE_3:
        text_parameters[3].text_size = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_SIZE_4:
        text_parameters[4].text_size = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_R_0:
        text_parameters[0].colour_r = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_R_1:
        text_parameters[1].colour_r = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_R_2:
        text_parameters[2].colour_r = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_R_3:
        text_parameters[3].colour_r = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_R_4:
        text_parameters[4].colour_r = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_G_0:
        text_parameters[0].colour_g = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_G_1:
        text_parameters[1].colour_g = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_G_2:
        text_parameters[2].colour_g = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_G_3:
        text_parameters[3].colour_g = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_G_4:
        text_parameters[4].colour_g = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_B_0:
        text_parameters[0].colour_b = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_B_1:
        text_parameters[1].colour_b = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_B_2:
        text_parameters[2].colour_b = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_B_3:
        text_parameters[3].colour_b = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_COLOUR_B_4:
        text_parameters[4].colour_b = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_HUE_MSB_0:
        text_parameters[0].hue = set_hue_colour(sensor_data_frame.frame_buffer[alpha + 1], 1); //function to set the value of hue based on the msb or lsb received
        break;

      case PREFIX_TEXT_HUE_LSB_0:
        text_parameters[0].hue = set_hue_colour(sensor_data_frame.frame_buffer[alpha + 1], 2);
        break;

      case PREFIX_TEXT_HUE_MSB_1:
        text_parameters[1].hue = set_hue_colour(sensor_data_frame.frame_buffer[alpha + 1], 1); //function to set the value of hue based on the msb or lsb received
        break;

      case PREFIX_TEXT_HUE_LSB_1:
        text_parameters[1].hue = set_hue_colour(sensor_data_frame.frame_buffer[alpha + 1], 2);
        break;

      case PREFIX_TEXT_HUE_MSB_2:
        text_parameters[2].hue = set_hue_colour(sensor_data_frame.frame_buffer[alpha + 1], 1); //function to set the value of hue based on the msb or lsb received
        break;

      case PREFIX_TEXT_HUE_LSB_2:
        text_parameters[2].hue = set_hue_colour(sensor_data_frame.frame_buffer[alpha + 1], 2);
        break;

      case PREFIX_TEXT_HUE_MSB_3:
        text_parameters[3].hue = set_hue_colour(sensor_data_frame.frame_buffer[alpha + 1], 1); //function to set the value of hue based on the msb or lsb received
        break;

      case PREFIX_TEXT_HUE_LSB_3:
        text_parameters[3].hue = set_hue_colour(sensor_data_frame.frame_buffer[alpha + 1], 2);
        break;

      case PREFIX_TEXT_HUE_MSB_4:
        text_parameters[4].hue = set_hue_colour(sensor_data_frame.frame_buffer[alpha + 1], 1); //function to set the value of hue based on the msb or lsb received
        break;

      case PREFIX_TEXT_HUE_LSB_4:
        text_parameters[4].hue = set_hue_colour(sensor_data_frame.frame_buffer[alpha + 1], 2);
        break;

      case PREFIX_TEXT_USE_HUE_0:
        text_parameters[0].use_hue = (sensor_data_frame.frame_buffer[alpha + 1] == 1) ? true : false;
        break;

      case PREFIX_TEXT_USE_HUE_1:
        text_parameters[1].use_hue = (sensor_data_frame.frame_buffer[alpha + 1] == 1) ? true : false;
        break;

      case PREFIX_TEXT_USE_HUE_2:
        text_parameters[2].use_hue = (sensor_data_frame.frame_buffer[alpha + 1] == 1) ? true : false;
        break;

      case PREFIX_TEXT_USE_HUE_3:
        text_parameters[3].use_hue = (sensor_data_frame.frame_buffer[alpha + 1] == 1) ? true : false;
        break;

      case PREFIX_TEXT_USE_HUE_4:
        text_parameters[4].use_hue = (sensor_data_frame.frame_buffer[alpha + 1] == 1) ? true : false;
        break;

      case PREFIX_DEBUG_STATE:
        menu_parameters.debug = (sensor_data_frame.frame_buffer[alpha + 1] == 1) ? true : false;
        break;

      case PREFIX_SCREEN_MODE:
        screen_parameters.mode = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_SD_MOUNTED_1:
        menu_parameters.sd_card1_detected = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_SD_MOUNTED_2:
        menu_parameters.sd_card2_detected = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_SCROLL_SPEED_X_0:
        cursor_parameters[0].x_dir = sensor_data_frame.frame_buffer[alpha + 1] - 128;
        break;

      case PREFIX_TEXT_SCROLL_SPEED_Y_0:
        cursor_parameters[0].y_dir = sensor_data_frame.frame_buffer[alpha + 1] - 128;
        break;

      case PREFIX_TEXT_SCROLL_SPEED_X_1:
        cursor_parameters[1].x_dir = sensor_data_frame.frame_buffer[alpha + 1] - 128;
        break;

      case PREFIX_TEXT_SCROLL_SPEED_Y_1:
        cursor_parameters[1].y_dir = sensor_data_frame.frame_buffer[alpha + 1] - 128;
        break;

      case PREFIX_TEXT_SCROLL_SPEED_X_2:
        cursor_parameters[2].x_dir = sensor_data_frame.frame_buffer[alpha + 1] - 128;
        break;

      case PREFIX_TEXT_SCROLL_SPEED_Y_2:
        cursor_parameters[2].y_dir = sensor_data_frame.frame_buffer[alpha + 1] - 128;
        break;

      case PREFIX_TEXT_SCROLL_SPEED_X_3:
        cursor_parameters[3].x_dir = sensor_data_frame.frame_buffer[alpha + 1] - 128;
        break;

      case PREFIX_TEXT_SCROLL_SPEED_Y_3:
        cursor_parameters[3].y_dir = sensor_data_frame.frame_buffer[alpha + 1] - 128;
        break;

      case PREFIX_TEXT_SCROLL_SPEED_X_4:
        cursor_parameters[4].x_dir = sensor_data_frame.frame_buffer[alpha + 1] - 128;
        break;

      case PREFIX_TEXT_SCROLL_SPEED_Y_4:
        cursor_parameters[4].y_dir = sensor_data_frame.frame_buffer[alpha + 1] - 128;
        break;

      case PREFIX_TEXT_OBJ_ENABLED_0:
        text_parameters[3].object_used = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      case PREFIX_TEXT_OBJ_ENABLED_1:
        text_parameters[3].object_used = sensor_data_frame.frame_buffer[alpha + 1];
        break;
        
      case PREFIX_TEXT_OBJ_ENABLED_2:
        text_parameters[3].object_used = sensor_data_frame.frame_buffer[alpha + 1];
        break;
        
      case PREFIX_TEXT_OBJ_ENABLED_3:
        text_parameters[3].object_used = sensor_data_frame.frame_buffer[alpha + 1];
        break;
        
      case PREFIX_TEXT_OBJ_ENABLED_4:
        text_parameters[3].object_used = sensor_data_frame.frame_buffer[alpha + 1];
        break;

      default:  Sprint("Error: recieved prefix not defined. Prefix :");
        Sprintln((byte)sensor_data_frame.frame_buffer[alpha + 1]);


    }
  }
  return (0);
}


void Coms::extract_pos_frame(byte obj_num) {
  pos_frame.frame_arrived = false;
  cursor_parameters[obj_num].global_x_pos = calc_pos(pos_frame.frame_buffer[4], pos_frame.frame_buffer[5]);
  cursor_parameters[obj_num].global_y_pos = calc_pos(pos_frame.frame_buffer[6], pos_frame.frame_buffer[7]);


  cursor_parameters[obj_num].local_x_pos = calc_local_pos(pos_frame.frame_buffer[4], pos_frame.frame_buffer[5]);
  cursor_parameters[obj_num].local_y_pos = calc_local_pos(pos_frame.frame_buffer[6], pos_frame.frame_buffer[7]);
//  calc_local_pos();

  cursor_parameters[obj_num].x_dir = pos_frame.frame_buffer[8] - 128;
  cursor_parameters[obj_num].y_dir = pos_frame.frame_buffer[9] - 128;



}

void Coms::extract_menu_frame() {
  menu_frame.frame_arrived = false;
}


inline int Coms::calc_pos(byte MSB, byte LSB) {
return (MSB<<8)+LSB;
}

inline int Coms::calc_local_pos(byte MSB, byte LSB) {
int temp = (MSB<<8)+LSB;
return  temp -(screen_parameters.node_address << SINGLE_MATRIX_WIDTH_AS_POW_2);
}

#endif // COMS_CPP
