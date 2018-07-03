#ifndef Graphics_H
#define Graphics_H

#include "Config_Local.h"

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




struct Text {
  byte text_width = ASCII_CHARACTER_BASIC_WIDTH;
  byte text_height = ASCII_CHARACTER_BASIC_HEIGHT;
  byte text_size = DEFAULT_TEXT_SIZE;
  
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
  byte x_pos_dir = 129;   //direction and speed of the cursor, static = 128
  byte y_pos_dir = 127;   

  int x_start =0;   // limits for pos, also specify direction, overflow values
  int y_start =0;   // if conflict between direction and speed and start end points, points win
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
};





class Graphics {

  private:

  public:
    Graphics() {}
    void flip_direction();
    void update_brightness();

    void update_colour();
    void update_size();
     
};





#endif  // Graphics_H
