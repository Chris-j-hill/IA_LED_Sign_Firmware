
#ifndef Host_CPP
#define Host_CPP


#include "Host.h"
#include "Menus.h"
#include "Graphics.h"
#include "Local_config.h"

extern struct Menu_Struct menu_parameters;
extern struct Menu_tree_items menu_items;
extern struct Text_Struct text_parameters[MAX_NUM_OF_TEXT_OBJECTS];
extern struct Screen_Struct screen_parameters;
extern struct Cursor_Struct cursor_parameters[MAX_NUM_OF_TEXT_OBJECTS];
extern Menu menu;

String last_command = "text0";

const char pin_error_msg PROGMEM = "Error: Cannot change pin value during operation, aborting assignment";
const char string_write_error_msg PROGMEM = "Error: Cannot write string from serial monitor interface";

//some frequenly used strings
const char space_colon_string PROGMEM = " : ";
const char dash_space_string PROGMEM = "- ";
const char tab_string PROGMEM = "\t";
const char space_string PROGMEM = " ";
const char percent_string PROGMEM = "%";
const char divide_string PROGMEM = "/";
const char comma_space_string PROGMEM = ", ";
const char yes_string PROGMEM = "Yes";
const char no_string PROGMEM = "No ";
const char y_string PROGMEM = "Y";
const char n_string PROGMEM = "N";

inline void space() {
  Serial.print(space_string);
};

inline void tab() {
  Serial.print(tab_string);
};

inline void space_colon() {
  Serial.print(space_colon_string);
};

inline void dash_space() {
  Serial.print(dash_space_string);
};

inline void comma_space() {
  Serial.print(comma_space_string);
}

inline void yes() {
  Serial.print(yes_string);
}

inline void no() {
  Serial.print(no_string);
}

inline void y() {
  Serial.print(y_string);
}
inline void n() {
  Serial.print(n_string);
}

inline void print_command_name(String command_data) {
  Serial.print(command_data);
  space_colon();
};

inline void print_new_command_name(String command_data) {
  Serial.println(F("New Value"));
  print_command_name(command_data);
};


void Host::init_serial() {
  Serial.begin(HOST_SERIAL_SPEED);
}

void Host::check_serial() {   //to read incomming data

  if (Serial.available() > 0) {

    String rx = Serial.readString();
    rx.trim();  //trim off return carraige

    byte temp = data_to_report; // log what we were doing in case we only want

    //set message printing mode
    data_to_report = data_set_LUT(rx);
  }
}



byte Host::data_set_LUT(String data_set) {

  if (data_set == "stop")
    return STOP_REPORT;

  else if (data_set == "menu")
    return REPORT_MENU;

    else if (data_set == "config")
    return REPORT_CONFIG;

    
  //check substring for text
  String sub_string_data_set = data_set.substring(0, data_set.length()-1);
  if (sub_string_data_set == "text"){
    last_command = data_set;
    return REPORT_TEXT;
  }
  else
    return 255;
}





void Host::print_messages() {
  static uint32_t last_message_print_time;
  static byte previously_reporting = 0; // reset header counter if just changed printing messages

  if (millis() > last_message_print_time + MESSAGE_DELAY_PERIOD) { // check if specified time delay elapsed

    last_message_print_time = millis();

    // increment counter for printing header info message
    if (previously_reporting != data_to_report)
      header_print_counter = 0;
    else
      header_print_counter += HEADER_PRINT_INCREMENT;

    previously_reporting = data_to_report;

    switch (data_to_report) {
      case STOP_REPORT:
        break;

      case REPORT_CONFIG:
        print_config();
        break;

      case REPORT_MENU:
        print_menu_tree();
        break;

      case REPORT_TEXT:
        print_text(last_command);
        break;

        break;

      default: break;
    }

  }
}











