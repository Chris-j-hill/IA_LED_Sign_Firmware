#ifndef Current_Control_CPP
#define Current_Control_CPP

#include "Current_Control.h"
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



byte screen_brightness = 255;

LDR_Struct light_sensor_parameters;
Current_Meter_Struct current_meter_parameters;





#endif  // Current_Control_CPP
