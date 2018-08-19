#ifndef Graphics_CPP
#define Graphics_CPP


#include "Due_Libraries.h"
#include "Graphics.h"
#include "Arduino.h"
#include "Config_Local.h"
#include "Current_control.h"
#include "Coms_Serial.h"
#include "SD_Cards.h"

#include "function_declarations.h"

extern struct LDR_Struct light_sensor_parameters;
extern Light_Sensor light_sensor;
extern Coms_Serial coms_serial;
extern Graphics graphics;
extern struct Timers timers;
Text text_parameters[MAX_NUM_OF_TEXT_OBJECTS];
Text_cursor text_cursor[MAX_NUM_OF_TEXT_OBJECTS];

byte screen_mode = 0; //mode of operation on startup should be both displaying
//mode0: both on
//mode1: one side on
//mode2: both off
//mode3: other side on

//some default text strings
#if MAX_NUM_OF_TEXT_OBJECTS == 5
char text_str[MAX_NUM_OF_TEXT_OBJECTS][MAX_TWEET_SIZE] = {{"this is a test0"}, {"this is a test1"}, {"this is a test2"}, {"this is a test3"}, {"this is a test4"}};
#elif MAX_NUM_OF_TEXT_OBJECTS == 4
char text_str[MAX_NUM_OF_TEXT_OBJECTS][MAX_TWEET_SIZE] = {{"this is a test0"}, {"this is a test1"}, {"this is a test2"}, {"this is a test3"}};
#elif MAX_NUM_OF_TEXT_OBJECTS == 3
char text_str[MAX_NUM_OF_TEXT_OBJECTS][MAX_TWEET_SIZE] = {{"this is a test0"}, {"this is a test1"}, {"this is a test2"}};
#elif MAX_NUM_OF_TEXT_OBJECTS == 2
char text_str[MAX_NUM_OF_TEXT_OBJECTS][MAX_TWEET_SIZE] = {{"this is a test0"}, {"this is a test1"}};
#elif MAX_NUM_OF_TEXT_OBJECTS == 1
char text_str[MAX_NUM_OF_TEXT_OBJECTS][MAX_TWEET_SIZE] = {{"this is a test0"}};
#endif


extern byte screen_brightness;

volatile bool send_pos_now;

extern struct SD_Strings SD_string;
extern Card card;
bool get_new_config[MAX_NUM_OF_TEXT_OBJECTS] = {false};


void Graphics::update_brightness() {
  static uint32_t last_brightness_update = millis();
  byte target_brightness = light_sensor.calculate_target_brightness();
  if (screen_brightness != target_brightness && (millis() - SCREEN_BRIGHTNESS_UPDATE_PERIOD) > last_brightness_update) {
    screen_brightness = target_brightness;
    for (int i = 0; i < NUM_SCREENS; i++)
#ifdef ALLOW_SCREEN_BRIGHTNESS_AUTO_ADJUSTMENT
      coms_serial.send_specific_calibration_data(PREFIX_SCREEN_BRIGHTNESS, i, false, 0);  //push to screens
#endif
    last_brightness_update = millis();
  }
}

void dummy_text_config() {    //use this to enable the pos frame and cursor positions
  text_cursor[0].object_used = true;
  text_cursor[1].object_used = true;
  text_parameters[0].text_size = 1;
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
#ifdef RUN_TEXT_CONFIG
  dummy_text_config();
#endif
}


void attach_timer_pos_update() {
  //attach pos update interrupt
  if (!timers.pos_timer_attached) {
    timers.pos_timer_attached = true;       //indicate the timer is attached

    POS_TIMER.attachInterrupt(send_pos_interrupt);   //attach ISR
    set_pos_update_frequency(POS_UPDATE_ISR_FREQ);         // set the freq

    POS_TIMER.start();
    NVIC_SetPriority (POS_TIMER_INTERRUPT, POS_FRAME_PRIORITY);  //set priority of interrupt, see priority definitions for details and links

  }
}

void set_pos_update_frequency(byte freq) {

  if (!timers.pos_timer_attached) {
    Sprintln(F("From 'set_pos_update_frequency': trying to set frequency but timer not attached"));
  }
  else {  //all good, set freq
    POS_TIMER.setFrequency(freq);   //set interval

  }
}

