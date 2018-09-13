
#ifndef Encoder_CPP
#define Encoder_CPP
#include "Arduino.h"
#include "Encoder.h"
//#include "Due.h"
#include "Menu_Tree.h"
#include "Coms.h"

#ifdef ENABLE_ENCODER
bool enable_encoder_on_startup = true;
#else
bool enable_encoder_on_startup = false;
#endif

#ifdef ENABLE_BUTTON
bool enable_button_on_startup = true;
#else
bool enable_button_on_startup = false;
#endif


extern Menu menu;
extern Menu_tree_menu_limits menu_limits;


Encoder_Struct encoder_parameters;     //create encoder struct
Button_Struct button_parameters;       //create button struct


void init_encoder_ISR() {

  pinMode(encoder_parameters.pinA, INPUT);
  pinMode(encoder_parameters.pinB, INPUT);
  attachInterrupt(encoder_parameters.pinA, update_encoder_ISR, CHANGE);
  encoder_parameters.is_attached = true;
  encoder_parameters.enabled = true;

}

void init_button_ISR() {

  pinMode(button_parameters.button_pin, INPUT_PULLUP);
  attachInterrupt(button_parameters.button_pin, update_button_ISR, RISING);
  button_parameters.is_attached = true;
  button_parameters.enabled = true;
}

void update_encoder_ISR () {
  if (encoder_parameters.enabled) {
    encoder_parameters.aVal = digitalRead(encoder_parameters.pinA);
    if (encoder_parameters.aVal != encoder_parameters.pinALast) { // Means the knob is rotating
      // if the knob is rotating, we need to determine direction
      // We do that by reading pin B.
      if (digitalRead(encoder_parameters.pinB) != encoder_parameters.aVal) {  // Means pin A Changed first - We're Rotating Clockwise
        encoder_parameters.PosCount ++;

      } else {// Otherwise B changed first and we're moving CCW

        encoder_parameters.PosCount--;

      }
      encoder_parameters.position = encoder_parameters.PosCount / (2 / encoder_parameters.sensitivity);

    }

    encoder_parameters.pinALast = encoder_parameters.aVal;
    encoder_parameters.encoder_moved_ISR = true;
  }
}

void update_button_ISR() {
  if (button_parameters.enabled) {
    if (millis() - button_parameters.last_button_pressed > button_parameters.button_press_interval) {
      Sprintln(F("Button Pressed"));
      button_parameters.last_button_pressed = millis();
      button_parameters.button_pressed_ISR = true;
    }
  }
}


byte Encoder::get_text_encoder_position(byte byte_number) {
  
  // function to return the MSB or LSB of the current encoder value to send
  // see Coms::pack_xy_coordinates for explanation of packing and int16_t as two bytes
  // less of an issue here but still take precautions, fix is light weight

  byte vals[2];
  int16_t pos = (int16_t)encoder_parameters.position;
  word_packing(vals, pos);

  if (byte_number == 1) return vals[0];
  else return vals[1];

//  if (byte_number == 1) { //looking for MSB
//
//    if (encoder_parameters.position < 0) {
//      byte neg_val = abs(encoder_parameters.position >> 8);
//      neg_val |= 0b10000000;
//      return neg_val;
//    }
//    else
//      return ((encoder_parameters.position >> 8) & 0x7F);
//
//  }
//  else if (byte_number == 2) { //LSB
//    return (encoder_parameters.position & 0xFF);
//  }
}


void Encoder::init_encoder() {
  if (enable_encoder_on_startup)
    this -> enable_encoder();

}

void Encoder::init_button() {
  if (enable_button_on_startup)
    this -> enable_button();

}

void Encoder::enable_encoder() {

  if (!encoder_parameters.enabled)
    encoder_parameters.enabled = true;

  if (!encoder_parameters.is_attached)
    init_encoder_ISR();

}

void Encoder::disable_encoder() {
  if (encoder_parameters.enabled)
    encoder_parameters.enabled = false;
}

void Encoder::enable_button() {
  if (!button_parameters.enabled)
    button_parameters.enabled = true;

  if (!button_parameters.is_attached)
    init_button_ISR();

}

void Encoder::disable_button() {
  if (button_parameters.enabled)
    button_parameters.enabled = false;
}

void Encoder::handle_interupts() {   // function to repond to an ISR.

  //ISR is short ideally meant to be a short piece of code, and can arrive at any point in the loop
  // this code forces all code that responds to an ISR based input to do so for one complete loop only, regardless of where in the loop the ISR may occur

  if (encoder_parameters.encoder_moved_ISR || encoder_parameters.encoder_moved) {   //if interrupt just happened or happened one loop ago

    if (encoder_parameters.encoder_moved_ISR) { //interrupt just happened
      encoder_parameters.encoder_moved_ISR = false;         //acknowlege interrupt just happened
      encoder_parameters.encoder_moved = true;             //set clean loop to true to allow funtions to execute next loop
    }

    else if (encoder_parameters.encoder_moved) {
      encoder_parameters.encoder_moved = false;              // functions executed, set clean loop false until next ISR
    }
  }


  if (button_parameters.button_pressed_ISR || button_parameters.button_pressed) {

    if (button_parameters.button_pressed_ISR) {
      button_parameters.button_pressed_ISR = false;
      button_parameters.button_pressed = true;
    }
    else if (button_parameters.button_pressed) {
      button_parameters.button_pressed = false;
    }
  }
}


