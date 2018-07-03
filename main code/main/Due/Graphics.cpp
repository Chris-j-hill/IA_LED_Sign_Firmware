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

void Graphics::flip_direction() {}  //flip the direction currently scrolling

void Graphics::update_brightness() {

  byte target_brightness = light_sensor.calculate_target_brightness();  //
  if (screen_brightness != target_brightness && (millis() - SCREEN_BRIGHTNESS_UPDATE_PERIOD) > last_brightness_update) {
    screen_brightness = target_brightness;
    for (int i = 0; i < NUM_SCREENS; i++)
      coms_serial.send_specific_calibration_data(PREFIX_SCREEN_BRIGHTNESS, i, false, 0);  //push to screens
    last_brightness_update = millis();
  }
}

void Graphics::use_hue() {
  text_parameters.use_hue = true;
}
void Graphics::use_rgb() {
  text_parameters.use_hue = false;
}



#endif  // Graphics_CPP
