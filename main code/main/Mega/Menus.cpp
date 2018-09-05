

#ifndef MENUS_CPP
#define MENUS_CPP

#include "Menus.h"
#include "Graphics.h"


Menu_Struct menu_parameters;

Menu_colour_struct    menu_option_colour;
Menu_colour_struct    title_colour;
Menu_tree_items       menu_items;
Menu_Tree_Item_Lengths menu_tree_item_lengths;

extern struct Object_Struct_Circles  startup_ring;
extern struct Text_Struct text_parameters[MAX_NUM_OF_TEXT_OBJECTS];
extern Graphics  graphics;

//byte current_menu = 0;
//byte previous_menu = current_menu + 1; //force startup sequency by initialising pervious_menu!= startup
//byte menu_width = DEFAULT_MENU_WIDTH;
//uint16_t time_since_menu_last_changed = 0;
//uint16_t startup_menu_timer = 0;
//uint16_t startup_counter = 0;
bool menu_visible = false;
//byte encoder_position = 0;

void Menu::init_menu_tree() {

#ifdef SKIP_INTITAL_STARTUP_SEQUENCE
  current_menu = DEFUALT_MENU;
#else
  current_menu = STARTUP;
#endif

}

void Menu::display_menu() {


  switch (current_menu) {
    case STARTUP:                     display_startup_sequence(); menu_visible = false; break;
    case DEFAULT_MENU:                default_display(); menu_visible = false; break;
    default:                          graphics.draw_background(); menu_visible = true; // menu visable, display any partially visable objects on unused parts of the screen
  }
  if (menu_visible) {
    //clear_background_text(); // check if menu covers whole area, if not display partial background
    switch (current_menu) {
      case MAIN_MENU:                   display_main_menu();                                  break;
      case SCREEN_MODE_MENU:            display_screen_mode_menu();                           break;
      case BRIGHTNESS_MENU:             display_adjustment_menu(BRIGHTNESS_MENU);             break;
      case TEXT_SETTINGS_MENU:          display_text_settings_menu();                         break;
      case FAN_SETTINGS_MENU:           display_fan_settings_menu();                          break;
      case INTERNET_CONFIG_MENU:        display_internet_config_menu();                       break;
      case SD_CARD_MENU:                display_SD_cards_menu();                              break;
      case LED_STRIP_MENU:              display_led_strip_menu();                             break;
      case TEXT_SIZE_MENU:              display_adjustment_menu(TEXT_SIZE_MENU);              break;
      case TEXT_COLOUR_MENU:            display_text_colour_menu();                           break;
      case SCROLL_SPEED_MENU:           display_scroll_speed_menu();                          break;
      case FAN_SPEED_MENU:              display_adjustment_menu(FAN_SPEED_MENU);              break;
      case MIN_FAN_SPEED_MENU:          display_adjustment_menu(MIN_FAN_SPEED_MENU);          break;
      case SD_FOLDERS_MENU:             display_sd_folder_menu();                             break;
      case LED_STRIP_BRIGHTNESS_MENU:   display_adjustment_menu(LED_STRIP_BRIGHTNESS_MENU);   break;
      case TEXT_COLOUR_RED:             display_adjustment_menu(TEXT_COLOUR_RED);             break;
      case TEXT_COLOUR_GREEN:           display_adjustment_menu(TEXT_COLOUR_GREEN);           break;
      case TEXT_COLOUR_BLUE:            display_adjustment_menu(TEXT_COLOUR_BLUE);            break;
      case TEXT_COLOUR_HUE:             display_adjustment_menu(TEXT_COLOUR_HUE);             break;
      case SCROLL_SPEED_MENU_X:         display_adjustment_menu(SCROLL_SPEED_MENU_X);         break;
      case SCROLL_SPEED_MENU_Y:         display_adjustment_menu(SCROLL_SPEED_MENU_Y);         break;

      case TEXT_OBJ_SELECTION_MENU:     display_text_obj_selection_menu();                    break;

      default: current_menu = STARTUP;    //restart, run startup
    }
  }
}

void Menu::display_startup_sequence() { // startup is to draw a circle expnding from the screen global centre

//  graphics.set_object_colour(STARTUP_R, STARTUP_G, STARTUP_B);

  if (current_menu != previous_menu) {
    previous_menu = current_menu;   //edge detecter
//    startup_counter = 1;
//    startup_menu_timer = millis();
//    startup_ring.enabled = true;
  }
//  if (startup_counter < STARTUP_RING_MAX_RADIUS) {} // do nothing
//
//  //otherwise increment counter at non linear rate (based on 3rd order function)
//  else if ((millis() - startup_menu_timer) > (STARTUP_RING_EXPANSION_RATE - graphics.non_linear_startup_function(startup_counter))) {
//    startup_menu_timer = millis();
//    startup_counter++;
//  }
//  startup_ring.radius = startup_counter;
//  graphics.draw_ring(TOTAL_WIDTH / 2, SINGLE_MATRIX_HEIGHT, startup_ring.radius);
}

