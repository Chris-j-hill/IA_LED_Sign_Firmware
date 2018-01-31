

#ifndef MENUS_CPP
#define MENUS_CPP

#include "Menus.h"
#include "Graphics.h"


Menu_Struct menu_parameters;

Menu_colour_struct menu_option_colour;
Menu_colour_struct title_colour;
extern Graphics graphics;
extern struct Object_Struct_Circles startup_ring;
Menu_tree_items menu_items;
byte current_menu = 0;
byte previous_menu = current_menu + 1; //force startup sequency by initialising pervious_menu!= startup
byte menu_width = DEFAULT_MENU_WIDTH;
uint16_t time_since_menu_last_changed = 0;
uint16_t startup_menu_timer = 0;
uint16_t startup_counter = 0;
bool menu_visable = false;
byte encoder_position = 0;

int Menu::init_menu_tree() {

#ifdef SKIP_INTITAL_STARTUP_SEQUENCE
  current_menu = DEFUALT_MENU;
#else
  current_menu = STARTUP;
#endif

}

void Menu::display_menu() {


  switch (current_menu) {
    case STARTUP:                     display_startup_sequence(); menu_visable = false; break;
    case DEFAULT_MENU:                default_display(); menu_visable = false; break;
    default:                          graphics.draw_background(); menu_visable = true; // menu visable, display any partially visable objects on unused parts of the screen
  }
  if (menu_visable) {
    switch (current_menu) {
      case MAIN_MENU:                   display_main_menu(); break;
      case SCREEN_MODE_MENU:            display_screen_mode_menu(); break;
      case BRIGHTNESS_MENU:             display_brightness_menu(); break;
      case TEXT_SETTINGS_MENU:          display_text_settings_menu(); break;
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

      default: current_menu = STARTUP;    //restart, run startup
    }
  }
}

void Menu::display_startup_sequence() { // startup is to draw a circle expnding from the screen global centre

  graphics.set_object_colour(STARTUP_R, STARTUP_G, STARTUP_B);

  if (current_menu != previous_menu) {
    previous_menu = current_menu;   //edge detecter
    startup_counter = 1;
    startup_menu_timer = millis();
    startup_ring.enabled = true;
  }
  if (startup_counter < STARTUP_RING_MAX_RADIUS) {} // do nothing

  //otherwise increment counter at non linear rate (based on 3rd order function)
  else if ((millis() - startup_menu_timer) > (STARTUP_RING_EXPANSION_RATE - graphics.non_linear_startup_function(startup_counter))) {
    startup_menu_timer = millis();
    startup_counter++;
  }
  startup_ring.radius = startup_counter;
  graphics.draw_ring(TOTAL_WIDTH / 2, SINGLE_MATRIX_HEIGHT, startup_ring.radius);
}

void Menu::default_display() {

  if (current_menu != previous_menu) {
    previous_menu = current_menu;
    startup_ring.enabled = false;   // likely came from startup animation, force annimation off
  }

  graphics.set_object_colour(STARTUP_R, STARTUP_G, STARTUP_B);  //draw a set of all enabled objects
  graphics.draw_objects();
  graphics.set_text_colour(STARTUP_R, STARTUP_G, STARTUP_B);
  graphics.draw_text();

}

void Menu::display_main_menu() {
  this -> display_background_text();    // check if menu covers whole area, if not display partial background
  graphics.write_title(MAIN_MENU);

  switch (encoder_position) {
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

  this -> display_background_text();
  graphics.write_title(SCREEN_MODE_MENU);

  switch (encoder_position) {
    case 0: graphics.write_menu_option(NULL_STRING,     RETURN_MENU,    SCREEN_MODE_0,   1);  break;
    case 1: graphics.write_menu_option(RETURN_MENU,     SCREEN_MODE_0,  SCREEN_MODE_1,   2);  break;
    case 2: graphics.write_menu_option(SCREEN_MODE_0,   SCREEN_MODE_1,  SCREEN_MODE_2,   2);  break;
    case 3: graphics.write_menu_option(SCREEN_MODE_1,   SCREEN_MODE_2,  SCREEN_MODE_3,   2);  break;
    case 4: graphics.write_menu_option(SCREEN_MODE_2,   SCREEN_MODE_3,  NULL_STRING,     3);  break;

  }
}


void display_brightness_menu() {}
void display_text_settings_menu() {}
void display_fan_settings_menu() {}
void display_internet_config_menu() {}
void display_SD_cards_menu() {}
void display_led_strip_menu() {}



#endif // MENUS_CPP
