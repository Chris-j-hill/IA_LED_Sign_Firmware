#ifndef Graphics_CPP
#define Graphics_CPP

#include "Graphics.h"
#include "Arduino.h"
#include "Config_Local.h"
#include "Current_control.h"
#include "Coms_Serial.h"

extern struct LDR_Struct light_sensor_parameters;
extern Light_Sensor light_sensor;
extern Coms_Serial coms_serial;
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
int last_brightness_update=0;
byte screen_mode=0;   //mode of operation on startup should be both displaying
                      //mode0: both on
                      //mode1: one side on
                      //mode2: both off
                      //mode3: other side on

char text_str[MAX_TWEET_SIZE] = "this is a test4 this is a test5 this is a test6";



void Graphics::flip_direction(){}   //flip the direction currently scrolling

void Graphics::update_brightness(){
  
  byte target_brightness = light_sensor.calculate_target_brightness();
  if (screen_brightness != target_brightness && (millis()-SCREEN_BRIGHTNESS_UPDATE_PERIOD) > last_brightness_update){
    for (int i=0;i<NUM_SCREENS;i++)
      coms_serial.send_specific_calibration_data(PREFIX_SCREEN_BRIGHTNESS, i, false, 0);  //push to screens
    last_brightness_update = millis();
  }
}
#endif  // Graphics_CPP
