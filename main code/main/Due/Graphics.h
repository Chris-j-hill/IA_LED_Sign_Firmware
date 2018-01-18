#ifndef Graphics_H
#define Graphics_H

#include "Config_Local.h"

#define ASCII_CHARACTER_BASIC_WIDTH 6
#define ASCII_CHARACTER_BASIC_HEIGHT 9




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

    

struct Text_cursor {        //xy coordinates of cursor position
  int x = 0;
  int y = 0;

  byte x_min = 0;
  
  bool mega1_updated = false;   //is the mega up to date with current positions, useful if not sending text displaying menu option
  bool mega2_updated = false;
  bool mega3_updated = false;
  bool mega4_updated = false;
};




//declare variables used outside of this cpp file

extern char text_str[150];
extern byte text_size;
extern byte text_colour_r;
extern byte text_colour_g;
extern byte text_colour_b;
extern bool use_hue;
extern byte screen_mode;
extern int  text_colour_hue;








#endif  // Graphics_H
