#ifndef Menu_CPP
#define Menu_CPP

#include "Menu_Tree.h"
#include "Config_Local.h"
#include "Encoder.h"
#include "Coms_Serial.h"
#include "Coms.h"
#include "Graphics.h"
#include "Current_Control.h"
#include "Fans.h"
#include "Internet.h"
#include "SD_Cards.h"
#include "Led_Strip.h"


// forward declare extern variables
Menu_tree_items menu_items;
Menu_tree_menu_limits menu_limits;
extern struct Button_Struct button_parameters;
extern struct Encoder_Struct encoder_parameters;
extern struct Fan_Struct fan_parameters;
extern struct Led_Strip_Struct led_strip_parameters;
extern struct SD_Card card1;
extern struct SD_Card card2;
extern struct Text text_parameters[MAX_NUM_OF_TEXT_OBJECTS];
extern struct Text_cursor text_cursor[MAX_NUM_OF_TEXT_OBJECTS];


//classes
extern Encoder encoder;
extern Graphics graphics;
extern Coms_Serial coms_serial;
extern Fans fans;
extern Internet internet;
extern Card card;
extern Led_Strip led_strip;
extern Menu menu;



extern byte screen_mode;
extern byte screen_brightness;


//declare local variables

int time_since_menu_last_changed = 0; //time variable updated from encoder

int time_since_menu_startup_run = 0;

bool supress_frame_to_this_screen = false;  // use this to supress text frame to screens covered fully by menu
// other screens will require a black box cover if menu visable

bool menu_just_changed = false;


//menu class methods

void Menu::init_menu_tree() {

#ifdef SKIP_INTITAL_STARTUP_SEQUENCE
  current_menu = DEFUALT_MENU;
#else
  current_menu = STARTUP;
#endif

}


//level 1 menus
void Menu::display_menu() {

  if (encoder_parameters.encoder_moved) {
    encoder.encoder_position_limits();    //make sure encoder position is within the range limits of the current menu
  }

  if (MENU_VISABLITIY_TIMOUT < millis() - time_since_menu_last_changed) {
    current_menu = DEFAULT_MENU;
  }

  switch (current_menu) {
    case STARTUP:                     display_startup_sequence();
    case DEFAULT_MENU:                default_display(); break;
    case MAIN_MENU:                   display_main_menu(); break;
    case SCREEN_MODE_MENU:            display_screen_mode_menu(); break;
    case BRIGHTNESS_MENU:             display_screen_brightness_menu(); break;
    case TEXT_SETTINGS_MENU:          display_text_settings_menu(); break;
    case TEXT_OBJ_SELECTION_MENU:     display_text_obj_selection_menu(); break;
    case FAN_SETTINGS_MENU:           display_fan_settings_menu(); break;
    case INTERNET_CONFIG_MENU:        display_internet_config_menu(); break;
    case SD_CARD_MENU:                display_SD_cards_menu(); break;
    case LED_STRIP_MENU:              display_led_strip_menu(); break;
    case TEXT_SIZE_MENU:              display_text_size_menu(); break;
    case TEXT_COLOUR_MENU:            display_text_colour_menu(); break;
    case SCROLL_SPEED_MENU:           display_scroll_speed_menu(); break;
    case FAN_SPEED_MENU:              display_fan_speed_menu(); break;
    case MIN_FAN_SPEED_MENU:          display_min_fan_speed_menu(); break;
    case SD_FOLDERS_MENU:             display_sd_folder_menu(); break;
    case LED_STRIP_BRIGHTNESS_MENU:   display_led_strip_brightness_menu(); break;
    case TEXT_COLOUR_RED:             display_text_colour_red_menu(); break;
    case TEXT_COLOUR_GREEN:           display_text_colour_green_menu(); break;
    case TEXT_COLOUR_BLUE:            display_text_colour_blue_menu(); break;
    case SCROLL_SPEED_MENU_X:         display_text_scroll_speed_x(); break;
    case SCROLL_SPEED_MENU_Y:         display_text_scroll_speed_y(); break;

    default: current_menu = STARTUP;    //restart, run startup
  }
}