void Menu::default_display() {
//not much to do here, just edge detection to set variables
  if (current_menu != previous_menu) {
    previous_menu = current_menu;
    startup_ring.enabled = false;   // likely came from startup animation, force annimation off
  }
}

void Menu::display_main_menu() {

  graphics.write_title(MAIN_MENU);

  switch (menu_parameters.encoder_position) {
    case 0: graphics.write_menu_option(NULL_STRING,           RETURN_MENU,          SCREEN_MODE_MENU,     1);  break;
    case 1: graphics.write_menu_option(RETURN_MENU,           SCREEN_MODE_MENU,     BRIGHTNESS_MENU,      2);  break;
    case 2: graphics.write_menu_option(SCREEN_MODE_MENU,      BRIGHTNESS_MENU,      TEXT_SETTINGS_MENU,   2);  break;
    case 3: graphics.write_menu_option(BRIGHTNESS_MENU,       TEXT_SETTINGS_MENU,   FAN_SETTINGS_MENU,    2);  break;
    case 4: graphics.write_menu_option(TEXT_SETTINGS_MENU,    FAN_SETTINGS_MENU,    INTERNET_CONFIG_MENU, 2);  break;
    case 5: graphics.write_menu_option(FAN_SETTINGS_MENU,     INTERNET_CONFIG_MENU, SD_CARD_MENU,         2);  break;
    case 6: graphics.write_menu_option(INTERNET_CONFIG_MENU,  SD_CARD_MENU,         LED_STRIP_MENU,       2);  break;
    case 7: graphics.write_menu_option(SD_CARD_MENU,          LED_STRIP_MENU,       NULL_STRING,          3);  break;
  }

}

void Menu::display_screen_mode_menu() {

  graphics.write_title(SCREEN_MODE_MENU);

  switch (menu_parameters.encoder_position) {
    case 0: graphics.write_menu_option(NULL_STRING,     RETURN_MENU,    SCREEN_MODE_0,   1);  break;
    case 1: graphics.write_menu_option(RETURN_MENU,     SCREEN_MODE_0,  SCREEN_MODE_1,   2);  break;
    case 2: graphics.write_menu_option(SCREEN_MODE_0,   SCREEN_MODE_1,  SCREEN_MODE_2,   2);  break;
    case 3: graphics.write_menu_option(SCREEN_MODE_1,   SCREEN_MODE_2,  SCREEN_MODE_3,   2);  break;
    case 4: graphics.write_menu_option(SCREEN_MODE_2,   SCREEN_MODE_3,  NULL_STRING,     3);  break;

  }
}

void Menu::display_text_settings_menu() {

  graphics.write_title(TEXT_SETTINGS_MENU);

  switch (menu_parameters.encoder_position) {
    case 0: graphics.write_menu_option(NULL_STRING,         RETURN_MENU,        TEXT_SIZE_MENU,       1);  break;
    case 1: graphics.write_menu_option(RETURN_MENU,         TEXT_SIZE_MENU,     TEXT_COLOUR_MENU,     2);  break;
    case 2: graphics.write_menu_option(TEXT_SIZE_MENU,      TEXT_COLOUR_MENU,   SCROLL_SPEED_MENU,    2);  break;
    case 3: graphics.write_menu_option(TEXT_COLOUR_MENU,    SCROLL_SPEED_MENU,  NULL_STRING,          3);  break;
  }
}


void Menu::display_fan_settings_menu() {

  graphics.write_title(FAN_SETTINGS_MENU);

  byte toggle_fan_enable;
  if (menu_parameters.fan_enabled)
    toggle_fan_enable = ENABLE_FAN;
  else
    toggle_fan_enable = DISABLE_FAN;

  switch (menu_parameters.encoder_position) {
    case 0: graphics.write_menu_option(NULL_STRING,         RETURN_MENU,        FAN_SPEED_MENU,       1);  break;
    case 1: graphics.write_menu_option(RETURN_MENU,         FAN_SPEED_MENU,     toggle_fan_enable,    2);  break;
    case 2: graphics.write_menu_option(FAN_SPEED_MENU,      toggle_fan_enable,  MIN_FAN_SPEED_MENU,   2);  break;
    case 3: graphics.write_menu_option(toggle_fan_enable,   MIN_FAN_SPEED_MENU, NULL_STRING,          3);  break;
  }
}


void Menu::display_led_strip_menu() {

  graphics.write_title(LED_STRIP_MENU);

  byte toggle_led_strip_enable;
  if (menu_parameters.led_strip_enabled)
    toggle_led_strip_enable = ENABLE_LED_STRIP;
  else
    toggle_led_strip_enable = DISABLE_LED_STRIP;

  switch (menu_parameters.encoder_position) {
    case 0: graphics.write_menu_option(NULL_STRING,               RETURN_MENU,                toggle_led_strip_enable,      1);  break;
    case 1: graphics.write_menu_option(RETURN_MENU,               toggle_led_strip_enable,    LED_STRIP_BRIGHTNESS_MENU,    2);  break;
    case 2: graphics.write_menu_option(toggle_led_strip_enable,   LED_STRIP_BRIGHTNESS_MENU,  NULL_STRING,                  3);  break;
  }
}


