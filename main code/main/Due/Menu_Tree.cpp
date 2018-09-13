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

  if ((current_menu != DEFAULT_MENU && current_menu != STARTUP) && (millis() > time_since_menu_last_changed + MENU_VISABLITIY_TIMOUT)) {
    previous_menu = current_menu;
    current_menu = DEFAULT_MENU;
    encoder.recenter_encoder();
    Serial.println("menu timeout");
    coms_serial.send_menu_frame(DEFAULT_MENU);
  }


  switch (current_menu) {
    case STARTUP:                     display_startup_sequence();           break;
    case DEFAULT_MENU:                default_display();                    break;
    case MAIN_MENU:                   display_main_menu();                  break;
    case SCREEN_MODE_MENU:            display_screen_mode_menu();           break;
    case BRIGHTNESS_MENU:             display_screen_brightness_menu();     break;
    case TEXT_SETTINGS_MENU:          display_text_settings_menu();         break;
    case TEXT_OBJ_SELECTION_MENU:     display_text_obj_selection_menu();    break;
    case FAN_SETTINGS_MENU:           display_fan_settings_menu();          break;
    case INTERNET_CONFIG_MENU:        display_internet_config();            break;
    case SD_CARD_MENU:                display_SD_cards_menu();              break;
    case LED_STRIP_MENU:              display_led_strip_menu();             break;
    case TEXT_SIZE_MENU:              display_text_size_menu();             break;
    case TEXT_COLOUR_MENU:            display_text_colour_menu();           break;
    case SCROLL_SPEED_MENU:           display_scroll_speed_menu();          break;
    case FAN_SPEED_MENU:              display_fan_speed_menu();             break;
    case MIN_FAN_SPEED_MENU:          display_min_fan_speed_menu();         break;
    case SD_FOLDERS_MENU:             display_sd_folder_menu();             break;
    case LED_STRIP_BRIGHTNESS_MENU:   display_led_strip_brightness_menu();  break;
    case TEXT_COLOUR_RED:             display_text_colour_red_menu();       break;
    case TEXT_COLOUR_GREEN:           display_text_colour_green_menu();     break;
    case TEXT_COLOUR_BLUE:            display_text_colour_blue_menu();      break;
    case SCROLL_SPEED_MENU_X:         display_text_scroll_speed_x();        break;
    case SCROLL_SPEED_MENU_Y:         display_text_scroll_speed_y();        break;
    case INTERNET_CONNECTION_MENU:    display_current_network_config();     break;
    case GIT_SETTING_MENU:            display_git_menu();                   break;

    default: current_menu = STARTUP;    //restart, run startup
  }
}

void Menu::display_startup_sequence() {

  static bool get_new_start_time = true;

  if (encoder_parameters.encoder_moved) {} //disregard encoder and button for this

  if (button_parameters.button_pressed) {}

  if (get_new_start_time) {
    time_since_menu_startup_run = millis();  //log time this function was run
    coms_serial.send_menu_frame(STARTUP);
    get_new_start_time = false;
  }

  else if (millis() > TIME_TO_DISPLAY_STARTUP + time_since_menu_startup_run) {

    current_menu = DEFAULT_MENU;  // force this as next menu
    get_new_start_time = true; //next time we run this it will start delay period again
    menu_just_changed = true;
  }
  else {
    encoder_parameters.encoder_moved = false;   //ignore button presses and encoder input before startup finished
    button_parameters.button_pressed = false;
  }
}

void Menu::default_display() {

  current_menu = DEFAULT_MENU;  //return to this menu until current_menu changed

  if (menu_just_changed) {
    encoder.recenter_encoder();
    coms_serial.send_menu_frame(DEFAULT_MENU);
    menu_just_changed = false;
  }

  if (button_parameters.button_pressed) {
    current_menu = MAIN_MENU;
    previous_menu = DEFAULT_MENU;
    menu_just_changed = true;
    encoder.recenter_encoder();
    time_since_menu_last_changed = millis();
    button_parameters.button_pressed = false;
  }
}

