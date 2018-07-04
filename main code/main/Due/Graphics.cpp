#ifndef Graphics_CPP
#define Graphics_CPP

#include "Graphics.h"
#include "Arduino.h"
#include "Config_Local.h"
#include "Current_control.h"
#include "Coms_Serial.h"

#include "function_declarations.h"

extern struct LDR_Struct light_sensor_parameters;
extern Light_Sensor light_sensor;
extern Coms_Serial coms_serial;
extern struct Timers timers;
Text text_parameters;
Text_cursor text_cursor;

int last_brightness_update = 0;
byte screen_mode = 0; //mode of operation on startup should be both displaying
//mode0: both on
//mode1: one side on
//mode2: both off
//mode3: other side on

char text_str[MAX_TWEET_SIZE] = "this is a test4 this is a test5 this is a test6";

extern byte screen_brightness;

volatile bool send_pos_now;





void Graphics::flip_direction() {

  text_cursor.x_pos_dir = (-1 * (text_cursor.x_pos_dir - 128)) + 128;
  text_cursor.y_pos_dir = (-1 * (text_cursor.y_pos_dir - 128)) + 128;
}  //flip the direction currently scrolling

void Graphics::update_brightness() {

  byte target_brightness = light_sensor.calculate_target_brightness();  //
  if (screen_brightness != target_brightness && (millis() - SCREEN_BRIGHTNESS_UPDATE_PERIOD) > last_brightness_update) {
    screen_brightness = target_brightness;
    for (int i = 0; i < NUM_SCREENS; i++)
      coms_serial.send_specific_calibration_data(PREFIX_SCREEN_BRIGHTNESS, i, false, 0);  //push to screens
    last_brightness_update = millis();
  }
}


void Graphics::init_cursor() {
  //    if (enable_cursor)
  //    this -> enable();
  //
  //  else
  //    this -> disable();

  attach_timer_pos_update();
  configure_limits();
  reset_position();
}

void attach_timer_pos_update() {
  //attach pos update interrupt
  if (!timers.pos_timer_attached) {
    timers.pos_timer_attached = true;       //indicate the timer is attached

    Timer3.attachInterrupt(send_pos_interrupt);   //attach ISR
    set_pos_update_frequency(text_cursor.ISR_freq);         // set the freq

    Timer3.start();
    Sprintln(F("Attached pos timer"));
  }
}

void set_pos_update_frequency(int freq) {

  if (!timers.pos_timer_attached) {
    Sprintln(F("From 'set_pos_update_frequency': trying to set frequency but timer not attached"));
  }
  else {  //all good, set freq
    Timer3.setFrequency(text_cursor.ISR_freq);   //set interval

  }
}