void Menu::display_startup_sequence() {

  if (encoder_parameters.encoder_moved) {} //disregard encoder and button for this

  if (button_parameters.button_pressed) {}

  time_since_menu_startup_run = millis();  //log time this function was run
  coms_serial.send_menu_frame(STARTUP, encoder_parameters.position);
  current_menu = DEFAULT_MENU;  // force this as next menu
}

void Menu::default_display() {
  current_menu = DEFAULT_MENU;  //return to this menu until current_menu changed

  //only run if startup finished
  if (millis() - time_since_menu_startup_run > TIME_TO_DISPLAY_STARTUP) {
    if (menu_just_changed) {
      encoder.recenter_encoder();
      coms_serial.send_menu_frame(DEFAULT_MENU, encoder_parameters.position);
      menu_just_changed = false;
    }

    if (button_parameters.button_pressed) {
      current_menu = MAIN_MENU;
      menu_just_changed = true;
      time_since_menu_last_changed = millis();
      button_parameters.button_pressed = false;
    }
  }
  else {
    encoder_parameters.encoder_moved = false;   //ignore button presses and encoder input before startup finished
    button_parameters.button_pressed = false;
  }
}

void Menu::display_main_menu() {
  current_menu = MAIN_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.recenter_encoder();
    coms_serial.send_menu_frame(MAIN_MENU, encoder_parameters.position);
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(MAIN_MENU, encoder_parameters.position);
    encoder_parameters.encoder_moved = false;
  }

  if (button_parameters.button_pressed) {
    switch (encoder_parameters.position) {
      case 0: current_menu = DEFAULT_MENU;            break;
      case 1: current_menu = SCREEN_MODE_MENU;        break;
      case 2: current_menu = BRIGHTNESS_MENU;         break;
      case 3: current_menu = TEXT_OBJ_SELECTION_MENU; break;
      //      case 3: current_menu = TEXT_SETTINGS_MENU;      break;
      case 4: current_menu = FAN_SETTINGS_MENU;       break;
      case 5: current_menu = INTERNET_CONFIG_MENU;    break;
      case 6: current_menu = SD_CARD_MENU;            break;
      case 7: current_menu = LED_STRIP_MENU;          break;

      default: current_menu = STARTUP;
    }

    menu_just_changed = true;
    time_since_menu_last_changed = millis();
    button_parameters.button_pressed = false;
  }
}


// level 2 menus
void Menu::display_screen_mode_menu() {
  current_menu = SCREEN_MODE_MENU;

  if (menu_just_changed) {

    menu_just_changed = false;
    encoder.recenter_encoder();
    coms_serial.send_menu_frame(SCREEN_MODE_MENU, encoder_parameters.position);
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(SCREEN_MODE_MENU, encoder_parameters.position);
    encoder_parameters.encoder_moved = false;
  }

  if (button_parameters.button_pressed) {
    switch (encoder_parameters.position) {
      case 0: current_menu = MAIN_MENU;   break;    //return
      case 1: screen_mode = 0 ;           break;
      case 2: screen_mode = 1 ;           break;
      case 3: screen_mode = 2 ;           break;
      case 4: screen_mode = 3 ;           break;

      default: current_menu = STARTUP;

    }
    if (encoder_parameters.position >= 1 && encoder_parameters.position <= 4) {
      for (int i = 0; i < NUM_SCREENS; i++)
        coms_serial.send_specific_calibration_data(PREFIX_SCREEN_MODE, i, false, 0);  //send screen_mode update to screens
    }
    if (encoder_parameters.position == 0)
      menu_just_changed = true;
    time_since_menu_last_changed = millis();
    button_parameters.button_pressed = false;
  }
}