void Menu::display_main_menu() {
  current_menu = MAIN_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    if (previous_menu == DEFAULT_MENU)
      encoder.recenter_encoder();
    coms_serial.send_menu_frame(MAIN_MENU);
    Serial.println("entered main menu");
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(MAIN_MENU);
    encoder_parameters.encoder_moved = false;
  }

  if (button_parameters.button_pressed) {
    switch (encoder_parameters.position) {
      case 0: current_menu = DEFAULT_MENU;            break;
      case 1: current_menu = SCREEN_MODE_MENU;        break;
      case 2: current_menu = BRIGHTNESS_MENU;         break;
      case 3: current_menu = TEXT_OBJ_SELECTION_MENU; break;
      case 4: current_menu = FAN_SETTINGS_MENU;       break;
      case 5: current_menu = INTERNET_CONFIG_MENU;    break;
      case 6: current_menu = SD_CARD_MENU;            break;
      case 7: current_menu = LED_STRIP_MENU;          break;

      default: current_menu = STARTUP;
    }
    previous_menu = MAIN_MENU;
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
    coms_serial.send_menu_frame(SCREEN_MODE_MENU);
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(SCREEN_MODE_MENU);
    encoder_parameters.encoder_moved = false;
  }

  if (button_parameters.button_pressed) {
    switch (encoder_parameters.position) {
      case 0:
        current_menu = MAIN_MENU;
        previous_menu = SCREEN_MODE_MENU;
        menu_just_changed = true;
        break;    //return
      case 1: screen_mode = 0 ;           break;
      case 2: screen_mode = 1 ;           break;
      case 3: screen_mode = 2 ;           break;
      case 4: screen_mode = 3 ;           break;

      default: current_menu = STARTUP;

    }
    if (encoder_parameters.position >= 1 && encoder_parameters.position <= 4)
      send_cailbration_data_to_megas(PREFIX_SCREEN_MODE);

    else if (encoder_parameters.position == 0)
      encoder.set_encoder_position(1);

    time_since_menu_last_changed = millis();
    button_parameters.button_pressed = false;
  }
}

void Menu::display_screen_brightness_menu() {
  current_menu = BRIGHTNESS_MENU;

  if (menu_just_changed) {

    menu_just_changed = false;
    encoder.set_encoder_position(screen_brightness);
    coms_serial.send_menu_frame(BRIGHTNESS_MENU);
    send_cailbration_data_to_megas(PREFIX_SCREEN_BRIGHTNESS);
  }

  if (button_parameters.button_pressed) {
    current_menu = MAIN_MENU;
    previous_menu = BRIGHTNESS_MENU;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(2);
  }

  if (encoder_parameters.encoder_moved) {
    screen_brightness = encoder_parameters.position;
    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();
    send_cailbration_data_to_megas(PREFIX_SCREEN_BRIGHTNESS);
  }
}

void Menu::display_text_obj_selection_menu() {
  current_menu = TEXT_OBJ_SELECTION_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.recenter_encoder();
    check_obj_enabled();    //function to count which objects are enabled and put them in array
    coms_serial.send_menu_frame(TEXT_OBJ_SELECTION_MENU);
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(TEXT_OBJ_SELECTION_MENU);
    encoder_parameters.encoder_moved = false;
  }


  if (button_parameters.button_pressed) {
    //instead of switch, auto accommodates N text objects, most options go to TEXT_SETTINGS_MENU
    if (encoder_parameters.position <= num_obj_enabled) {
      if (encoder_parameters.position == 0) {
        current_menu = MAIN_MENU;
        previous_menu = TEXT_OBJ_SELECTION_MENU;
        encoder.set_encoder_position(3);
      }
      else if (encoder_parameters.position > 0 && encoder_parameters.position <= num_obj_enabled && num_obj_enabled != 0) {
        obj_selected = obj_enabled[encoder_parameters.position - 1];
        current_menu = TEXT_SETTINGS_MENU;
        previous_menu = TEXT_OBJ_SELECTION_MENU;
      }
      coms_serial.send_menu_frame(TEXT_OBJ_SELECTION_MENU);
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
    if (previous_menu == MAIN_MENU)
      encoder.recenter_encoder();
    coms_serial.send_menu_frame(TEXT_SETTINGS_MENU);
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(TEXT_SETTINGS_MENU);
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
    previous_menu = TEXT_SETTINGS_MENU;
    menu_just_changed = true;
    time_since_menu_last_changed = millis();
    button_parameters.button_pressed = false;
  }
}

