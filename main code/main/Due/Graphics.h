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
#error "Limits not configured fully for 444 colour range"
#define RED_MAX_VALUE 15
#define GREEN_MAX_VALUE 15
#define BLUE_MAX_VALUE 15
#endif

#define HUE_MAX_VALUE 1535
#define HUE_MIN_VALUE -1535
#define HUE_ADJUSTMENT_STEP_SIZE 5    // step size in menu when adjusting hue


#define SCREEN_BRIGHTNESS_UPDATE_PERIOD 500

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





class Graphics{

  private:

  public:
  Graphics(){}
  void flip_direction();
  void use_hue(){};
  void use_rgb(){};
  void update_brightness();
  
};





#endif  // Graphics_H