//void Host::print_text(String command) {
//
//  String obj_num_as_char = command.substring(command.length()-1);
//  byte obj_num = obj_num_as_char.toInt();
//  if (obj_num >= MAX_NUM_OF_TEXT_OBJECTS)
//    obj_num = 0;
//
//  if (header_print_counter == 0) {
//    Serial.println();
//    Serial.println(F("Configured \tSize \tR   G   B   H    \tUse Hue \tXpos Ypos \t\tScroll Speeds   \tStart points   \tEnd Points \tLimits X- X+ Y- Y+"));
//  }
//  if (text_cursor[obj_num].object_used)
//    yes();
//  else{
//    no();space();
//  }
//  tab(); tab();
//
//  Serial.print(text_parameters[obj_num].text_size);
//  tab();
//
//  Serial.print(text_parameters[obj_num].red);
//  if (text_parameters[obj_num].red < 10) space();
//  if (text_parameters[obj_num].red < 100) space();
//  space();
//
//  Serial.print(text_parameters[obj_num].green);
//  if (text_parameters[obj_num].green < 10) space();
//  if (text_parameters[obj_num].green < 100) space();
//  space();
//
//  Serial.print(text_parameters[obj_num].blue);
//  if (text_parameters[obj_num].blue < 10) space();
//  if (text_parameters[obj_num].blue < 100) space();
//  space();
//
//  Serial.print(text_parameters[obj_num].hue);
//  if (abs(text_parameters[obj_num].hue < 10)) space();
//  if (abs(text_parameters[obj_num].hue < 100)) space();
//  if (abs(text_parameters[obj_num].hue < 1000)) space();
//  if (text_parameters[obj_num].hue >= 0) space();
//  tab();
//
//  if (text_parameters[obj_num].use_hue)
//    yes();
//  else{
//    no();space();
//  }
//  tab(); tab();
//
//  Serial.print(text_cursor[obj_num].x);
//  if (abs(text_cursor[obj_num].x < 10)) space();
//  if (abs(text_cursor[obj_num].x < 100)) space();
//  if (text_cursor[obj_num].x >= 0) space();
//  space();
//
//  Serial.print(text_cursor[obj_num].y);
//  if (abs(text_cursor[obj_num].y < 10)) space();
//  if (abs(text_cursor[obj_num].y < 100)) space();
//  if (text_cursor[obj_num].y >= 0) space();
//  tab(); tab();
//
//  Serial.print(text_cursor[obj_num].x_pos_dir - 128);
//  if (abs(text_cursor[obj_num].x_pos_dir - 128 < 10)) space();
//  if (abs(text_cursor[obj_num].x_pos_dir - 128 < 100)) space();
//  if (text_cursor[obj_num].x_pos_dir - 128 >= 0) space();
//  space();
//
//  Serial.print(text_cursor[obj_num].y_pos_dir - 128);
//  if (abs(text_cursor[obj_num].y_pos_dir - 128 < 10)) space();
//  if (abs(text_cursor[obj_num].y_pos_dir - 128 < 100)) space();
//  if (text_cursor[obj_num].y_pos_dir - 128 >= 0) space();
//  space();
//  tab();tab();
//
//  Serial.print(text_cursor[obj_num].x_start);
//  if (abs(text_cursor[obj_num].x_start < 10)) space();
//  if (abs(text_cursor[obj_num].x_start < 100)) space();
//  if (text_cursor[obj_num].x_start >= 0) space();
//  space();
//
//  Serial.print(text_cursor[obj_num].y_start);
//  if (abs(text_cursor[obj_num].y_start < 10)) space();
//  if (abs(text_cursor[obj_num].y_start < 100)) space();
//  if (text_cursor[obj_num].y_start >= 0) space();
//  space();
//
//  tab();tab();
//  Serial.print(text_cursor[obj_num].x_end);
//  if (abs(text_cursor[obj_num].x_end < 10)) space();
//  if (abs(text_cursor[obj_num].x_end < 100)) space();
//  if (text_cursor[obj_num].x_end >= 0) space();
//  space();
//
//  Serial.print(text_cursor[obj_num].y_end);
//  if (abs(text_cursor[obj_num].y_end < 10)) space();
//  if (abs(text_cursor[obj_num].y_end < 100)) space();
//  if (text_cursor[obj_num].y_end >= 0) space();
//  space();
//
//  tab();tab();
//
//  Serial.print(text_cursor[obj_num].x_limit_min);
//  space();
//  Serial.print(text_cursor[obj_num].x_limit_max);
//  space();
//  Serial.print(text_cursor[obj_num].y_limit_min);
//  space();
//  Serial.print(text_cursor[obj_num].y_limit_max);
//
//
//  Serial.println();
//
//}


void Host::print_menu_tree() {

  //  static int previous_menu_displayed = menu.get_current_menu();
  //  if (previous_menu_displayed != menu.get_current_menu()) //if we changed menu for menu auto changed by other process, push all menu options to host
  //    print_menu_tree_options();
  //
  //  previous_menu_displayed = menu.get_current_menu();

  if (header_print_counter == 0) {
    Serial.println();
    Serial.println(F("Pos \t Current Menu  \t Value Selected"));
  }
  Serial.print(menu_parameters.encoder_position);
  tab();
  print_menu_tree_options(menu.get_current_menu());
  tab();

  //convert encoder position to menu item
  position_to_menu_value();

  Serial.println();
}


