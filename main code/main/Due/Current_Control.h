#ifndef Current_Control_H
#define Current_Control_H

// all data and functions associated with the current meters, LDR's and associated brightness control algorithms

#include "Config_Local.h"
#include "Due_Pins.h"


#define MAX_CURRENT_DRAW 35   // limit of the psu-10%
#define SCREEN_BRIGHTNESS_UPDATE_THRESHOLD 5  // require 5% difference in target screen brightness and actual brightness before update pushed to megas

#define RING_BUFFER_SHIFT 4
#define RING_BUFFER_LENGTH 16//2^RING_BUFFER_SHIFT

#define LDR_UPDATE_PERIOD (int16_t)5000/RING_BUFFER_LENGTH  // ring buffer filter cutoff frequency approximately 5s
#define CURRENT_METER_UPDATE_PERIOD (int16_t)15000/RING_BUFFER_LENGTH // approximately 1s sampling rate


#define THIRTY_AMP 3
#define TWENTY_AMP 2
#define FIVE_AMP 1

struct LDR_Struct {
  byte pin1 = LDR1;
  byte pin2 = LDR2;

  byte reading1 = 0;
  byte reading2 = 0;

  bool enabled1 = true;
  bool enabled2 = true;

  bool bad_connection1 = false;   //use if one sensor has a bad connection but the other is fine
  bool bad_connection2 = false;

  byte avg_reading = 0;
  byte large_disparity = 25;   // ~10% of potiential range

  byte config_max1 = 200;
  byte config_max2 = 200;

  byte config_min1 = 0;
  byte config_min2 = 0;
};

struct Current_Meter_Struct {
  byte pin1 = CURRENT_METER_1;
  byte pin2 = CURRENT_METER_2;
  byte reading1 = 0;
  byte reading2 = 0;
  byte max_current_limit = MAX_CURRENT_DRAW;

  bool enabled1 = true;
  bool enabled2 = true;

  byte total = 0;

  byte config_max1 = 200;
  byte config_max2 = 200;

  byte config_min1 = 0;
  byte config_min2 = 0;
};


class Light_Sensor {

  private:

    void read_sensor(byte sensor);
    inline void enable1();
    inline void disable1();
    inline void enable2();
    inline void disable2();
    void avg_sensor_result();
    void read_sensors();

  public:
    Light_Sensor() {}
    int init_LDR();

    void enable();
    void disable();
    byte calculate_target_brightness(); //use sensor readings to calculate the scaling value for brightness, returns percentage
    void get_readings();
};

class Current_Meter {

  private:

    inline void enable1();
    inline void enable2();
    inline void disable1();
    inline void disable2();

    void read_current_meter(byte sensor);  // read the current sensor
    double reading_to_amps(uint16_t value, byte sensor_type = THIRTY_AMP);   // convert 10 bit analog reading to amps
    void avg_sensor_result();
  public:
    Current_Meter() {}

    void init_current_meter();
    void set_current_limit(byte value);    //software limit, can only be less than MAX_CURRENT_DRAW
    void enable();
    void disable();
    void get_readings();
};

#endif  // Current_Control_H
