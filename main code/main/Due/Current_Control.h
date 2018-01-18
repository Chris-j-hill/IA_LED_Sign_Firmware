#ifndef Current_Control_H
#define Current_Control_H

// all data and functions associated with the current meters, LDR's and associated brightness control algorithms

#include "Config_Local.h"
#include "Due_Pins.h"
#define MAX_CURRENT_DRAW 35   // limit of the psu-10%

extern byte screen_brightness;

struct LDR_Struct{
  int pin1 = LDR1;
  int pin2 = LDR2;
  byte reading1 =7;
  byte reading2 =8;

  bool LRD1_enabled = true;
  bool LDR2_enabled = true;
};

struct Current_Meter_Struct{
  int pin1 = CURRENT_METER_1;
  int pin2 = CURRENT_METER_2;
  byte reading1 = 2;
  byte reading2 = 3;
  int max_current_limit = MAX_CURRENT_DRAW;
  
  bool meter1_enabled = true;
  bool meter2_enabled = true;
};


class Light_Sensor{

private:


public:
  Light_Sensor(){}
  int init_LDR();
  int read_sensors();
  
};

class Current_Meter{

private:

public:
  Current_Meter(){}

  int init_current_meter();
  int set_current_limit();
    
};

#endif  // Current_Control_H
