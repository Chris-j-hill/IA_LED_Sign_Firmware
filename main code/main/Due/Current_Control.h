#ifndef Current_Control_CPP
#define Current_Control_CPP

// all data and functions associated with the current meters, LDR's and associated brightness control algorithms

#include "Config_Local.h"

#ifdef ENABLE_CURRENT_METER
bool enable_current_meter = true;
#else
bool enable_current_meter = false;
#endif

#ifdef ENABLE_LIGHT_SENSORS
bool enable_LDR = true;
#else
bool enable_LDR = false;
#endif

bool current_meter_enabled = false;
bool LDR_enabled = false;

byte light1 =7;
byte light2 =8;
byte current1 = 2;
byte current2 = 3;
byte screen_brightness = 255;




#endif  // Graphics_H