void Menu::display_screen_brightness_menu() {
  current_menu = BRIGHTNESS_MENU;

  if (menu_just_changed) {

    menu_just_changed = false;
    encoder.set_encoder_position(screen_brightness);
    coms_serial.send_menu_frame(BRIGHTNESS_MENU, encoder_parameters.position);
  }

  if (button_parameters.button_pressed) {
    current_menu = MAIN_MENU;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(2);
  }

  if (encoder_parameters.encoder_moved) {
    screen_brightness = encoder_parameters.position;
    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();
    for (int i = 0; i < NUM_SCREENS; i++)
      coms_serial.send_specific_calibration_data(PREFIX_SCREEN_BRIGHTNESS, i, false, 0);  //send screen_mode update to screens
  }
}

void Menu::display_text_obj_selection_menu() {
  current_menu = TEXT_OBJ_SELECTION_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.recenter_encoder();
    check_obj_enabled();    //function to count which objects are enabled and put them in array
    coms_serial.send_menu_frame(TEXT_OBJ_SELECTION_MENU, encoder_parameters.position);
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(TEXT_OBJ_SELECTION_MENU, encoder_parameters.position);
    encoder_parameters.encoder_moved = false;
  }


  if (button_parameters.button_pressed) {
    //instead of switch, auto accommodates N text objects, most options go to TEXT_SETTINGS_MENU
    if (encoder_parameters.position <= num_obj_enabled) {
      if (encoder_parameters.position == 0)
        current_menu = MAIN_MENU;
      else if (encoder_parameters.position > 0 && encoder_parameters.position <= num_obj_enabled && num_obj_enabled != 0) {
        obj_selected = obj_enabled[encoder_parameters.position - 1];
        current_menu = TEXT_SETTINGS_MENU;
      }
      coms_serial.send_menu_frame(TEXT_OBJ_SELECTION_MENU, encoder_parameters.position-1);
    }
    else
      current_menu = STARTUP;

    menu_just_changed = true;
    time_since_menu_last_changed = millis();
    button_parameters.button_pressed = false;
  }
}

void Menu::display_text_settings_menu() {
  current_menu = TEXT_SETTINGS_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.recenter_encoder();
    coms_serial.send_menu_frame(TEXT_SETTINGS_MENU, encoder_parameters.position);
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(TEXT_SETTINGS_MENU, encoder_parameters.position);
    encoder_parameters.encoder_moved = false;
  }

  if (button_parameters.button_pressed) {
    switch (encoder_parameters.position) {
      case 0: current_menu = TEXT_OBJ_SELECTION_MENU;   break;
      case 1: current_menu = TEXT_SIZE_MENU;            break;
      case 2: current_menu = TEXT_COLOUR_MENU;          break;
      case 3: current_menu = SCROLL_SPEED_MENU;         break;
      default: current_menu = STARTUP;
    }

    menu_just_changed = true;
    time_since_menu_last_changed = millis();
    button_parameters.button_pressed = false;
  }
}

void Menu::display_fan_settings_menu() {
  current_menu = FAN_SETTINGS_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.recenter_encoder();
    coms_serial.send_menu_frame(FAN_SETTINGS_MENU, encoder_parameters.position);
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(FAN_SETTINGS_MENU, encoder_parameters.position);
    encoder_parameters.encoder_moved = false;
  }

  if (button_parameters.button_pressed) {
    switch (encoder_parameters.position) {
      case 0: current_menu = MAIN_MENU;          break;
      case 1: current_menu = FAN_SPEED_MENU;     break;
      case 2:
        if (fan_parameters.enabled)
          fans.disable();
        else
          fans.enable();
        break;
      case 3: current_menu = MIN_FAN_SPEED_MENU; break;

      default: current_menu = STARTUP;
    }

    if (encoder_parameters.position == 0 || encoder_parameters.position == 1 || encoder_parameters.position == 4)
      menu_just_changed = true;
    time_since_menu_last_changed = millis();
    button_parameters.button_pressed = false;
  }
}