void Menu::display_fan_settings_menu() {
  current_menu = FAN_SETTINGS_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    if (previous_menu == MAIN_MENU)
      encoder.recenter_encoder();
    coms_serial.send_menu_frame(FAN_SETTINGS_MENU);
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(FAN_SETTINGS_MENU);
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

    if (encoder_parameters.position == 0 || encoder_parameters.position == 1 || encoder_parameters.position == 3) {
      menu_just_changed = true;
      previous_menu = FAN_SETTINGS_MENU;
      if (encoder_parameters.position == 0)
        encoder.set_encoder_position(4);
    }
    time_since_menu_last_changed = millis();
    button_parameters.button_pressed = false;
  }
}

void Menu::display_internet_config() {

  current_menu = INTERNET_CONFIG_MENU;
  if (menu_just_changed) {
    menu_just_changed = false;
    if (previous_menu == MAIN_MENU)
      encoder.recenter_encoder();
    coms_serial.send_menu_frame(INTERNET_CONFIG_MENU);
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(INTERNET_CONFIG_MENU);
    encoder_parameters.encoder_moved = false;
  }

  if (button_parameters.button_pressed) {
    switch (encoder_parameters.position) {
      case 0: current_menu = MAIN_MENU;                 break;
      case 1: internet.connect_to_network();            break;
      case 2: current_menu = INTERNET_CONNECTION_MENU;  break;
      case 3: internet.wifi_enable();                   break;
      case 4: internet.wifi_disable();                  break;
      case 5: current_menu = GIT_SETTING_MENU;          break;

      default: current_menu = STARTUP;
    }
    if (encoder_parameters.position == 0 || encoder_parameters.position == 2 || encoder_parameters.position == 5) {
      menu_just_changed = true;
      previous_menu = INTERNET_CONFIG_MENU;
      if (encoder_parameters.position == 0)
        encoder.set_encoder_position(5);
    }
    time_since_menu_last_changed = millis();
    button_parameters.button_pressed = false;
  }
}

void Menu::display_SD_cards_menu() {
  current_menu = SD_CARD_MENU;
  if (menu_just_changed) {
    menu_just_changed = false;
    if (previous_menu == MAIN_MENU)
      encoder.recenter_encoder();
    coms_serial.send_menu_frame(SD_CARD_MENU);
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(SD_CARD_MENU);
    encoder_parameters.encoder_moved = false;
  }

  if (button_parameters.button_pressed) {
    switch (encoder_parameters.position) {
      case 0: current_menu = MAIN_MENU;         break;
      case 1:
        if (!card1.enabled)           // if card not enabled
          card.mount_card(EXTERNAL_CARD);         // show mount card option
        else                          // otherwise
          card.safely_eject_card(EXTERNAL_CARD);  // show eject option
        break;

      case 2:
        if (!card2.enabled)
          card.mount_card(INTERNAL_CARD);
        else
          card.safely_eject_card(INTERNAL_CARD);
        break;
      case 3: current_menu = SD_FOLDERS_MENU;    break;

      default: current_menu = STARTUP;
    }

    if (encoder_parameters.position == 0 || encoder_parameters.position == 3) {
      menu_just_changed = true;
      previous_menu = SD_CARD_MENU;
      if (encoder_parameters.position == 0)
        encoder.set_encoder_position(6);
    }
    time_since_menu_last_changed = millis();
    button_parameters.button_pressed = false;
  }
}