void Encoder::recenter_encoder() {

  encoder_parameters.position = encoder_parameters.center;
  encoder_parameters.PosCount = (encoder_parameters.center << 1) * encoder_parameters.sensitivity;

}


void Encoder::set_encoder_position(int val)      // take value input and set the encoder current position to this
{
  encoder_parameters.position = val;
  encoder_parameters.PosCount = (val << 1) * encoder_parameters.sensitivity;
}

inline void Encoder::below_zero_limit() {
  set_encoder_position(0);
  encoder_parameters.encoder_moved = false;
}

inline void Encoder::above_upper_limit(int limit) {
  set_encoder_position(limit);
  encoder_parameters.encoder_moved = false;
}

void Encoder::encoder_position_limits() {
  switch (menu.get_current_menu()) {
    case MAIN_MENU:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.main_menu) above_upper_limit(menu_limits.main_menu);
      break;

    case SCREEN_MODE_MENU:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.screen_mode_menu) above_upper_limit(menu_limits.screen_mode_menu);
      break;

    case BRIGHTNESS_MENU:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.brightness_menu) above_upper_limit(menu_limits.brightness_menu);
      break;

    case TEXT_SETTINGS_MENU:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.text_settings_menu) above_upper_limit(menu_limits.text_settings_menu);
      break;

    case FAN_SETTINGS_MENU:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.fan_settings_menu) above_upper_limit(menu_limits.fan_settings_menu);
      break;

    case INTERNET_CONFIG_MENU:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.internet_config_menu) above_upper_limit(menu_limits.internet_config_menu);
      break;

    case SD_CARD_MENU:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.sd_cards_menu) above_upper_limit(menu_limits.sd_cards_menu);
      break;

    case LED_STRIP_MENU:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.led_strip_menu) above_upper_limit(menu_limits.led_strip_menu);
      break;

    case TEXT_SIZE_MENU:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.text_size_menu) above_upper_limit(menu_limits.text_size_menu);
      break;

    case TEXT_COLOUR_MENU:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.text_colour_menu) above_upper_limit(menu_limits.text_colour_menu);
      break;

    case SCROLL_SPEED_MENU:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.scroll_speed_menu) above_upper_limit(menu_limits.scroll_speed_menu);
      break;

    case FAN_SPEED_MENU:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.fan_speed_menu) above_upper_limit(menu_limits.fan_speed_menu);
      break;

    case MIN_FAN_SPEED_MENU:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.minimum_fan_speed_menu) above_upper_limit(menu_limits.minimum_fan_speed_menu);
      break;

    case SD_FOLDERS_MENU:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.SD_card_folders_menu) above_upper_limit(menu_limits.SD_card_folders_menu);
      break;

    case LED_STRIP_BRIGHTNESS_MENU:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.led_strip_brightness_menu) above_upper_limit(menu_limits.led_strip_brightness_menu);
      break;

    case TEXT_COLOUR_RED:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.text_colour_red_menu) above_upper_limit(menu_limits.text_colour_red_menu);
      break;

    case TEXT_COLOUR_GREEN:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.text_colour_green_menu) above_upper_limit(menu_limits.text_colour_green_menu);
      break;

    case TEXT_COLOUR_BLUE:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.text_colour_blue_menu) above_upper_limit(menu_limits.text_colour_blue_menu);
      break;

    case TEXT_COLOUR_HUE:
      if (encoder_parameters.position < menu_limits.text_colour_hue_min) {
        set_encoder_position(menu_limits.text_colour_hue_min);
        encoder_parameters.encoder_moved = false;
      }
      else if (encoder_parameters.position > menu_limits.text_colour_hue_max) above_upper_limit(menu_limits.text_colour_hue_max);
      break;

    case SCROLL_SPEED_MENU_X:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.scroll_speed_menu_x) above_upper_limit(menu_limits.scroll_speed_menu_x);
      break;

    case SCROLL_SPEED_MENU_Y:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.scroll_speed_menu_y) above_upper_limit(menu_limits.scroll_speed_menu_y);
      break;

    case TEXT_OBJ_SELECTION_MENU:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.select_text_obj_menu) above_upper_limit(menu_limits.select_text_obj_menu);
      break;
      
    case INTERNET_CONNECTION_MENU:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.current_network_config_menu) above_upper_limit(menu_limits.current_network_config_menu);
      break;
      
    case GIT_SETTING_MENU:
      if (encoder_parameters.position < 0) below_zero_limit();
      else if (encoder_parameters.position > menu_limits.git_settings_menu) above_upper_limit(menu_limits.git_settings_menu);
      break;
  }
}

#endif // Encoder_Cpp