void Menu::display_internet_config_menu() {
  current_menu = INTERNET_CONFIG_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.recenter_encoder();
    coms_serial.send_menu_frame(INTERNET_CONFIG_MENU, encoder_parameters.position);
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(INTERNET_CONFIG_MENU, encoder_parameters.position);
    encoder_parameters.encoder_moved = false;
  }

  if (button_parameters.button_pressed) {
    switch (encoder_parameters.position) {
      case 0: current_menu = MAIN_MENU;          break;
      case 1: /*manually connect*/               break;     // <- implement this
      case 2: internet.ethernet_enable();        break;
      case 3: internet.ethernet_disable();       break;
      case 4: internet.wifi_enable();            break;
      case 5: internet.wifi_disable();           break;

      default: current_menu = STARTUP;
    }
    if (encoder_parameters.position == 0 || encoder_parameters.position == 1)
      menu_just_changed = true;
    time_since_menu_last_changed = millis();
    button_parameters.button_pressed = false;
  }
}

void Menu::display_SD_cards_menu() {
  current_menu = SD_CARD_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.recenter_encoder();
    coms_serial.send_menu_frame(SD_CARD_MENU, encoder_parameters.position);
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(SD_CARD_MENU, encoder_parameters.position);
    encoder_parameters.encoder_moved = false;
  }

  if (button_parameters.button_pressed) {
    switch (encoder_parameters.position) {
      case 0: current_menu = MAIN_MENU;          break;
      case 1:
        if (card1.enabled)
          card.mount_card(1);
        else
          card.safely_eject_card(1);
        break;

      case 2:
        if (card1.enabled)
          card.mount_card(1);
        else
          card.safely_eject_card(1);
        break;
      case 3: current_menu = SD_FOLDERS_MENU;    break;

      default: current_menu = STARTUP;
    }
    if (encoder_parameters.position == 0 || encoder_parameters.position == 3)
      menu_just_changed = true;
    time_since_menu_last_changed = millis();
    button_parameters.button_pressed = false;
  }
}

void Menu::display_led_strip_menu() {
  current_menu = LED_STRIP_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.recenter_encoder();
    coms_serial.send_menu_frame(LED_STRIP_MENU, encoder_parameters.position);
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(LED_STRIP_MENU, encoder_parameters.position);
    encoder_parameters.encoder_moved = false;
  }

  if (button_parameters.button_pressed) {
    switch (encoder_parameters.position) {
      case 0: current_menu = MAIN_MENU;                   break;
      case 1:
        if (!led_strip_parameters.enabled)
          led_strip.enable();
        else
          led_strip.disable();
        break;
      case 2: current_menu = LED_STRIP_BRIGHTNESS_MENU;   break;

      default: current_menu = STARTUP;
    }

    if (encoder_parameters.position == 0 || encoder_parameters.position == 3)
      menu_just_changed = true;
    time_since_menu_last_changed = millis();
    button_parameters.button_pressed = false;
  }
}


//level 3 menus
void Menu::display_text_size_menu() {
  current_menu = TEXT_SIZE_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.set_encoder_position(text_parameters[obj_selected].text_size);
    coms_serial.send_menu_frame(TEXT_SIZE_MENU, encoder_parameters.position);
  }

  if (button_parameters.button_pressed) {
    current_menu = TEXT_SETTINGS_MENU;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(1);
  }

  if (encoder_parameters.encoder_moved) {
    text_parameters[obj_selected].text_size = encoder_parameters.position;
    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();
    for (int i = 0; i < NUM_SCREENS; i++)
      coms_serial.send_specific_calibration_data(PREFIX_TEXT_SIZE_0 + obj_selected, i, false, 0); //send screen_mode update to screens
  }
}

