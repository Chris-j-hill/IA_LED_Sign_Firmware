#ifndef Graphics_H
#define Graphics_H

#include "Config_Local.h"

#define ASCII_CHARACTER_BASIC_WIDTH 6
#define ASCII_CHARACTER_BASIC_HEIGHT 9


byte text_size = DEFAULT_TEXT_SIZE;
byte text_colour_r = DEFAULT_TEXT_RED_BRIGHTNESS;
byte text_colour_g = DEFAULT_TEXT_GREEN_BRIGHTNESS;
byte text_colour_b = DEFAULT_TEXT_BLUE_BRIGHTNESS;
byte text_colour_hue_MSB = 0;   //value between +/- 1536 
byte text_colour_hue_LSB = 0;
bool use_hue = true;

int  text_colour_hue = 1000;

byte screen_mode=0;   //mode of operation on startup should be both displaying
                      //mode0: both on
                      //mode1: one side on
                      //mode2: both off
                      //mode3: other side on



struct Colour {
  int red = DEFAULT_TEXT_RED_BRIGHTNESS;
  int blue = DEFAULT_TEXT_BLUE_BRIGHTNESS;
  int green = DEFAULT_TEXT_GREEN_BRIGHTNESS;
  int hue = 0;
  bool use_hue = false;     //if true, use hue value, otherwise colour using rgb
};

struct Text {
  int text_width = ASCII_CHARACTER_BASIC_WIDTH;
  int text_height = ASCII_CHARACTER_BASIC_HEIGHT;
  Colour colour;
};

Text text;    

struct Text_cursor {        //xy coordinates of cursor position
  int x = 0;
  int y = 0;

  byte x_min = 0;
  
  bool mega1_updated = false;   //is the mega up to date with current positions, useful if not sending text displaying menu option
  bool mega2_updated = false;
  bool mega3_updated = false;
  bool mega4_updated = false;
};
Text_cursor text_cursor;












#endif  // Graphics_H