void send_pos_interrupt() {    // interrupt to send pos data to all megas

  //update pos for all objects

  static uint16_t loops_since_overflow_x[MAX_NUM_OF_TEXT_OBJECTS] = {0};
  static uint16_t loops_since_overflow_y[MAX_NUM_OF_TEXT_OBJECTS] = {0};

  for (byte i = 0; i < MAX_NUM_OF_TEXT_OBJECTS; i++) {
    if (text_cursor[i].object_used) {
      int8_t x_incr = text_cursor[i].x_pos_dir - 128;
      int8_t y_incr = text_cursor[i].y_pos_dir - 128;


      double temp = text_cursor[i].ISR_freq * XY_SPEED_UNITS;
      x_incr = round(x_incr * temp);
      y_incr = round(y_incr * temp);

      //increment cursor in correct direction based on start and end points
      if (text_cursor[i].x_start_set && text_cursor[i].x_end_set) {
        x_incr = abs(x_incr);
        if (text_cursor[i].x_start > text_cursor[i].x_end)
          text_cursor[i].x -= x_incr;
        else if (text_cursor[i].x_start < text_cursor[i].x_end)
          text_cursor[i].x += x_incr;
        else
          text_cursor[i].x = text_cursor[i].x_start;

        //overflow value
        if (text_cursor[i].x_start != text_cursor[i].x_end) {
          if (text_cursor[i].x_start < text_cursor[i].x_end && text_cursor[i].x > text_cursor[i].x_end) {
            text_cursor[i].x = text_cursor[i].x_start;
            loops_since_overflow_x[i] ++;
          }
          if (text_cursor[i].x_start > text_cursor[i].x_end && text_cursor[i].x < text_cursor[i].x_end) {
            text_cursor[i].x = text_cursor[i].x_start;
            loops_since_overflow_x[i] ++;
          }
        }
      }
      // increment based on direction alone
      else {
        text_cursor[i].x += x_incr;
        if (text_cursor[i].x > text_cursor[i].x_limit_max && x_incr > 0) {
          text_cursor[i].x = text_cursor[i].x_limit_min;
        }
        else if (text_cursor[i].x < text_cursor[i].x_limit_min && x_incr < 0) {
          text_cursor[i].x = text_cursor[i].x_limit_max;
          loops_since_overflow_x[i] ++;
        }
      }


      if (text_cursor[i].y_start_set && text_cursor[i].y_end_set) {
        y_incr = abs(y_incr);
        if (text_cursor[i].y_start > text_cursor[i].y_end)
          text_cursor[i].y -= y_incr;
        else if (text_cursor[i].y_start < text_cursor[i].y_end)
          text_cursor[i].y += x_incr;
        else
          text_cursor[i].y = text_cursor[i].y_start;

        //overflow value
        if (text_cursor[i].y_start != text_cursor[i].y_end) {
          if (text_cursor[i].y_start < text_cursor[i].y_end && text_cursor[i].y > text_cursor[i].y_end) {
            text_cursor[i].y = text_cursor[i].y_start;
            loops_since_overflow_y[i] ++;
          }
          if (text_cursor[i].y_start > text_cursor[i].y_end && text_cursor[i].y < text_cursor[i].y_end) {
            text_cursor[i].y = text_cursor[i].y_start;
            loops_since_overflow_y[i] ++;
          }
        }
      }
      else {
        text_cursor[i].y += y_incr;
        if (text_cursor[i].y > text_cursor[i].y_limit_max && y_incr > 0) {
          text_cursor[i].y = text_cursor[i].y_limit_min;
          loops_since_overflow_y[i] ++;
        }
        else if (text_cursor[i].y < text_cursor[i].y_limit_min && y_incr < 0) {
          text_cursor[i].y = text_cursor[i].y_limit_max;
          loops_since_overflow_y[i] ++;
        }
      }

      //if the next file exists and we've reached an overflow limit or time limit of this file get new file data
      if (text_cursor[i].check_for_new_file) {
        if ((text_cursor[i].found_loops_x && loops_since_overflow_x[i] >= text_cursor[i].loops_x) || (text_cursor[i].found_loops_y && loops_since_overflow_y[i] >= text_cursor[i].loops_y) || (text_cursor[i].found_time && millis() > text_cursor[i].change_file_timeout + text_cursor[i].str_disp_time)) {

          get_new_config[i] = true;

          loops_since_overflow_x[i] = 0;
          loops_since_overflow_y[i] = 0;

        }
      }
#ifndef DISABLE_POS_TRANSMISSION
      if (graphics.check_transmission_enabled()) //update positions but prevent text transmission if also using serial for something else
        coms_serial.send_pos_frame(i);
#endif
    }
  }
}
void Graphics::get_next_string_config_profile() {
  for (byte i = 0; i < MAX_NUM_OF_TEXT_OBJECTS; i++) {
    if (get_new_config[i]) {
#ifdef ALLOW_CONFIG_CHAINING
      Serial.println("get new config");
      card.retrieve_data(EXT_STRING_FILE, i, true);
#endif
      get_new_config[i] = false;
    }
  }
}