void Menu::display_led_strip_menu() {
  current_menu = LED_STRIP_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    if (previous_menu == MAIN_MENU)
      encoder.recenter_encoder();
    coms_serial.send_menu_frame(LED_STRIP_MENU);
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(LED_STRIP_MENU);
    encoder_parameters.encoder_moved = false;
  }

  if (button_parameters.button_pressed) {
    switch (encoder_parameters.position) {
      case 0: current_menu = MAIN_MENU;        break;
      case 1:
        if (!led_strip_parameters.enabled)
          led_strip.enable();
        else
          led_strip.disable();
        break;
      case 2: current_menu = LED_STRIP_BRIGHTNESS_MENU;   break;

      default: current_menu = STARTUP;
    }

    if (encoder_parameters.position == 0 || encoder_parameters.position == 2) {
      menu_just_changed = true;
      previous_menu = LED_STRIP_MENU;
      if (encoder_parameters.position == 0)
        encoder.set_encoder_position(7);
    }

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
    coms_serial.send_menu_frame(TEXT_SIZE_MENU);
    send_cailbration_data_to_megas(PREFIX_TEXT_SIZE_0 + obj_selected);
  }

  if (button_parameters.button_pressed) {
    current_menu = TEXT_SETTINGS_MENU;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(1);
    previous_menu = TEXT_SIZE_MENU;
  }

  if (encoder_parameters.encoder_moved) {
    text_parameters[obj_selected].text_size = encoder_parameters.position;
    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();
    send_cailbration_data_to_megas(PREFIX_TEXT_SIZE_0 + obj_selected);
  }
}

void Menu::display_text_colour_menu() {
  current_menu = TEXT_COLOUR_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    if (previous_menu == TEXT_SETTINGS_MENU)
      encoder.recenter_encoder();
    coms_serial.send_menu_frame(TEXT_COLOUR_MENU);
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(TEXT_COLOUR_MENU);
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

    previous_menu = TEXT_COLOUR_MENU;
    menu_just_changed = true;

    if (encoder_parameters.position == 0)
      encoder.set_encoder_position(2);

    time_since_menu_last_changed = millis();
    button_parameters.button_pressed = false;
  }

}

void Menu::display_scroll_speed_menu() {
  current_menu = SCROLL_SPEED_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    if (previous_menu == TEXT_SETTINGS_MENU)
      encoder.recenter_encoder();
    coms_serial.send_menu_frame(SCROLL_SPEED_MENU);
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(SCROLL_SPEED_MENU);
    encoder_parameters.encoder_moved = false;
  }

  if (button_parameters.button_pressed) {
    switch (encoder_parameters.position) {
      case 0: current_menu = TEXT_SETTINGS_MENU;  break;
      case 1: current_menu = SCROLL_SPEED_MENU_X; break;
      case 2: current_menu = SCROLL_SPEED_MENU_Y; break;

      default: current_menu = STARTUP;
    }
    previous_menu = SCROLL_SPEED_MENU;
    menu_just_changed = true;

    if (encoder_parameters.position == 0)
      encoder.set_encoder_position(3);

    time_since_menu_last_changed = millis();
    button_parameters.button_pressed = false;
  }
}

void Menu::display_fan_speed_menu() {
  current_menu = FAN_SPEED_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.set_encoder_position(map(fan_parameters.target_speed, fan_parameters.fan_minimum, 255, 0, 100));
    coms_serial.send_menu_frame(FAN_SPEED_MENU);
    send_cailbration_data_to_megas(PREFIX_FAN_SPEED, left_most_address_displaying_menu);
  }

  if (button_parameters.button_pressed) {
    current_menu = FAN_SETTINGS_MENU;
    previous_menu = FAN_SPEED_MENU;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(1);
  }

  if (encoder_parameters.encoder_moved) {
    fan_parameters.manual_set_value = map(encoder_parameters.position, 0, 100, fan_parameters.fan_minimum - 1, 255); //map percentage to byte, -1 to allow off state
    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();
    fan_parameters.manual = true;     //set manual mode
    send_cailbration_data_to_megas(PREFIX_FAN_SPEED, left_most_address_displaying_menu);
  }
}

