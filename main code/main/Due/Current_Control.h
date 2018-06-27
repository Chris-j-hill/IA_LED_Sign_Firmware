#ifndef Current_Control_H
#define Current_Control_H

// all data and functions associated with the current meters, LDR's and associated brightness control algorithms

#include "Config_Local.h"
#include "Due_Pins.h"


#define MAX_CURRENT_DRAW 35   // limit of the psu-10%
#define SCREEN_BRIGHTNESS_UPDATE_THRESHOLD 5  // require 5% difference in target screen brightness and actual brightness before update pushed to megas

extern byte screen_brightness;

struct LDR_Struct {
  int pin1 = LDR1;
  int pin2 = LDR2;

  byte reading1 = 7;
  byte reading2 = 8;

  bool enabled1 = true;
  bool enabled2 = true;

  bool bad_connection1 = false;   //use if one sensor has a bad connection but the other is fine
  bool bad_connection2 = false;

  byte avg_reading = 0;
  byte large_disparity = 100;   // ~10% of potiential range
};

struct Current_Meter_Struct {
  int pin1 = CURRENT_METER_1;
  int pin2 = CURRENT_METER_2;
  byte reading1 = 2;
  byte reading2 = 3;
  int max_current_limit = MAX_CURRENT_DRAW;

  bool meter1_enabled = true;
  bool meter2_enabled = true;

  byte total = 0;
};


class Light_Sensor {

  private:

    void read_sensor(int sensor);
    void enable1();
    void disable1();
    void enable2();
    void disable2();
    void avg_sensor_result();

  public:
    Light_Sensor() {}
    int init_LDR();
    void read_sensors();
    void enable();
    void disable();
    byte calculate_target_brightness(); //use sensor readings to calculate the scaling value for brightness, returns percentage

};

class Current_Meter {

  private:

  public:
    Current_Meter() {}

    int init_current_meter();
    void set_current_limit(int value);    //software limit, can only be less than MAX_CURRENT_DRAW

};




int read_current_meter(int pin);  // read the current sensor
float reading_to_amps(int value);   // convert 10 bit analog reading to amps

//int LDR_calibration();        //calibrate maximum minimum and room brightness lights






//int due_class::LDR_calibration() {        // code to set ambient brightness, max and min values
//
//  // since the lrd likely will never read near the  rail voltage when in light or darkness
//  // ambient, max and min will need to be calibrated. this could be calculated roughly too, based on
//  // approximate series resistor values, but will vary depending on component tollerances
//  // TO DO: 1) add screen interface not just serial feedback
//  //        2) configure sensors individually not just the average, in case using non identical components
//
//
//  if (debug) {    // need debug for this to work
//    char accept_val = 'w';    //variable to detect if serial input given, if val == x -> quit
//    //                                          id val == s -> skip current
//    int reading1;             // variables to store readings from ldr's
//    int reading2;
//    // configure ambient
//    Sprintln (F("LDR configuration tool:"));
//
//    // set ambient brightness value
//    Sprint(F("Set the ambient brightness, make sure neither ldr's covered"));
//    while (Serial.available() == 0) { //wait until some serial data arrives
//      // give prompt on value read from pins
//      delay(500);
//      Sprint(F("LDR 1 value: "));
//      Sprint(analogRead(LDR.pin1));
//      Sprint(F("LDR 2 value: "));
//      Sprintln(analogRead(LDR.pin2));
//    }
//
//    accept_val = Serial.read();    //read one byte
//    Serial.flush();                  //empty buffer
//    if (accept_val == 'x')
//      return (0);                      // if user quits, return 0
//    if (accept_val != 's') {      // if not skipping this stage
//      reading1 = analogRead(LDR.pin1);
//      reading2 = analogRead(LDR.pin2);
//      LDR.ambient = (reading1 + reading2) / 2; //set average as ambient
//      if (abs(reading1 - reading2) >= LDR.large_disparity) //if large disparity, give warning
//        Sprintln(F("Warning, large disparity on readings"));
//    }
//
//
//    //set maximum brightness value
//
//    Sprint(F("Set the maximum brightness, make sure neither ldr's covered, shine light on ldr 1"));
//    while (Serial.available() == 0) { //wait until some serial data arrives
//      // give prompt on value read from pin, help to identify pin
//      delay(500);
//      Sprint(F("LDR 1 value: "));
//      Sprintln(analogRead(LDR.pin1));
//    }
//
//    accept_val = Serial.read();
//    Serial.flush();
//    if (accept_val == 'x')
//      return (0);                      // if user quits, return 0
//
//    if (accept_val != 's') {
//
//      LDR.max_reading1 = analogRead(LDR.pin1);
//
//      Sprint(F("Set the maximum brightness, make sure neither ldr's covered, shine light on ldr 2"));
//      while (Serial.available() == 0) { //wait until some serial data arrives
//        // give prompt on value read from pin, help to identify pin
//        delay(500);
//        Sprint(F("LDR 2 value: "));
//        Sprintln(analogRead(LDR.pin2));
//      }
//
//      accept_val = Serial.read();
//      Serial.flush();
//      if (accept_val == 'x')
//        return (0);                      // if user quits, return 0
//      LDR.max_reading2 = analogRead(LDR.pin2);
//      if (abs(LDR.max_reading1 - LDR.max_reading2) >= LDR.large_disparity)
//        Sprintln(F("Warning, large disparity on readings"));
//    }
//
//    //set minimum brightness value
//    Sprint(F("Set the minimum brightness, cover ldr 1"));
//    while (Serial.available() == 0) { //wait until some serial data arrives
//      // give prompt on value read from pin, help to identify pin
//      delay(500);
//      Sprint(F("LDR 1 value: "));
//      Sprintln(analogRead(LDR.pin1));
//    }
//    accept_val = Serial.read();
//    Serial.flush();
//    if (accept_val == 'x')
//      return (0);                      // if user quits, return 0
//    if (accept_val != 's') {
//      LDR.min_reading1 = analogRead(LDR.pin1);
//      Sprint(F("Set the maximum brightness, cover ldr 2"));
//      while (Serial.available() == 0) { //wait until some serial data arrives
//        // give prompt on value read from pin, help to identify pin
//        delay(500);
//        Sprint(F("LDR 2 value: "));
//        Sprintln(analogRead(LDR.pin2));
//      }
//      accept_val = Serial.read();
//      Serial.flush();
//      if (accept_val == 'x')
//        return (0);                      // if user quits, return 0
//      LDR.min_reading2 = analogRead(LDR.pin2);
//      if (abs(LDR.min_reading1 - LDR.min_reading2) >= LDR.large_disparity)
//        Sprintln(F("Warning, large disparity on readings"));
//    }
//  }
//  else {  //debug not enabled, cant provide feedback to user
//    return (-2);
//  }
//}








#endif  // Current_Control_H
