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



byte screen_brightness = 100;

LDR_Struct light_sensor_parameters;
Current_Meter_Struct current_meter_parameters;

int Light_Sensor::init_LDR(){

  if (enable_LDR){
    this -> enable();
    this -> read_sensor(1); //check connection
  }
  else 
    this -> disable();
    this -> read_sensor(2); 
}

void Light_Sensor::read_sensors(){
  
  if (!light_sensor_parameters.bad_connection1 && light_sensor_parameters.enabled1){
     this -> read_sensor(1);
  }
  if (!light_sensor_parameters.bad_connection2 && light_sensor_parameters.enabled2){
     this -> read_sensor(2);
  }

  this -> avg_sensor_result();
  
}


byte Light_Sensor::calculate_target_brightness(){

  byte temp_brightness = map(light_sensor_parameters.avg_reading, 0, 255, 0, 100);
  if (current_meter_parameters.total>current_meter_parameters.max_current_limit){
    temp_brightness = temp_brightness-2;        //reduce by 2% every loop until under current limit
    
    }
  //require difference between target_brightness and current birghtness to be above threshold before update gets pushed to megas
  if (abs(screen_brightness-temp_brightness)>SCREEN_BRIGHTNESS_UPDATE_THRESHOLD)  
    return temp_brightness;  
  else
    return screen_brightness;
  
  }



void Light_Sensor::enable(){
  if (!light_sensor_parameters.bad_connection1 && !light_sensor_parameters.enabled1)  //if bad connection or already enabled dont run
  this -> enable1();
  if (!light_sensor_parameters.bad_connection2 && !light_sensor_parameters.enabled2)
  this -> enable2();
  
}

void Light_Sensor::disable(){
  
  this -> disable1();
  this -> disable2();
  
}

void Light_Sensor::enable1(){
 
  pinMode(light_sensor_parameters.pin1, INPUT_PULLUP);
  light_sensor_parameters.enabled1 = true;  
}

void Light_Sensor::enable2(){
  
  pinMode(light_sensor_parameters.pin2, INPUT_PULLUP);
  light_sensor_parameters.enabled2 = true;
}

void Light_Sensor::disable1(){
  light_sensor_parameters.enabled1 = false;
}
void Light_Sensor::disable2(){
  light_sensor_parameters.enabled1 = false;
}

void Light_Sensor::read_sensor(int sensor) {       // read the average value of the LDR's, if one fails to read correctly or large disparity fail returned

  if (sensor==1 && light_sensor_parameters.enabled1 && light_sensor_parameters.bad_connection1){
    light_sensor_parameters.reading1 = map(analogRead(light_sensor_parameters.pin1), 0, 1024, 0, 255);
    if (light_sensor_parameters.reading1 == 0 || light_sensor_parameters.reading1 == 255) {  
      Sprintln(F("From Light_Sensor.read_sensor: \t Problem reading light sensor 1, check wiring"));
      light_sensor_parameters.bad_connection1 = true;
    }    
  }

  else if (sensor == 2 && light_sensor_parameters.enabled2 && light_sensor_parameters.bad_connection2){
    light_sensor_parameters.reading2 = map(analogRead(light_sensor_parameters.pin2), 0, 1024, 0, 255);
    if (light_sensor_parameters.reading2 == 0 || light_sensor_parameters.reading2 == 255) {  
      Sprintln(F("From Light_Sensor.read_sensor: \t Problem reading light sensor 2, check wiring"));
      light_sensor_parameters.bad_connection2 = true;
    } 
  }
}


void Light_Sensor::avg_sensor_result(){

  if (!light_sensor_parameters.bad_connection1 && !light_sensor_parameters.bad_connection2){  //if connections fine, get average
    if (abs(light_sensor_parameters.reading1 - light_sensor_parameters.reading2) >= light_sensor_parameters.large_disparity) { //if large disparity between sensor readings, uses the higher, possibly one covered
      
       Sprintln(F("From LDR_check_value: \t Large disparity in sensor readings, return higher"));
       Sprint(F("sensor 1: "));
       Sprint(light_sensor_parameters.reading1);
       Sprint(F("\t sensor 2: "));
       Sprintln(light_sensor_parameters.reading2);
      
      if (light_sensor_parameters.reading1 > light_sensor_parameters.reading2)
        light_sensor_parameters.avg_reading = light_sensor_parameters.reading1;

      else
        light_sensor_parameters.avg_reading = light_sensor_parameters.reading2;
    }
    else {                                      
      light_sensor_parameters.avg_reading = (light_sensor_parameters.reading1 + light_sensor_parameters.reading2) / 2;
      Sprintln(F("From Light_Sensor.avg_sensor_result: Both sensor readings good, return average"));
      Sprint(F("Average reading: "));
      Sprintln(light_sensor_parameters.avg_reading);
    }
  }

  else { // return good result if available 
    if (!light_sensor_parameters.bad_connection1 && light_sensor_parameters.bad_connection2) {  //if one sensor is usable just return the value    
      light_sensor_parameters.avg_reading = light_sensor_parameters.reading1;
      Sprintln(F("From Light_Sensor.avg_sensor_result: \t Return sensor 1 reading"));
    }
  
    else if (light_sensor_parameters.bad_connection1 && !light_sensor_parameters.bad_connection2) {
     light_sensor_parameters.avg_reading = light_sensor_parameters.reading2;
      Sprintln(F("From Light_Sensor.avg_sensor_result: \t Return sensor 2 reading"));   
    }
    
    else{   //if neither usable set avg as highest value
      light_sensor_parameters.avg_reading = 255;
      Sprintln(F("From Light_Sensor.avg_sensor_result: \t Error reading both sensors, check connections"));
      
    }
  }
}


#endif  // Current_Control_CPP