void Menu::display_min_fan_speed_menu() {
  current_menu = MIN_FAN_SPEED_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.set_encoder_position(fan_parameters.fan_minimum);
    coms_serial.send_menu_frame(MIN_FAN_SPEED_MENU);
    send_cailbration_data_to_megas(PREFIX_FAN_MINIMUM_SPEED, left_most_address_displaying_menu);
  }

  if (button_parameters.button_pressed) {
    current_menu = FAN_SETTINGS_MENU;
    previous_menu = MIN_FAN_SPEED_MENU;
    button_parameters.button_pressed = false;
    menu_just_changed = true;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(3);
  }

  if (encoder_parameters.encoder_moved) {
    fan_parameters.fan_minimum = encoder_parameters.position;
    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();
    Serial.print("min fan prefix:");
    Serial.println(PREFIX_FAN_MINIMUM_SPEED);
    send_cailbration_data_to_megas(PREFIX_FAN_MINIMUM_SPEED, left_most_address_displaying_menu);

  }
}

void Menu::display_sd_folder_menu() {

  // NOTE: NOT IMPLEMENTED

  current_menu = SD_FOLDERS_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.recenter_encoder();
    coms_serial.send_menu_frame(SD_FOLDERS_MENU);
  }

  if (button_parameters.button_pressed) {
    current_menu = SD_CARD_MENU;
    previous_menu = SD_FOLDERS_MENU;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(3);
  }

  if (encoder_parameters.encoder_moved) {
    //    fan_parameters.fan_minimum = encoder_parameters.position;
    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();

    //send_cailbration_data_to_megas(PREFIX_LED_STRIP_BRIGHTNESS, left_most_address_displaying_menu);
    //    for (int i = left_most_address_displaying_menu; i < NUM_SCREENS; i++)
    //      coms_serial.send_specific_calibration_data(PREFIX_FAN_SPEED, i, false, 0);
  }
}

void Menu::display_led_strip_brightness_menu() {
  current_menu = LED_STRIP_BRIGHTNESS_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.set_encoder_position(led_strip_parameters.target_brightness);
    coms_serial.send_menu_frame(LED_STRIP_BRIGHTNESS_MENU);
    send_cailbration_data_to_megas(PREFIX_LED_STRIP_BRIGHTNESS, left_most_address_displaying_menu);
  }

  if (button_parameters.button_pressed) {
    current_menu = LED_STRIP_MENU;
    previous_menu = LED_STRIP_BRIGHTNESS_MENU;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(2);
  }

  if (encoder_parameters.encoder_moved) {
    led_strip_parameters.target_brightness = encoder_parameters.position;
    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();
    send_cailbration_data_to_megas(PREFIX_LED_STRIP_BRIGHTNESS, left_most_address_displaying_menu);
  }
}


// level 4
void Menu::display_text_colour_red_menu() {
  current_menu = TEXT_COLOUR_RED;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.set_encoder_position(text_parameters[obj_selected].red);
    coms_serial.send_menu_frame(TEXT_COLOUR_RED);
    send_cailbration_data_to_megas(PREFIX_TEXT_COLOUR_R_0 + obj_selected);
  }

  if (button_parameters.button_pressed) {
    current_menu = TEXT_COLOUR_MENU;
    previous_menu = TEXT_COLOUR_RED;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(1);
  }

  if (encoder_parameters.encoder_moved) {
    text_parameters[obj_selected].red = encoder_parameters.position;
    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();
    send_cailbration_data_to_megas(PREFIX_TEXT_COLOUR_R_0 + obj_selected);

  }
}

