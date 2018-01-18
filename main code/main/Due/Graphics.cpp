#ifndef Graphics_CPP
#define Graphics_CPP

#include "Graphics.h"
#include "Arduino.h"
#include "Config_Local.h"



Text text;
Text_cursor text_cursor;

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

char text_str[150] = "this is a test4 this is a test5 this is a test6";



#endif  // Graphics_CPP
