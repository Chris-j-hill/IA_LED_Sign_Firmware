
#ifndef Fans_H
#define Fans_H

#include "Config_Local.h"
#include "function_declarations.h"
#include "Due_Pins.h"



#define FAN_DEFAULT_SPEED 180 
#define FAN_TURN_ON_TEMPERATURE 30
#define FAN_MAX_SPEED_TEMPERATURE 40
#define ALLOW_FULL_MANUAL_OVERRIDE  // will stay at manually set speed regardless of temperature
#define ALLOW_SMART_MANUAL_OVERRIDE // allow override, but will increase temperature to avoid thermal run away
#define MANUAL_START_SPEED 0


#define TEMPERATURE_SENSOR_1_CONNECTED    //comment out if not using specific sensors
#define TEMPERATURE_SENSOR_2_CONNECTED
#define TEMPERATURE_SENSOR_3_CONNECTED

#define MAX_OPERATING_TEMPERATURE 50    // max value, if bigger assume sensor broken
#define MIN_OPERATING_TEMPERATURE 5

struct Temp_sensor{

  int pin1 = TEMP1;
  int pin2 = TEMP2;
  int pin3 = TEMP3;

  bool enabled1 = true;
  bool enabled2 = false;
  bool enabled3 = true;

  byte dat1[5];         //data array for temp and humidity
  byte dat2[5];
  byte dat3[5];

  int temp1 = -100;   //current temperature as read by sensor, set to something unlikely to start
  int temp2 = -100;
  int temp3 = -100;

  int humidity1 = -100;   //current humidity as read by sensor
  int humidity2 = -100;
  int humidity3 = -100;

  int bad_connection1 = false;  // detect fault based on temp returned, if outside min/max range defined above, presume fault
  int bad_connection2 = false;
  int bad_connection3 = false;
  
  int avg=0;
};


//byte fan_speed = FAN_DEFAULT_SPEED;

struct Fan_Struct {
  int pin = FAN_PIN;                  // pin num
  int manual_set_value = MANUAL_START_SPEED;
  int target_speed = FAN_DEFAULT_SPEED;              // target value to approach, not necessicarily the current speed
  int current_speed = 0;              // current value written to fan
  int change_increment = 10;           // value to increment by to approach target
  int fan_change_interval = 50;           // milliseconds to wait between incrementing value
  int fan_minimum = 100;                  // minimum value where the fans are on
  bool enabled  = false;                    // is fan enabled, if so, run functions
  bool manual = false;
};

int attach_timer_fan();

void fade_fans();          // interrupt to change the current value of the fans to approach the target value

int fans_set_freq();      //interrupt to set the frequency the fans are adjusted




//class for both fan cantrol and temperature monitoring  
class Fans{

  private:

    int poll_temperature_sensor (int pin);       // read temp sensor registers, used to update stored value to most recent
    byte read_temp_data_from_register (int pin);        // convert incoming bits from temp sensor to a byte
    void enable_temp1();
    void enable_temp2();
    void enable_temp3();
    
    void disable_temp1();
    void disable_temp2();
    void disable_temp3();
    void calculate_avg_temp();
    void calculate_fan_speed();
    
  public:
    
    Fans(){}
    
      //initialisations of individual items, set pins, test if working if applicable, etc
    int init_fans();
    int init_temp_sensors();            
    int set_fan_speed();   
    int get_temperature (int pin);       // read temperature based on specified pin, return value in degrees C
    void enable_temp();
    void disable_temp();
//TODO

    int set_fan(int newValue);
    int is_fan_on();                  //useful for menu display
    int return_fan_speed();
    int get_humidity();       //because we can
    int set_temp_sensor_enable(int sensor);   //code to enable sensors, might be useful to toggle them, maybe autotoggle if not working?
    int set_temp_sensor_disable(int sensor);
    void enable();
    void disable();

};


#endif // Fans_H