void Menu::display_text_colour_green_menu() {
  current_menu = TEXT_COLOUR_GREEN;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.set_encoder_position(text_parameters[obj_selected].green);
    coms_serial.send_menu_frame(TEXT_COLOUR_GREEN);
    send_cailbration_data_to_megas(PREFIX_TEXT_COLOUR_G_0 + obj_selected);
  }

  if (button_parameters.button_pressed) {
    current_menu = TEXT_COLOUR_MENU;
    previous_menu = TEXT_COLOUR_GREEN;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(2);
  }

  if (encoder_parameters.encoder_moved) {
    text_parameters[obj_selected].green = encoder_parameters.position;
    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();
    send_cailbration_data_to_megas(PREFIX_TEXT_COLOUR_G_0 + obj_selected);
  }
}

void Menu::display_text_colour_blue_menu() {

  current_menu = TEXT_COLOUR_BLUE;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.set_encoder_position(text_parameters[obj_selected].blue);
    coms_serial.send_menu_frame(TEXT_COLOUR_BLUE);
    send_cailbration_data_to_megas(PREFIX_TEXT_COLOUR_B_0 + obj_selected);
  }

  if (button_parameters.button_pressed) {
    current_menu = TEXT_COLOUR_MENU;
    previous_menu = TEXT_COLOUR_BLUE;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(3);
  }

  if (encoder_parameters.encoder_moved) {
    text_parameters[obj_selected].blue = encoder_parameters.position;
    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();
    send_cailbration_data_to_megas(PREFIX_TEXT_COLOUR_B_0 + obj_selected);
  }
}

void Menu::display_text_colour_hue_menu() {

  current_menu = TEXT_COLOUR_HUE;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.set_encoder_position(text_parameters[obj_selected].hue);
    coms_serial.send_menu_frame(TEXT_COLOUR_HUE);
    for (byte i = 0; i < NUM_SCREENS; i++) {
      coms_serial.send_specific_calibration_data(PREFIX_TEXT_HUE_MSB_0 + obj_selected, i, true, 0);
      coms_serial.send_specific_calibration_data(PREFIX_TEXT_HUE_LSB_0 + obj_selected, i, false, 1);
    }
  }

  if (button_parameters.button_pressed) {
    current_menu = TEXT_COLOUR_MENU;
    previous_menu = TEXT_COLOUR_HUE;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(4);
  }

  if (encoder_parameters.encoder_moved) {
    text_parameters[obj_selected].hue = encoder_parameters.position * HUE_ADJUSTMENT_STEP_SIZE;
    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();

    //dont ue function here as sending two values in same frame
    for (byte i = 0; i < NUM_SCREENS; i++) {
      coms_serial.send_specific_calibration_data(PREFIX_TEXT_HUE_MSB_0 + obj_selected, i, true, 0);
      coms_serial.send_specific_calibration_data(PREFIX_TEXT_HUE_LSB_0 + obj_selected, i, false, 1);
    }
  }
}