void send_pos_interrupt() {    // interrupt to send pos data to all megas

  //update pos

  int8_t x_incr = text_cursor.x_pos_dir - 128;
  int8_t y_incr = text_cursor.y_pos_dir - 128;

  x_incr = round(x_incr * text_cursor.ISR_freq * XY_SPEED_UNITS);
  y_incr = round(y_incr * text_cursor.ISR_freq * XY_SPEED_UNITS);

  //increment cursor in correct direction
  if (text_cursor.x_start_set && text_cursor.x_end_set) {
    x_incr = abs(x_incr);
    if (text_cursor.x_start > text_cursor.x_end)
      text_cursor.x -= x_incr;
    else if (text_cursor.x_start < text_cursor.x_end)
      text_cursor.x += x_incr;
    else
      text_cursor.x = text_cursor.x_start;

    //overflow value
    if (text_cursor.x_start != text_cursor.x_end) {
      if (text_cursor.x_start < text_cursor.x_end && text_cursor.x > text_cursor.x_end)
        text_cursor.x = text_cursor.x_start;
      if (text_cursor.x_start > text_cursor.x_end && text_cursor.x < text_cursor.x_end)
        text_cursor.x = text_cursor.x_start;
    }
  }
  else {
    text_cursor.x += x_incr;
    if (text_cursor.x > text_cursor.x_limit_max && x_incr > 0) {
      text_cursor.x = text_cursor.x_limit_min;
    }
    else if (text_cursor.x < text_cursor.x_limit_min && x_incr < 0) {
      text_cursor.x = text_cursor.x_limit_max;
    }
  }


  if (text_cursor.y_start_set && text_cursor.y_end_set) {
    y_incr = abs(y_incr);
    if (text_cursor.y_start > text_cursor.y_end)
      text_cursor.y -= y_incr;
    else if (text_cursor.y_start < text_cursor.y_end)
      text_cursor.y += x_incr;
    else
      text_cursor.y = text_cursor.y_start;

    //overflow value
    if (text_cursor.y_start != text_cursor.y_end) {
      if (text_cursor.y_start < text_cursor.y_end && text_cursor.y > text_cursor.y_end)
        text_cursor.y = text_cursor.y_start;
      if (text_cursor.y_start > text_cursor.y_end && text_cursor.y < text_cursor.y_end)
        text_cursor.y = text_cursor.y_start;
    }
  }
  else {
    text_cursor.y += y_incr;
    if (text_cursor.y > text_cursor.y_limit_max && y_incr > 0) {
      text_cursor.y = text_cursor.y_limit_min;
    }
    else if (text_cursor.y < text_cursor.y_limit_min && y_incr < 0) {
      text_cursor.y = text_cursor.y_limit_max;
    }
  }

  send_pos_now = true;
}

void Graphics::configure_limits() {

  if (text_cursor.x_start_set && text_cursor.x_end_set) {
    if (text_cursor.x_end > text_cursor.x_start) {
      text_cursor.x_limit_max = text_cursor.x_end;
      text_cursor.x_limit_min = text_cursor.x_start;
    }
    else if (text_cursor.x_end < text_cursor.x_start) {
      text_cursor.x_limit_max = text_cursor.x_start;
      text_cursor.x_limit_min = text_cursor.x_end;
    }
    else
      text_cursor.x_limit_max = text_cursor.x_limit_min = text_cursor.x_start;
  }

  else {
    text_cursor.x_limit_min = -1 * (text_parameters.text_size * text_parameters.text_width * text_parameters.text_str_length);
    text_cursor.x_limit_max = TOTAL_WIDTH;
  }

  if (text_cursor.y_start_set && text_cursor.y_end_set) {
    if (text_cursor.y_end > text_cursor.y_start) {
      text_cursor.y_limit_max = text_cursor.y_end;
      text_cursor.y_limit_min = text_cursor.y_start;
    }
    else if (text_cursor.y_end < text_cursor.y_start) {
      text_cursor.y_limit_max = text_cursor.y_start;
      text_cursor.y_limit_min = text_cursor.y_end;
    }
    else
      text_cursor.y_limit_max = text_cursor.y_limit_min = text_cursor.y_start;

  }
  else {
    text_cursor.y_limit_min = -1 * (text_parameters.text_size * text_parameters.text_height);
    text_cursor.y_limit_max = SINGLE_MATRIX_HEIGHT;
  }

}

void Graphics::reset_position() {


  if (text_cursor.x_start_set && text_cursor.x_end_set)
    text_cursor.x = text_cursor.x_start;
  else {
    if (text_cursor.x_pos_dir >= 0)
      text_cursor.x = text_cursor.x_limit_min;
    else
      text_cursor.x = text_cursor.x_limit_max;
  }

  if (text_cursor.y_start_set && text_cursor.y_end_set)
    text_cursor.y = text_cursor.y_start;
  else {
    if (text_cursor.y_pos_dir >= 0)
      text_cursor.y = text_cursor.y_limit_min;
    else
      text_cursor.y = text_cursor.y_limit_max;
  }
}

#endif  // Graphics_CPP
