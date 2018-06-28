#ifndef Current_Control_H
#define Current_Control_H

// all data and functions associated with the current meters, LDR's and associated brightness control algorithms

#include "Config_Local.h"
#include "Due_Pins.h"


#define MAX_CURRENT_DRAW 35   // limit of the psu-10%
#define SCREEN_BRIGHTNESS_UPDATE_THRESHOLD 5  // require 5% difference in target screen brightness and actual brightness before update pushed to megas
#define RING_BUFFER_SHIFT 4
#define RING_BUFFER_LENGTH 2^RING_BUFFER_SHIFT

#define LDR_UPDATE_PERIOD (int16_t)5000/RING_BUFFER_LENGTH  // ring buffer filter cutoff frequency approximately 5s

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
  byte large_disparity = 25;   // ~10% of potiential range

  byte config_max1 =200;
  byte config_max2 =200;
  
  byte config_min1 = 0;
  byte config_min2 = 0;
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

    void read_sensor(byte sensor);
    void enable1();
    void disable1();
    void enable2();
    void disable2();
    void avg_sensor_result();
    void read_sensors();

  public:
    Light_Sensor() {}
    int init_LDR();

    void enable();
    void disable();
    byte calculate_target_brightness(); //use sensor readings to calculate the scaling value for brightness, returns percentage
    void LDR_calibration();
    void get_readings();
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













#endif  // Current_Control_H