void Menu::display_text_scroll_speed_x() {
  current_menu = SCROLL_SPEED_MENU_X;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.set_encoder_position(text_cursor[obj_selected].x_pos_dir);    //will require offset of 128 when displayed for clarity
    coms_serial.send_menu_frame(SCROLL_SPEED_MENU);
    send_cailbration_data_to_megas(PREFIX_TEXT_SCROLL_SPEED_X_0 + obj_selected, left_most_address_displaying_menu);
  }

  if (button_parameters.button_pressed) {
    current_menu = SCROLL_SPEED_MENU;
    previous_menu = SCROLL_SPEED_MENU_X;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(1);
  }

  if (encoder_parameters.encoder_moved) {

    text_cursor[obj_selected].x_pos_dir = encoder_parameters.position;

    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();

    // remember due controls speeds, objects on screen are not controlled by megas
    send_cailbration_data_to_megas(PREFIX_TEXT_SCROLL_SPEED_X_0 + obj_selected, left_most_address_displaying_menu);

  }
}
void Menu::display_text_scroll_speed_y() {
  current_menu = SCROLL_SPEED_MENU_Y;

  if (menu_just_changed) {
    menu_just_changed = false;
    encoder.set_encoder_position(text_cursor[obj_selected].y_pos_dir);    //will require offset of 128 when displayed for clarity
    coms_serial.send_menu_frame(SCROLL_SPEED_MENU);
    send_cailbration_data_to_megas(PREFIX_TEXT_SCROLL_SPEED_Y_0 + obj_selected, left_most_address_displaying_menu);

  }

  if (button_parameters.button_pressed) {
    current_menu = SCROLL_SPEED_MENU;
    previous_menu = SCROLL_SPEED_MENU_Y;
    button_parameters.button_pressed = false;
    time_since_menu_last_changed = millis();
    encoder.set_encoder_position(2);
  }

  if (encoder_parameters.encoder_moved) {

    text_cursor[obj_selected].y_pos_dir = encoder_parameters.position;

    encoder_parameters.encoder_moved = false;
    time_since_menu_last_changed = millis();

    send_cailbration_data_to_megas(PREFIX_TEXT_SCROLL_SPEED_Y_0 + obj_selected, left_most_address_displaying_menu);

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

inline void Menu::send_cailbration_data_to_megas(byte prefix, byte starting_address, byte ending_address) {
  for (byte i = starting_address; i <= ending_address; i++)
    coms_serial.send_specific_calibration_data(prefix, i, false, 0);
}




void Menu::display_current_network_config() {
  current_menu = INTERNET_CONNECTION_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    //    encoder.recenter_encoder();
    coms_serial.send_menu_frame(INTERNET_CONNECTION_MENU);
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(INTERNET_CONNECTION_MENU);
    encoder_parameters.encoder_moved = false;
  }

  if (button_parameters.button_pressed) {
    switch (encoder_parameters.position) {
      case 0: current_menu = INTERNET_CONFIG_MENU;  break;
      case 1: current_menu = CURRENT_NETWORK_MENU;  break;
      case 2: current_menu = CURRENT_IP_MENU;       break;

      default: current_menu = STARTUP;
    }
    if (encoder_parameters.position == 0) {
      menu_just_changed = true;
      previous_menu = INTERNET_CONNECTION_MENU;
      encoder.set_encoder_position(2);
    }
    time_since_menu_last_changed = millis();
    button_parameters.button_pressed = false;

  }
}

void Menu::display_git_menu() {
  current_menu = GIT_SETTING_MENU;

  if (menu_just_changed) {
    menu_just_changed = false;
    if (previous_menu == INTERNET_CONFIG_MENU)
      encoder.recenter_encoder();
    coms_serial.send_menu_frame(GIT_SETTING_MENU);
  }

  if (encoder_parameters.encoder_moved) {
    time_since_menu_last_changed = millis();
    coms_serial.send_menu_frame(GIT_SETTING_MENU);
    encoder_parameters.encoder_moved = false;
  }

  if (button_parameters.button_pressed) {
    switch (encoder_parameters.position) {
      case 0: current_menu = INTERNET_CONFIG_MENU;   break;
      case 1: current_menu = GIT_AUTHER_MENU;        break;
      case 2: current_menu = GIT_COMMIT_MSG_MENU;    break;
      case 3: current_menu = GIT_COMMIT_TAG_MENU;    break;
      case 4: internet.update_firmware();            break;

      default: current_menu = STARTUP;
    }
    if (encoder_parameters.position != 4) {
      menu_just_changed = true;
      previous_menu = GIT_SETTING_MENU;
      if (encoder_parameters.position == 0)
        encoder.set_encoder_position(5);
    }
    time_since_menu_last_changed = millis();
    button_parameters.button_pressed = false;
  }
}


void Menu::show_current_ip_address() {}
void Menu::show_current_network() {}
void Menu::show_git_commit_auther() {}
void Menu::show_git_commit_msg() {}
void Menu::show_git_commit_tag() {}

#endif  // Menu_CPP
