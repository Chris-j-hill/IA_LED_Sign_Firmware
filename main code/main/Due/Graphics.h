#ifndef Graphics_H
#define Graphics_H

#include "Config_Local.h"
#include "Coms.h"

#define ASCII_CHARACTER_BASIC_WIDTH 6
#define ASCII_CHARACTER_BASIC_HEIGHT 9


#if defined(USING_COLOUR_SET_333)
#define RED_MAX_VALUE 7
#define GREEN_MAX_VALUE 7
#define BLUE_MAX_VALUE 7
#elif defined(USING_COLOUR_SET_444)
#define RED_MAX_VALUE 15
#define GREEN_MAX_VALUE 15
#define BLUE_MAX_VALUE 15

#elif defined(USING_COLOUR_SET_888)
#define RED_MAX_VALUE 255
#define GREEN_MAX_VALUE 255
#define BLUE_MAX_VALUE 255
#endif

#define HUE_MAX_VALUE 1535
#define HUE_MIN_VALUE -1535
#define HUE_ADJUSTMENT_STEP_SIZE 5    // step size in menu when adjusting hue

//how often will the text buffer in the megas be refreshed
#define TEXT_TRANSMIT_PERIOD 2000
#define DEFAULT_TEXT_SIZE 2

#define DEFAULT_TEXT_RED_BRIGHTNESS RED_MAX_VALUE
#define DEFAULT_TEXT_GREEN_BRIGHTNESS GREEN_MAX_VALUE
#define DEFAULT_TEXT_BLUE_BRIGHTNESS BLUE_MAX_VALUE


#define SCREEN_BRIGHTNESS_UPDATE_PERIOD 500

#define POS_UPDATE_ISR_FREQ 5//hz

#define XY_SPEED_UNITS 0.1//speed of +1 moves XY_SPEED_UNITS pixels per second


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

  bool mega1_updated = false;   //is the mega up to date with current string
  bool mega2_updated = false;
  bool mega3_updated = false;
  bool mega4_updated = false;
};



struct Text_cursor {        //xy coordinates of cursor position
  int x = 0;
  int y = 0;
  byte x_pos_dir = 138;   //direction and speed of the cursor, static = 128
  byte y_pos_dir = 118;

  int x_start = 0;  // limits for pos, also specify direction, overflow values
  int y_start = 0;  // if conflict between direction and speed and start end points, points win take speed magnitude
  int x_end = 255;
  int y_end = 255;

  bool x_start_set = false;
  bool y_start_set = false;
  bool x_end_set = false;
  bool y_end_set = false;

  bool mega1_updated = false;   //is the mega up to date with current positions, useful if not sending text displaying menu option
  bool mega2_updated = false;
  bool mega3_updated = false;
  bool mega4_updated = false;

  byte ISR_freq = POS_UPDATE_ISR_FREQ;

  int x_limit_max = TOTAL_WIDTH;
  int y_limit_max = SINGLE_MATRIX_HEIGHT;
  int x_limit_min = 0;
  int y_limit_min = 0;

  byte loops =0;
  int str_disp_time = 0;
  bool check_for_new_file=false;
  
};



void attach_timer_pos_update();

void set_pos_update_frequency(int freq);

void send_pos_interrupt();     // interrupt to send pos data to all megas




class Graphics {

  private:

  public:
    Graphics() {}
    void flip_direction();
    void update_brightness();

    void update_colour();
    void update_size();

    void init_cursor();
    void configure_limits();
    void reset_position();
};





#endif  // Graphics_H