void Host::print_menu_tree_options(int cur_menu) {

  if (cur_menu == -1) {
    switch (menu.get_current_menu()) {
      case DEFAULT_MENU:
        Serial.print(F("default menu, press encoder to enter menu tree"));
        break;

      case MAIN_MENU:
        Serial.println(menu_items.RETURN);
        Serial.println(menu_items.screen_mode);
        Serial.println(menu_items.brightness);
        Serial.println(menu_items.text_settings);
        Serial.println(menu_items.fan_settings);
        Serial.println(menu_items.internet_settings);
        Serial.println(menu_items.sd_card_settings);
        Serial.println(menu_items.led_strip_settings);
        break;

      case SCREEN_MODE_MENU:
        Serial.println(menu_items.RETURN);
        Serial.println(menu_items.screen_mode0);
        Serial.println(menu_items.screen_mode1);
        Serial.println(menu_items.screen_mode3);
        Serial.println(menu_items.screen_mode2);
        break;

      case TEXT_SETTINGS_MENU:
        Serial.println(menu_items.RETURN);
        Serial.println(menu_items.text_size_settings);
        Serial.println(menu_items.text_colour_settings);
        Serial.println(menu_items.scroll_speed_settings);
        Serial.println(menu_items.flip_dir_settings);
        break;

      case FAN_SETTINGS_MENU:
        Serial.println(menu_items.RETURN);
        Serial.println(menu_items.fan_speed_settings);
        Serial.println(menu_items.fan_enable);
        Serial.println(menu_items.fan_disable);
        Serial.println(menu_items.minimum_rotating_speed);
        break;

      case INTERNET_CONFIG_MENU:
        Serial.println(menu_items.RETURN);
        Serial.println(menu_items.select_network_manually);
        Serial.println(menu_items.ethernet_enable);
        Serial.println(menu_items.ethernet_disable);
        Serial.println(menu_items.wifi_enable);
        Serial.println(menu_items.wifi_disable);
        break;

      case SD_CARD_MENU:
        Serial.println(menu_items.RETURN);
        Serial.println(menu_items.enable_ext_card);
        Serial.println(menu_items.disable_ext_card);
        Serial.println(menu_items.sd_card_folders);
        break;

      case LED_STRIP_MENU:
        Serial.println(menu_items.RETURN);
        Serial.println(menu_items.enable_led_strip);
        Serial.println(menu_items.disable_led_strip);
        Serial.println(menu_items.led_strip_brightness);
        break;

      case TEXT_COLOUR_MENU:
        Serial.println(menu_items.RETURN);
        Serial.println(menu_items.text_colour_red);
        Serial.println(menu_items.text_colour_green);
        Serial.println(menu_items.text_colour_blue);
        Serial.println(menu_items.text_colour_hue);
        Serial.println(menu_items.text_colour_use_hue);
        Serial.println(menu_items.text_colour_use_rgb);
        break;

      //if any of these just print same string, these have no sub menu list
      case BRIGHTNESS_MENU:
      case TEXT_SIZE_MENU:
      case SCROLL_SPEED_MENU:
      case FAN_SPEED_MENU:
      case MIN_FAN_SPEED_MENU:
      //      case SD_FOLDERS_MENU:
      case LED_STRIP_BRIGHTNESS_MENU:
      case TEXT_COLOUR_RED:
      case TEXT_COLOUR_GREEN:
      case TEXT_COLOUR_BLUE:
      case TEXT_COLOUR_HUE:

        Serial.println(F("Variable adjustment menu, click button to return"));
        break;
    }

  }
  else
  {
    switch (cur_menu) {
      case STARTUP:                     Serial.print(F("System Starting, Please wait..."));     break;
      case DEFAULT_MENU:                Serial.print(F("Menu Minimised, Press button"));        break;
      case MAIN_MENU:                   Serial.print(menu_items.main_menu);                     break;
      case SCREEN_MODE_MENU:            Serial.print(menu_items.screen_mode_menu);              break;
      case TEXT_SETTINGS_MENU:          Serial.print(menu_items.text_settings_menu);            break;
      case FAN_SETTINGS_MENU:           Serial.print(menu_items.fan_settings_menu);             break;
      case INTERNET_CONFIG_MENU:        Serial.print(menu_items.internet_config_menu);          break;
      case SD_CARD_MENU:                Serial.print(menu_items.sd_cards_menu);                 break;
      case LED_STRIP_MENU:              Serial.print(menu_items.led_strip_menu);                break;
      case TEXT_COLOUR_MENU:            Serial.print(menu_items.text_colour_menu);              break;
      case BRIGHTNESS_MENU:             Serial.print(menu_items.brightness_menu);               break;
      case TEXT_SIZE_MENU:              Serial.print(menu_items.text_size_menu);                break;
      case SCROLL_SPEED_MENU:           Serial.print(menu_items.scroll_speed_menu);             break;
      case FAN_SPEED_MENU:              Serial.print(menu_items.fan_speed_menu);                break;
      case MIN_FAN_SPEED_MENU:          Serial.print(menu_items.minimum_fan_speed_menu);        break;
      //      case SD_FOLDERS_MENU:             Serial.print(menu_items.SD_card_folders_menu);          break;
      case LED_STRIP_BRIGHTNESS_MENU:   Serial.print(menu_items.led_strip_brightness_menu);     break;
      case TEXT_COLOUR_RED:             Serial.print(menu_items.text_colour_red_menu);          break;
      case TEXT_COLOUR_GREEN:           Serial.print(menu_items.text_colour_green_menu);        break;
      case TEXT_COLOUR_BLUE:            Serial.print(menu_items.text_colour_blue_menu);         break;
      case TEXT_COLOUR_HUE:             Serial.print(menu_items.text_colour_hue_menu);          break;
    }
  }
}