void Menu::display_text_colour_menu() {
  current_menu = TEXT_COLOUR_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.recenter_encoder();
    coms_serial.send_menu_frame(TEXT_COLOUR_MENU, encoder_parameters.position);
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(TEXT_COLOUR_MENU, encoder_parameters.position);
    encoder_parameters.encoder_moved = false;
  }

  if (button_parameters.button_pressed) {
    switch (encoder_parameters.position) {
      case 0: current_menu = TEXT_SETTINGS_MENU; break;
      case 1: current_menu = TEXT_COLOUR_RED;    break;
      case 2: current_menu = TEXT_COLOUR_GREEN;  break;
      case 3: current_menu = TEXT_COLOUR_BLUE;   break;
      case 4: current_menu = TEXT_COLOUR_HUE;    break;
      case 5:
        if (text_parameters[obj_selected].use_hue)
          text_parameters[obj_selected].use_hue = false;
        else
          text_parameters[obj_selected].use_hue = true;
        break;

      default: current_menu = STARTUP;
    }

    menu_just_changed = true;
    time_since_menu_last_changed = millis();
    button_parameters.button_pressed = false;
  }

}

void Menu::display_scroll_speed_menu() {
  current_menu = SCROLL_SPEED_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.recenter_encoder();
    coms_serial.send_menu_frame(SCROLL_SPEED_MENU, encoder_parameters.position);
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(SCROLL_SPEED_MENU, encoder_parameters.position);
    encoder_parameters.encoder_moved = false;
  }

  if (button_parameters.button_pressed) {
    switch (encoder_parameters.position) {
      case 0: current_menu = TEXT_SETTINGS_MENU; break;
      case 1: current_menu = SCROLL_SPEED_MENU_X; break;
      case 2: current_menu = SCROLL_SPEED_MENU_Y; break;

      default: current_menu = STARTUP;
    }

    menu_just_changed = true;
    time_since_menu_last_changed = millis();
    button_parameters.button_pressed = false;
  }
}

void Menu::display_fan_speed_menu() {
  current_menu = FAN_SPEED_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.set_encoder_position(map(fan_parameters.target_speed, fan_parameters.fan_minimum, 255, 0, 100));
    coms_serial.send_menu_frame(FAN_SPEED_MENU, encoder_parameters.position);
  }

  if (button_parameters.button_pressed) {
    current_menu = FAN_SETTINGS_MENU;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(1);
  }

  if (encoder_parameters.encoder_moved) {
    fan_parameters.manual_set_value = map(encoder_parameters.position, 0, 100, fan_parameters.fan_minimum - 1, 255); //map percentage to byte, -1 to allow off state
    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();
    fan_parameters.manual = true;     //set manual mode

    // not important for running megas, just update on the megas displaying the menu
    int left_most_address_displaying_menu = (TOTAL_WIDTH / menu.get_menu_width()) - 1; //  (256/64)-1 = 3 -> (256/65)-1 = 2.9... = 2 etc
    for (int i = left_most_address_displaying_menu; i < NUM_SCREENS; i++)
      coms_serial.send_specific_calibration_data(PREFIX_FAN_SPEED, i, false, 0);  //send screen_mode update to screens
  }
}

void Menu::display_min_fan_speed_menu() {
  current_menu = MIN_FAN_SPEED_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.set_encoder_position(fan_parameters.fan_minimum);
    coms_serial.send_menu_frame(MIN_FAN_SPEED_MENU, encoder_parameters.position);
  }

  if (button_parameters.button_pressed) {
    current_menu = FAN_SETTINGS_MENU;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(4);
  }

  if (encoder_parameters.encoder_moved) {
    fan_parameters.fan_minimum = encoder_parameters.position;
    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();

    // not important for running megas, just update on the megas displaying the menu
    int left_most_address_displaying_menu = (TOTAL_WIDTH / menu.get_menu_width()) - 1; //  (256/64)-1 = 3 -> (256/65)-1 = 2.9... = 2 etc
    for (int i = left_most_address_displaying_menu; i < NUM_SCREENS; i++)
      coms_serial.send_specific_calibration_data(PREFIX_FAN_MINIMUM_SPEED, i, false, 0);
  }
}