void Menu::display_text_obj_selection_menu() {

  graphics.write_title(TEXT_OBJ_SELECTION_MENU);

  if (menu_parameters.encoder_position == 0)
    graphics.write_menu_option(NULL_STRING, RETURN_MENU, TEXT_OBJ_0, 1);

  else if (menu_parameters.encoder_position == 1)
    graphics.write_menu_option(RETURN_MENU, TEXT_OBJ_0, TEXT_OBJ_0 + 1, 2);

  else if (menu_parameters.encoder_position == MAX_NUM_OF_TEXT_OBJECTS)
    graphics.write_menu_option(TEXT_OBJ_0 + (menu_parameters.encoder_position - 2), TEXT_OBJ_0 + (menu_parameters.encoder_position - 1), NULL_STRING, 3);

  else
    graphics.write_menu_option(TEXT_OBJ_0 + (menu_parameters.encoder_position - 2), TEXT_OBJ_0 + (menu_parameters.encoder_position - 1), TEXT_OBJ_0 + (menu_parameters.encoder_position), 2);
}


void Menu::display_text_colour_menu() {

  graphics.write_title(TEXT_COLOUR_MENU);

  byte toggle_use_hue;
  if (text_parameters[menu_parameters.obj_selected].use_hue)
    toggle_use_hue = USE_RGB;
  else
    toggle_use_hue = USE_HUE;

  switch (menu_parameters.encoder_position) {
    case 0: graphics.write_menu_option(NULL_STRING,               RETURN_MENU,                TEXT_COLOUR_RED,      1);  break;
    case 1: graphics.write_menu_option(RETURN_MENU,               TEXT_COLOUR_RED,            TEXT_COLOUR_GREEN,    2);  break;
    case 2: graphics.write_menu_option(TEXT_COLOUR_RED,           TEXT_COLOUR_GREEN,          TEXT_COLOUR_BLUE,     2);  break;
    case 3: graphics.write_menu_option(TEXT_COLOUR_GREEN,         TEXT_COLOUR_BLUE,           TEXT_COLOUR_HUE,      2);  break;
    case 4: graphics.write_menu_option(TEXT_COLOUR_BLUE,          TEXT_COLOUR_HUE,            toggle_use_hue,       2);  break;
    case 5: graphics.write_menu_option(TEXT_COLOUR_HUE,           toggle_use_hue,             NULL_STRING,          3);  break;
  }
}

void Menu::display_scroll_speed_menu() {

  graphics.write_title(TEXT_COLOUR_MENU);

  switch (menu_parameters.encoder_position) {
    case 0: graphics.write_menu_option(NULL_STRING,               RETURN_MENU,                SCROLL_SPEED_MENU_X,      1);  break;
    case 1: graphics.write_menu_option(RETURN_MENU,               SCROLL_SPEED_MENU_X,        SCROLL_SPEED_MENU_Y,      2);  break;
    case 2: graphics.write_menu_option(SCROLL_SPEED_MENU_X,       SCROLL_SPEED_MENU_Y,        NULL_STRING,              3);  break;
  }
}

void Menu::display_SD_cards_menu() {

  graphics.write_title(SD_CARD_MENU);

  byte toggle_sd1_mounted;
  byte toggle_sd2_mounted;

  if (menu_parameters.sd_card1_mounted)
    toggle_sd1_mounted = UNMOUNT_CARD1;
  else
    toggle_sd1_mounted = MOUNT_CARD1;

  if (menu_parameters.sd_card2_mounted)
    toggle_sd2_mounted = UNMOUNT_CARD2;
  else
    toggle_sd2_mounted = MOUNT_CARD2;

  switch (menu_parameters.encoder_position) {
    case 0: graphics.write_menu_option(NULL_STRING,               RETURN_MENU,                toggle_sd1_mounted,      1);  break;
    case 1: graphics.write_menu_option(RETURN_MENU,               toggle_sd1_mounted,         toggle_sd2_mounted,      2);  break;
    case 2: graphics.write_menu_option(toggle_sd1_mounted,        toggle_sd2_mounted,         SD_FOLDERS_MENU,         2);  break;
    case 3: graphics.write_menu_option(toggle_sd2_mounted,        SD_FOLDERS_MENU,            NULL_STRING,             3);  break;
  }
}


inline void Menu::display_adjustment_menu(byte menu) {
  graphics.write_title(menu);
  graphics.write_adjustment_menu(menu);
}

void Menu::display_internet_config_menu() {}

#endif // MENUS_CPP
