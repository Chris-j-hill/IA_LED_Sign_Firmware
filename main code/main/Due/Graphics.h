#ifndef Graphics_H
#define Graphics_H


#include "Config_Local.h"
#include "Coms.h"
#include <Arduino.h>

#define ASCII_CHARACTER_BASIC_WIDTH 6
#define ASCII_CHARACTER_BASIC_HEIGHT 9


#if defined(USING_COLOUR_SET_333)
const byte RED_MAX_VALUE = 7;
const byte GREEN_MAX_VALUE = 7;
const byte BLUE_MAX_VALUE = 7;

#elif defined(USING_COLOUR_SET_444)
const byte RED_MAX_VALUE = 15;
const byte GREEN_MAX_VALUE = 15;
const byte BLUE_MAX_VALUE = 15;

#elif defined(USING_COLOUR_SET_888)
const byte RED_MAX_VALUE = 255;
const byte GREEN_MAX_VALUE = 255;
const byte BLUE_MAX_VALUE = 255;
#endif

const int HUE_MAX_VALUE = 1535;
const int HUE_MIN_VALUE = -1535;
#define HUE_ADJUSTMENT_STEP_SIZE 5    // step size in menu when adjusting hue

//how often will the text buffer in the megas be refreshed
#define DEFAULT_TEXT_SIZE 2

#define DEFAULT_TEXT_RED_BRIGHTNESS RED_MAX_VALUE
#define DEFAULT_TEXT_GREEN_BRIGHTNESS GREEN_MAX_VALUE
#define DEFAULT_TEXT_BLUE_BRIGHTNESS BLUE_MAX_VALUE


#define SCREEN_BRIGHTNESS_UPDATE_PERIOD 500

#define RUN_TEXT_CONFIG //define this to populate some text objects with minimal data, forces pos interrupt to send frames

#define POS_TIMER Timer3
#define POS_TIMER_INTERRUPT TC2_IRQn



struct Text {
  byte text_width = ASCII_CHARACTER_BASIC_WIDTH;
  byte text_height = ASCII_CHARACTER_BASIC_HEIGHT;
  byte text_size = DEFAULT_TEXT_SIZE;
  uint16_t text_str_length = MAX_TWEET_SIZE;

  byte red = DEFAULT_TEXT_RED_BRIGHTNESS;
  byte blue = DEFAULT_TEXT_BLUE_BRIGHTNESS;
  byte green = DEFAULT_TEXT_GREEN_BRIGHTNESS;
  int hue = 0;
  bool use_hue = false;     //if true, use hue value, otherwise colour using rgb

  bool megas_up_to_date = false;   //is the mega up to date with this objects string

};



struct Text_cursor {        //xy coordinates of cursor position
  int x = 0;
  int y = 0;
  byte x_pos_dir = 140;   //direction and speed of the cursor, static = 128
  byte y_pos_dir = 128;

  int x_start = 0;  // limits for pos, also specify direction, overflow values
  int y_start = 0;  // if conflict between direction and speed and start end points, points win take speed magnitude
  int x_end = 255;
  int y_end = 255;

  bool x_start_set = false;
  bool y_start_set = false;
  bool x_end_set = false;
  bool y_end_set = false;

//  bool mega1_updated = false;   //is the mega up to date with current positions, useful if not sending text displaying menu option
//  bool mega2_updated = false;
//  bool mega3_updated = false;
//  bool mega4_updated = false;

  byte ISR_freq = POS_UPDATE_ISR_FREQ;

  int x_limit_max = TOTAL_WIDTH;
  int y_limit_max = SINGLE_MATRIX_HEIGHT;
  int x_limit_min = 0;
  int y_limit_min = 0;

  uint16_t loops_x =0;
  uint16_t loops_y =0;
  int str_disp_time = 0;
  bool check_for_new_file=false;
  bool found_loops_x = false;
  bool found_loops_y = false;
  bool found_time = false;
  int change_file_timeout; //<- on file change, reset this

  bool object_used = false; //<- is this object used, has it been populated from string file
};



void attach_timer_pos_update();

void set_pos_update_frequency(float freq);

void send_pos_interrupt();     // interrupt to send pos data to all megas




class Graphics {

  private:
    bool transmission_enabled = true; //disable transmission within pos update isr (used to allow complete text string to be transmitted uninterrupted)
    inline uint16_t find_string_length(byte obj_num);
  public:
    Graphics() {}
    void update_brightness();

    void update_colour();
    void update_size();

    void init_cursor();
    void configure_limits(byte obj_num);
    void configure_limits();
    void reset_position(byte obj_num);
    void reset_position();
    void get_next_string_config_profile();
    void push_string_data();


    
    inline void disable_pos_isr(){
      transmission_enabled = false;
    }
    
    inline void enable_pos_isr(){
      transmission_enabled = true;
    }
    inline bool check_transmission_enabled(){
      return transmission_enabled;
    }
};





#endif  // Graphics_H