void Menu::display_sd_folder_menu() {

  // NOTE: NOT IMPLEMENTED

  current_menu = SD_FOLDERS_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.recenter_encoder();
    coms_serial.send_menu_frame(SD_FOLDERS_MENU, encoder_parameters.position);
  }

  if (button_parameters.button_pressed) {
    current_menu = SD_CARD_MENU;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(3);
  }

  if (encoder_parameters.encoder_moved) {
    //    fan_parameters.fan_minimum = encoder_parameters.position;
    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();

    // not important for running megas, just update on the megas displaying the menu
    //    int left_most_address_displaying_menu = (TOTAL_WIDTH/menu.get_menu_width())-1;     //  (256/64)-1 = 3 -> (256/65)-1 = 2.9... = 2 etc
    //    for (int i = left_most_address_displaying_menu; i < NUM_SCREENS; i++)
    //      coms_serial.send_specific_calibration_data(PREFIX_FAN_SPEED, i, false, 0);  //send screen_mode update to screens
  }
}

void Menu::display_led_strip_brightness_menu() {
  current_menu = LED_STRIP_BRIGHTNESS_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.set_encoder_position(led_strip_parameters.target_brightness);
    coms_serial.send_menu_frame(LED_STRIP_BRIGHTNESS_MENU, encoder_parameters.position);
  }

  if (button_parameters.button_pressed) {
    current_menu = LED_STRIP_MENU;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(3);
  }

  if (encoder_parameters.encoder_moved) {
    led_strip_parameters.target_brightness = encoder_parameters.position;
    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();

    // not important for running megas, just update on the megas displaying the menu
    int left_most_address_displaying_menu = (TOTAL_WIDTH / menu.get_menu_width()) - 1; //  (256/64)-1 = 3 -> (256/65)-1 = 2.9... = 2 etc
    for (int i = left_most_address_displaying_menu; i < NUM_SCREENS; i++)
      coms_serial.send_specific_calibration_data(PREFIX_LED_STRIP_BRIGHTNESS, i, false, 0);  //send screen_mode update to screens
  }
}


// level 4
void Menu::display_text_colour_red_menu() {
  current_menu = TEXT_COLOUR_RED;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.set_encoder_position(text_parameters[obj_selected].red);
    coms_serial.send_menu_frame(TEXT_COLOUR_RED, encoder_parameters.position);
  }

  if (button_parameters.button_pressed) {
    current_menu = TEXT_COLOUR_MENU;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(1);
  }

  if (encoder_parameters.encoder_moved) {
    text_parameters[obj_selected].red = encoder_parameters.position;
    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();

    // not important for running megas, just update on the megas displaying the menu

    for (int i = 0; i < NUM_SCREENS; i++)
      coms_serial.send_specific_calibration_data(PREFIX_TEXT_COLOUR_R_0 + obj_selected, i, false, 0); //send screen_mode update to screens
  }
}

void Menu::display_text_colour_green_menu() {
  current_menu = TEXT_COLOUR_GREEN;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.set_encoder_position(text_parameters[obj_selected].green);
    coms_serial.send_menu_frame(TEXT_COLOUR_GREEN, encoder_parameters.position);
  }

  if (button_parameters.button_pressed) {
    current_menu = TEXT_COLOUR_MENU;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(2);
  }

  if (encoder_parameters.encoder_moved) {
    text_parameters[obj_selected].green = encoder_parameters.position;
    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();

    // not important for running megas, just update on the megas displaying the menu

    for (int i = 0; i < NUM_SCREENS; i++)
      coms_serial.send_specific_calibration_data(PREFIX_TEXT_COLOUR_G_0 + obj_selected, i, false, 0); //send screen_mode update to screens
  }
}

void Menu::display_text_colour_blue_menu() {

  current_menu = TEXT_COLOUR_BLUE;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.set_encoder_position(text_parameters[obj_selected].blue);
    coms_serial.send_menu_frame(TEXT_COLOUR_BLUE, encoder_parameters.position);
  }

  if (button_parameters.button_pressed) {
    current_menu = TEXT_COLOUR_MENU;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(3);
  }

  if (encoder_parameters.encoder_moved) {
    text_parameters[obj_selected].blue = encoder_parameters.position;
    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();

    for (int i = 0; i < NUM_SCREENS; i++)
      coms_serial.send_specific_calibration_data(PREFIX_TEXT_COLOUR_B_0 + obj_selected, i, false, 0); //send screen_mode update to screens
  }
}