void Host::position_to_menu_value() {
  //given our current menu, what item is the encoder selecting
  switch (menu.get_current_menu()) {
    case MAIN_MENU:
      switch (menu_parameters.encoder_position) {
        case 0: Serial.print(menu_items.RETURN);                   break;
        case 1: Serial.print(menu_items.screen_mode);              break;
        case 2: Serial.print(menu_items.brightness);               break;
        case 3: Serial.print(menu_items.text_settings);            break;
        case 4: Serial.print(menu_items.fan_settings);             break;
        case 5: Serial.print(menu_items.internet_settings);        break;
        case 6: Serial.print(menu_items.sd_card_settings);         break;
        case 7: Serial.print(menu_items.led_strip_settings);       break;
      }
      break;

    case SCREEN_MODE_MENU:
      switch (menu_parameters.encoder_position) {
        case 0: Serial.print(menu_items.RETURN);                    break;
        case 1: Serial.print(menu_items.screen_mode0);              break;
        case 2: Serial.print(menu_items.screen_mode1);              break;
        case 3: Serial.print(menu_items.screen_mode3);              break;
        case 4: Serial.print(menu_items.screen_mode2);              break;
      }
      break;

    case TEXT_SETTINGS_MENU:
      switch (menu_parameters.encoder_position) {
        case 0: Serial.print(menu_items.RETURN);                    break;
        case 1: Serial.print(menu_items.text_size_settings);        break;
        case 2: Serial.print(menu_items.text_colour_settings);      break;
        case 3: Serial.print(menu_items.scroll_speed_settings);     break;
        case 4: Serial.print(menu_items.flip_dir_settings);         break;
      }
      break;

    case FAN_SETTINGS_MENU:
      switch (menu_parameters.encoder_position) {
        case 0: Serial.print(menu_items.RETURN);                    break;
        case 1: Serial.print(menu_items.fan_speed_settings);        break;
        case 2: Serial.print(menu_items.fan_enable);                break;
        case 3: Serial.print(menu_items.fan_disable);               break;
        case 4: Serial.print(menu_items.minimum_rotating_speed);    break;
      }
      break;

    case INTERNET_CONFIG_MENU:
      switch (menu_parameters.encoder_position) {
        case 0: Serial.print(menu_items.RETURN);                    break;
        case 1: Serial.print(menu_items.select_network_manually);   break;
        case 2: Serial.print(menu_items.ethernet_enable);           break;
        case 3: Serial.print(menu_items.ethernet_disable);          break;
        case 4: Serial.print(menu_items.wifi_enable);               break;
        case 5: Serial.print(menu_items.wifi_disable);              break;
      }
      break;

    case SD_CARD_MENU:
      switch (menu_parameters.encoder_position) {
        case 0: Serial.print(menu_items.RETURN);                    break;
        case 1: Serial.print(menu_items.enable_ext_card);           break;
        case 2: Serial.print(menu_items.disable_ext_card);          break;
        case 3: Serial.print(menu_items.sd_card_folders);           break;
      }
      break;

    case LED_STRIP_MENU:
      switch (menu_parameters.encoder_position) {
        case 0: Serial.print(menu_items.RETURN);                    break;
        case 1: Serial.print(menu_items.enable_led_strip);          break;
        case 2: Serial.print(menu_items.disable_led_strip);         break;
        case 3: Serial.print(menu_items.led_strip_brightness);      break;
      }
      break;

    case TEXT_COLOUR_MENU:
      switch (menu_parameters.encoder_position) {
        case 0: Serial.print(menu_items.RETURN);                    break;
        case 1: Serial.print(menu_items.text_colour_red);           break;
        case 2: Serial.print(menu_items.text_colour_green);         break;
        case 3: Serial.print(menu_items.text_colour_blue);          break;
        case 4: Serial.print(menu_items.text_colour_hue);           break;
        case 5: Serial.print(menu_items.text_colour_use_hue);       break;
        case 6: Serial.print(menu_items.text_colour_use_rgb);       break;
      }
      break;

    case BRIGHTNESS_MENU:
      Serial.print(screen_parameters.brightness);
      Serial.print(percent_string);
      break;

    case TEXT_SIZE_MENU:
      Serial.print(text_parameters[0].text_size);
      Serial.print(divide_string);
      Serial.print(menu.current_menu_limit_max);
      break;

    case SCROLL_SPEED_MENU:
      switch (menu_parameters.encoder_position) {
        case 0: Serial.print(menu_items.RETURN);                    break;
        case 1: Serial.print(menu_items.scroll_speed_x);            break;
        case 2: Serial.print(menu_items.scroll_speed_y);            break;
      }
      break;
  
    case FAN_SPEED_MENU:
      Serial.print(menu_parameters.fan_speed);
      Serial.print(divide_string);
      Serial.print(menu.current_menu_limit_max);
      break;

    case MIN_FAN_SPEED_MENU:
      Serial.print(menu_parameters.min_fan_speed);
      Serial.print(divide_string);
      Serial.print(menu.current_menu_limit_max);
      break;

    //      case SD_FOLDERS_MENU:


    case LED_STRIP_BRIGHTNESS_MENU:
      Serial.print(menu_parameters.led_strip_brightness);
      Serial.print(divide_string);
      Serial.print(menu.current_menu_limit_max);
      break;

    case TEXT_COLOUR_RED:
      Serial.print(text_parameters[0].colour_r);
      Serial.print(divide_string);
      Serial.print(menu.current_menu_limit_max);
      break;

    case TEXT_COLOUR_GREEN:
      Serial.print(text_parameters[0].colour_g);
      Serial.print(divide_string);
      Serial.print(menu.current_menu_limit_max);
      break;

    case TEXT_COLOUR_BLUE:
      Serial.print(text_parameters[0].colour_b);
      Serial.print(divide_string);
      Serial.print(menu.current_menu_limit_max);
      break;

    case TEXT_COLOUR_HUE:
      Serial.print(text_parameters[0].hue);
      Serial.print(" -> range: ");
      Serial.print(menu.current_menu_limit_min);
      Serial.print(divide_string);
      Serial.print(menu.current_menu_limit_max);
      break;

    case SCROLL_SPEED_MENU_X:
      Serial.print(cursor_parameters[0].x_dir - 128);
      Serial.print(" -> range: -");
      Serial.print(menu.current_menu_limit_min - 129);
      Serial.print(divide_string);
      Serial.print(menu.current_menu_limit_max - 128);
      break;

    case SCROLL_SPEED_MENU_Y:
      Serial.print(cursor_parameters[0].y_dir - 128);
      Serial.print(" -> range: -");
      Serial.print(menu.current_menu_limit_min - 129);
      Serial.print(divide_string);
      Serial.print(menu.current_menu_limit_max - 128);
      break;
  }                     
}



void Host::print_bits(uint32_t var, byte digits, byte units, bool carriage_return) {
  uint32_t test;
  if (units == BIN)
    test = (1 << (digits - 1));
  else
    test = (0xF << (digits - 1));

  for (test; test; ) {
    if (units == BIN) {
      Serial.write(var  & test ? '1' : '0');
      test >>= 1;
    }
    else if (units == HEX) {
      Serial.print(var & test, HEX);
      test >>= 4;
    }

  }
  if (carriage_return)
    Serial.println();
}


#endif // Host_CPP