void Graphics::configure_limits(byte obj_num) {

  //  X limits
  if (text_cursor[obj_num].x_start_set && text_cursor[obj_num].x_end_set) { //limit configured from profile
    if (text_cursor[obj_num].x_end > text_cursor[obj_num].x_start) {
      text_cursor[obj_num].x_limit_max = text_cursor[obj_num].x_end;
      text_cursor[obj_num].x_limit_min = text_cursor[obj_num].x_start;
    }
    else if (text_cursor[obj_num].x_end < text_cursor[obj_num].x_start) {
      text_cursor[obj_num].x_limit_max = text_cursor[obj_num].x_start;
      text_cursor[obj_num].x_limit_min = text_cursor[obj_num].x_end;
    }
    else
      text_cursor[obj_num].x_limit_max = text_cursor[obj_num].x_limit_min = text_cursor[obj_num].x_start;
  }

  else {   //limit auto configured
    text_parameters[obj_num].text_str_length = (byte)find_string_length(obj_num);//value might not be set, so reconfirm here

    if (text_cursor[obj_num].x_pos_dir == 128) {
      text_cursor[obj_num].x_limit_min = (TOTAL_WIDTH >> 1) - ((text_parameters[obj_num].text_size * text_parameters[obj_num].text_width * text_parameters[obj_num].text_str_length) >> 1);
      text_cursor[obj_num].x_limit_max = (TOTAL_WIDTH >> 1) - ((text_parameters[obj_num].text_size * text_parameters[obj_num].text_width * text_parameters[obj_num].text_str_length) >> 1);
    }
    else {

      text_cursor[obj_num].x_limit_min = -1 * (text_parameters[obj_num].text_size * text_parameters[obj_num].text_width * text_parameters[obj_num].text_str_length);
      text_cursor[obj_num].x_limit_max = TOTAL_WIDTH;
    }
  }


  //  Y limits
  if (text_cursor[obj_num].y_start_set && text_cursor[obj_num].y_end_set) {
    if (text_cursor[obj_num].y_end > text_cursor[obj_num].y_start) {
      text_cursor[obj_num].y_limit_max = text_cursor[obj_num].y_end;
      text_cursor[obj_num].y_limit_min = text_cursor[obj_num].y_start;
    }
    else if (text_cursor[obj_num].y_end < text_cursor[obj_num].y_start) {
      text_cursor[obj_num].y_limit_max = text_cursor[obj_num].y_start;
      text_cursor[obj_num].y_limit_min = text_cursor[obj_num].y_end;
    }
    else
      text_cursor[obj_num].y_limit_max = text_cursor[obj_num].y_limit_min = text_cursor[obj_num].y_start;

  }
  else {
    if (text_cursor[obj_num].y_pos_dir == 128) {
      text_cursor[obj_num].y_limit_min = (SINGLE_MATRIX_HEIGHT >> 1) - ((text_parameters[obj_num].text_size * text_parameters[obj_num].text_height) >> 1);
      text_cursor[obj_num].y_limit_max = (SINGLE_MATRIX_HEIGHT >> 1) - ((text_parameters[obj_num].text_size * text_parameters[obj_num].text_height) >> 1);
    }
    else {
      text_cursor[obj_num].y_limit_min = -1 * (text_parameters[obj_num].text_size * text_parameters[obj_num].text_height); //<- is this right
      text_cursor[obj_num].y_limit_max = SINGLE_MATRIX_HEIGHT;
    }
  }

}


void Graphics::reset_position(byte obj_num) {

  if (text_cursor[obj_num].x_start_set && text_cursor[obj_num].x_end_set)
    text_cursor[obj_num].x = text_cursor[obj_num].x_start;
  else {
    if (text_cursor[obj_num].x_pos_dir >= 128)
      text_cursor[obj_num].x = text_cursor[obj_num].x_limit_min;
    else
      text_cursor[obj_num].x = text_cursor[obj_num].x_limit_max;
  }

  if (text_cursor[obj_num].y_start_set && text_cursor[obj_num].y_end_set)
    text_cursor[obj_num].y = text_cursor[obj_num].y_start;
  else {
    if (text_cursor[obj_num].y_pos_dir >= 128)
      text_cursor[obj_num].y = text_cursor[obj_num].y_limit_min;
    else
      text_cursor[obj_num].y = text_cursor[obj_num].y_limit_max;
  }
}


void Graphics::reset_position() {

  for (byte i = 0; i < MAX_NUM_OF_TEXT_OBJECTS; i++) {
    reset_position(i);
  }

}


void Graphics::configure_limits() {

  for (byte i = 0; i < MAX_NUM_OF_TEXT_OBJECTS; i++) {
    configure_limits(i);
  }
}

void Graphics::push_string_data() {

  static uint16_t num_text_transmissions = 0;

  //loop through objects, push to all megas if enabled and not updated
  for (byte i = 0; i < MAX_NUM_OF_TEXT_OBJECTS; i++) {

#ifdef FORCE_TEXT_FRAME_TRANSMISSION// different condition for testing purposes, ignore if they were updated, just push set number of times
    if (num_text_transmissions < NUM_TEXT_TRANSMISSIONS)
#else
    if (text_cursor[i].object_used && !text_parameters[i].megas_up_to_date)  //if the object is enabled and has been changed by something
#endif
    {
      disable_pos_isr();  // disable pos isr while we push the string
      coms_serial.send_text_frame(i);
      coms_serial.send_text_calibration_data(i); //send all related text data
      text_parameters[i].megas_up_to_date = true; //confirm text up to date
      enable_pos_isr();   //enable pos isr again
    }
  }
#ifdef FORCE_TEXT_FRAME_TRANSMISSION  //if testing code, increment push count
  num_text_transmissions++;
#endif

}

inline uint16_t Graphics::find_string_length(byte obj_num) {
  uint16_t i = 0;
  char content;
  do {
    content = text_str[obj_num][i];
    i++;
  } while (content != '\0' && i <= MAX_TWEET_SIZE);
  return (i - 1); //return length of string
}


#endif  // Graphics_CPP