void Menu::display_text_colour_hue_menu() {

  current_menu = TEXT_COLOUR_HUE;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.set_encoder_position(text_parameters[obj_selected].hue);
    coms_serial.send_menu_frame(TEXT_COLOUR_HUE, encoder_parameters.position);
  }

  if (button_parameters.button_pressed) {
    current_menu = TEXT_COLOUR_MENU;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(4);
  }

  if (encoder_parameters.encoder_moved) {
    text_parameters[obj_selected].hue = encoder_parameters.position * HUE_ADJUSTMENT_STEP_SIZE;
    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();

    for (int i = 0; i < NUM_SCREENS; i++) {
      coms_serial.send_specific_calibration_data(PREFIX_TEXT_HUE_MSB_0 + obj_selected, i, true, 0); //send screen_mode update to screens
      coms_serial.send_specific_calibration_data(PREFIX_TEXT_HUE_LSB_0 + obj_selected, i, false, 1); //send screen_mode update to screens
    }
  }
}

void Menu::display_text_scroll_speed_x() {
  current_menu = SCROLL_SPEED_MENU_X;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.set_encoder_position(text_cursor[obj_selected].x_pos_dir);    //will require offset of 128 when displayed for clarity
    coms_serial.send_menu_frame(SCROLL_SPEED_MENU, encoder_parameters.position);
  }

  if (button_parameters.button_pressed) {
    current_menu = SCROLL_SPEED_MENU;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(1);
  }

  if (encoder_parameters.encoder_moved) {

    text_cursor[obj_selected].x_pos_dir = encoder_parameters.position;

    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();

    // not important for running megas, just update on the megas displaying the menu
    int left_most_address_displaying_menu = (TOTAL_WIDTH / menu.get_menu_width()) - 1; //  (256/64)-1 = 3 -> (256/65)-1 = 2.9... = 2 etc
    for (int i = left_most_address_displaying_menu; i < NUM_SCREENS; i++)
      coms_serial.send_specific_calibration_data(PREFIX_TEXT_SCROLL_SPEED_X_0 + obj_selected, i, false, 0); //send screen_mode update to screens
  }
}
void Menu::display_text_scroll_speed_y() {
  current_menu = SCROLL_SPEED_MENU_Y;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.set_encoder_position(text_cursor[obj_selected].y_pos_dir);    //will require offset of 128 when displayed for clarity
    coms_serial.send_menu_frame(SCROLL_SPEED_MENU, encoder_parameters.position);
  }

  if (button_parameters.button_pressed) {
    current_menu = SCROLL_SPEED_MENU;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(2);
  }

  if (encoder_parameters.encoder_moved) {

    text_cursor[obj_selected].y_pos_dir = encoder_parameters.position;

    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();

    // not important for running megas, just update on the megas displaying the menu
    int left_most_address_displaying_menu = (TOTAL_WIDTH / menu.get_menu_width()) - 1; //  (256/64)-1 = 3 -> (256/65)-1 = 2.9... = 2 etc
    for (int i = left_most_address_displaying_menu; i < NUM_SCREENS; i++)
      coms_serial.send_specific_calibration_data(PREFIX_TEXT_SCROLL_SPEED_Y_0 + obj_selected, i, false, 0); //send screen_mode update to screens
  }
}

void Menu::check_obj_enabled() {
  byte counter = 0;
  for (byte i = 0; i < MAX_NUM_OF_TEXT_OBJECTS; i++) {
    if (text_cursor[i].object_used) {
      obj_enabled[counter] = i;
      counter++;
    }
  }
  num_obj_enabled = counter;
}

#endif  // Menu_CPP
