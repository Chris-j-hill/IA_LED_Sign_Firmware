

#ifndef MENUS_CPP
#define MENUS_CPP

#include "Menus.h"
#include "Graphics.h"


#ifdef USE_CUSTOM_RGB_MATRIX_LIBRARY
#include "src/customRGBMatrixPanel/customRGBmatrixPanel.h"
#else
#include "RGBmatrixPanel.h"
#endif


Menu_Struct menu_parameters;

Menu_colour_struct    menu_option_colour;
Menu_colour_struct    title_colour;
Menu_tree_items       menu_items;
Menu_Tree_Item_Lengths menu_tree_item_lengths;

extern struct Object_Struct_Circles  startup_ring;
extern struct Text_Struct text_parameters[MAX_NUM_OF_TEXT_OBJECTS];
extern struct Screen_Struct screen_parameters;
extern Graphics  graphics;
extern RGBmatrixPanel matrix;



bool menu_visible = false;

void Menu::init_menu_tree() {

#ifdef SKIP_INTITAL_STARTUP_SEQUENCE  //which menu will we start in before due tells us what to do
  current_menu = DEFAULT_MENU;
#else
  current_menu = STARTUP;
#endif

  menu_parameters.last_encoder_pos = menu_parameters.encoder_position;

}

void Menu::display_menu() {

  static byte scroll_animation_counter = 0;

  switch (current_menu) {
    case STARTUP:                     display_startup_sequence(); menu_visible = false; break;
    case DEFAULT_MENU:                default_display(); menu_visible = false; break;
    default:                          menu_visible = true; // menu visable
  }

  if (menu_visible) {
    if (menu_parameters.last_encoder_pos != menu_parameters.encoder_position && !menu_is_adjustment_menu(current_menu)) {
      //run scroll animation
//      uint32_t start_time = millis();
      if (menu_parameters.last_encoder_pos < menu_parameters.encoder_position) { //moving down the menu
        switch (current_menu) {
          case MAIN_MENU:                   scroll_main_menu(SCROLL_DOWN);                                  break;
          case SCREEN_MODE_MENU:            scroll_screen_mode_menu(SCROLL_DOWN);                           break;
          case TEXT_SETTINGS_MENU:          scroll_text_settings_menu(SCROLL_DOWN);                         break;
          case FAN_SETTINGS_MENU:           scroll_fan_settings_menu(SCROLL_DOWN);                          break;
          case INTERNET_CONFIG_MENU:        scroll_internet_config_menu(SCROLL_DOWN);                       break;
          case SD_CARD_MENU:                scroll_SD_cards_menu(SCROLL_DOWN);                              break;
          case LED_STRIP_MENU:              scroll_led_strip_menu(SCROLL_DOWN);                             break;
          case TEXT_COLOUR_MENU:            scroll_text_colour_menu(SCROLL_DOWN);                           break;
          case SCROLL_SPEED_MENU:           scroll_scroll_speed_menu(SCROLL_DOWN);                          break;
          case SD_FOLDERS_MENU:             scroll_sd_folder_menu(SCROLL_DOWN);                             break;
          case TEXT_OBJ_SELECTION_MENU:     scroll_text_obj_selection_menu(SCROLL_DOWN);                    break;

          default: current_menu = STARTUP;    //restart, run startup

        }
      }
      else { //moving up the menu

        switch (current_menu) {
          case MAIN_MENU:                   scroll_main_menu(SCROLL_UP);                                  break;
          case SCREEN_MODE_MENU:            scroll_screen_mode_menu(SCROLL_UP);                           break;
          case TEXT_SETTINGS_MENU:          scroll_text_settings_menu(SCROLL_UP);                         break;
          case FAN_SETTINGS_MENU:           scroll_fan_settings_menu(SCROLL_UP);                          break;
          case INTERNET_CONFIG_MENU:        scroll_internet_config_menu(SCROLL_UP);                       break;
          case SD_CARD_MENU:                scroll_SD_cards_menu(SCROLL_UP);                              break;
          case LED_STRIP_MENU:              scroll_led_strip_menu(SCROLL_UP);                             break;
          case TEXT_COLOUR_MENU:            scroll_text_colour_menu(SCROLL_UP);                           break;
          case SCROLL_SPEED_MENU:           scroll_scroll_speed_menu(SCROLL_UP);                          break;
          case SD_FOLDERS_MENU:             scroll_sd_folder_menu(SCROLL_UP);                             break;
          case TEXT_OBJ_SELECTION_MENU:     scroll_text_obj_selection_menu(SCROLL_UP);                    break;

          default: current_menu = STARTUP;    //restart, run startup

        }
      }
//      Serial.print("animation run time:");
//      Serial.println(millis()-start_time);
      //      matrix.setCursor(6, 16);
      //      matrix.setTextColor(matrix.Color333(7,0,0));
      //      matrix.print("animation");
      scroll_animation_counter++;
      if (scroll_animation_counter == 1) {  //extend the length of time scroll animation is dispayed
        menu_parameters.last_encoder_pos = menu_parameters.encoder_position;
        scroll_animation_counter = 0;
      }
      screen_parameters.updated =  false; //force the update_display function to run again
    }


    else {

      Serial.println("standard");
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
    case 0: graphics.write_menu_option(NULL_STRING,           RETURN_MENU,          SCREEN_MODE_MENU);      break;
    case 1: graphics.write_menu_option(RETURN_MENU,           SCREEN_MODE_MENU,     BRIGHTNESS_MENU);       break;
    case 2: graphics.write_menu_option(SCREEN_MODE_MENU,      BRIGHTNESS_MENU,      TEXT_SETTINGS_MENU);    break;
    case 3: graphics.write_menu_option(BRIGHTNESS_MENU,       TEXT_SETTINGS_MENU,   FAN_SETTINGS_MENU);     break;
    case 4: graphics.write_menu_option(TEXT_SETTINGS_MENU,    FAN_SETTINGS_MENU,    INTERNET_CONFIG_MENU);  break;
    case 5: graphics.write_menu_option(FAN_SETTINGS_MENU,     INTERNET_CONFIG_MENU, SD_CARD_MENU);          break;
    case 6: graphics.write_menu_option(INTERNET_CONFIG_MENU,  SD_CARD_MENU,         LED_STRIP_MENU);        break;
    case 7: graphics.write_menu_option(SD_CARD_MENU,          LED_STRIP_MENU,       NULL_STRING);           break;
  }

}

void Menu::display_screen_mode_menu() {

  graphics.write_title(SCREEN_MODE_MENU);

  switch (menu_parameters.encoder_position) {
    case 0: graphics.write_menu_option(NULL_STRING,     RETURN_MENU,    SCREEN_MODE_0);  break;
    case 1: graphics.write_menu_option(RETURN_MENU,     SCREEN_MODE_0,  SCREEN_MODE_1);  break;
    case 2: graphics.write_menu_option(SCREEN_MODE_0,   SCREEN_MODE_1,  SCREEN_MODE_2);  break;
    case 3: graphics.write_menu_option(SCREEN_MODE_1,   SCREEN_MODE_2,  SCREEN_MODE_3);  break;
    case 4: graphics.write_menu_option(SCREEN_MODE_2,   SCREEN_MODE_3,  NULL_STRING);    break;

  }
}

void Menu::display_text_settings_menu() {

  graphics.write_title(TEXT_SETTINGS_MENU);

  switch (menu_parameters.encoder_position) {
    case 0: graphics.write_menu_option(NULL_STRING,         RETURN_MENU,        TEXT_SIZE_MENU);    break;
    case 1: graphics.write_menu_option(RETURN_MENU,         TEXT_SIZE_MENU,     TEXT_COLOUR_MENU);  break;
    case 2: graphics.write_menu_option(TEXT_SIZE_MENU,      TEXT_COLOUR_MENU,   SCROLL_SPEED_MENU); break;
    case 3: graphics.write_menu_option(TEXT_COLOUR_MENU,    SCROLL_SPEED_MENU,  NULL_STRING);       break;
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
    case 0: graphics.write_menu_option(NULL_STRING,         RETURN_MENU,        FAN_SPEED_MENU);      break;
    case 1: graphics.write_menu_option(RETURN_MENU,         FAN_SPEED_MENU,     toggle_fan_enable);   break;
    case 2: graphics.write_menu_option(FAN_SPEED_MENU,      toggle_fan_enable,  MIN_FAN_SPEED_MENU);  break;
    case 3: graphics.write_menu_option(toggle_fan_enable,   MIN_FAN_SPEED_MENU, NULL_STRING);         break;
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
    case 0: graphics.write_menu_option(NULL_STRING,               RETURN_MENU,                toggle_led_strip_enable);     break;
    case 1: graphics.write_menu_option(RETURN_MENU,               toggle_led_strip_enable,    LED_STRIP_BRIGHTNESS_MENU);   break;
    case 2: graphics.write_menu_option(toggle_led_strip_enable,   LED_STRIP_BRIGHTNESS_MENU,  NULL_STRING);                 break;
  }
}


void Menu::display_text_obj_selection_menu() {

  graphics.write_title(TEXT_OBJ_SELECTION_MENU);

  if (menu_parameters.encoder_position == 0)
    graphics.write_menu_option(NULL_STRING, RETURN_MENU, TEXT_OBJ_0);

  else if (menu_parameters.encoder_position == 1)
    graphics.write_menu_option(RETURN_MENU, TEXT_OBJ_0, TEXT_OBJ_0 + 1);

  else if (menu_parameters.encoder_position == MAX_NUM_OF_TEXT_OBJECTS)
    graphics.write_menu_option(TEXT_OBJ_0 + (menu_parameters.encoder_position - 2), TEXT_OBJ_0 + (menu_parameters.encoder_position - 1), NULL_STRING);

  else
    graphics.write_menu_option(TEXT_OBJ_0 + (menu_parameters.encoder_position - 2), TEXT_OBJ_0 + (menu_parameters.encoder_position - 1), TEXT_OBJ_0 + (menu_parameters.encoder_position));
}


void Menu::display_text_colour_menu() {

  graphics.write_title(TEXT_COLOUR_MENU);

  byte toggle_use_hue;
  if (text_parameters[menu_parameters.obj_selected].use_hue)
    toggle_use_hue = USE_RGB;
  else
    toggle_use_hue = USE_HUE;

  switch (menu_parameters.encoder_position) {
    case 0: graphics.write_menu_option(NULL_STRING,               RETURN_MENU,                TEXT_COLOUR_RED);     break;
    case 1: graphics.write_menu_option(RETURN_MENU,               TEXT_COLOUR_RED,            TEXT_COLOUR_GREEN);   break;
    case 2: graphics.write_menu_option(TEXT_COLOUR_RED,           TEXT_COLOUR_GREEN,          TEXT_COLOUR_BLUE);    break;
    case 3: graphics.write_menu_option(TEXT_COLOUR_GREEN,         TEXT_COLOUR_BLUE,           TEXT_COLOUR_HUE);     break;
    case 4: graphics.write_menu_option(TEXT_COLOUR_BLUE,          TEXT_COLOUR_HUE,            toggle_use_hue);      break;
    case 5: graphics.write_menu_option(TEXT_COLOUR_HUE,           toggle_use_hue,             NULL_STRING);         break;
  }
}

void Menu::display_scroll_speed_menu() {

  graphics.write_title(TEXT_COLOUR_MENU);

  switch (menu_parameters.encoder_position) {
    case 0: graphics.write_menu_option(NULL_STRING,               RETURN_MENU,                SCROLL_SPEED_MENU_X);  break;
    case 1: graphics.write_menu_option(RETURN_MENU,               SCROLL_SPEED_MENU_X,        SCROLL_SPEED_MENU_Y);  break;
    case 2: graphics.write_menu_option(SCROLL_SPEED_MENU_X,       SCROLL_SPEED_MENU_Y,        NULL_STRING);          break;
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
    case 0: graphics.write_menu_option(NULL_STRING,               RETURN_MENU,                toggle_sd1_mounted);  break;
    case 1: graphics.write_menu_option(RETURN_MENU,               toggle_sd1_mounted,         toggle_sd2_mounted);  break;
    case 2: graphics.write_menu_option(toggle_sd1_mounted,        toggle_sd2_mounted,         SD_FOLDERS_MENU);     break;
    case 3: graphics.write_menu_option(toggle_sd2_mounted,        SD_FOLDERS_MENU,            NULL_STRING);         break;
  }
}


inline void Menu::display_adjustment_menu(byte menu) {
  graphics.write_title(menu);
  graphics.write_adjustment_menu(menu);
}

void Menu::display_internet_config_menu() {}


void Menu::scroll_main_menu(byte dir) {

  switch (menu_parameters.encoder_position) {
    case 0: graphics.write_menu_scrolling(NULL_STRING,           NULL_STRING,          RETURN_MENU,          SCREEN_MODE_MENU,      BRIGHTNESS_MENU,        dir);      break;
    case 1: graphics.write_menu_scrolling(NULL_STRING,           RETURN_MENU,          SCREEN_MODE_MENU,     BRIGHTNESS_MENU,       TEXT_SETTINGS_MENU,     dir);      break;
    case 2: graphics.write_menu_scrolling(RETURN_MENU,           SCREEN_MODE_MENU,     BRIGHTNESS_MENU,      TEXT_SETTINGS_MENU,    FAN_SETTINGS_MENU,      dir);      break;
    case 3: graphics.write_menu_scrolling(SCREEN_MODE_MENU,      BRIGHTNESS_MENU,      TEXT_SETTINGS_MENU,   FAN_SETTINGS_MENU,     INTERNET_CONFIG_MENU,   dir);      break;
    case 4: graphics.write_menu_scrolling(BRIGHTNESS_MENU,       TEXT_SETTINGS_MENU,   FAN_SETTINGS_MENU,    INTERNET_CONFIG_MENU,  SD_CARD_MENU,           dir);      break;
    case 5: graphics.write_menu_scrolling(TEXT_SETTINGS_MENU,    FAN_SETTINGS_MENU,    INTERNET_CONFIG_MENU, SD_CARD_MENU,          LED_STRIP_MENU,         dir);      break;
    case 6: graphics.write_menu_scrolling(FAN_SETTINGS_MENU,     INTERNET_CONFIG_MENU, SD_CARD_MENU,         LED_STRIP_MENU,        NULL_STRING,            dir);      break;
    case 7: graphics.write_menu_scrolling(INTERNET_CONFIG_MENU,  SD_CARD_MENU,         LED_STRIP_MENU,       NULL_STRING,           NULL_STRING,            dir);      break;

  }

  graphics.write_title(MAIN_MENU); //write title last, covers top portion of first row element
}


void Menu::scroll_screen_mode_menu(byte dir) {}
void Menu::scroll_text_settings_menu(byte dir) {}
void Menu::scroll_fan_settings_menu(byte dir) {}
void Menu::scroll_internet_config_menu(byte dir) {}
void Menu::scroll_SD_cards_menu(byte dir) {}
void Menu::scroll_led_strip_menu(byte dir) {}
void Menu::scroll_text_colour_menu(byte dir) {}
void Menu::scroll_scroll_speed_menu(byte dir) {}
void Menu::scroll_sd_folder_menu(byte dir) {}
void Menu::scroll_text_obj_selection_menu(byte dir) {}

inline bool Menu::menu_is_adjustment_menu(byte test_menu) {

  switch (test_menu) {
    case BRIGHTNESS_MENU:
    case TEXT_SIZE_MENU:
    case FAN_SPEED_MENU:
    case MIN_FAN_SPEED_MENU:
    case LED_STRIP_BRIGHTNESS_MENU:
    case TEXT_COLOUR_RED:
    case TEXT_COLOUR_GREEN:
    case TEXT_COLOUR_BLUE:
    case TEXT_COLOUR_HUE:
    case SCROLL_SPEED_MENU_X:
    case SCROLL_SPEED_MENU_Y:  return true;
  }
  return false;
}
#endif // MENUS_CPP
