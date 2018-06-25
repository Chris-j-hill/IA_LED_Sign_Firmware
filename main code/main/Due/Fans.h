
#ifndef Fans_H
#define Fans_H

#include "Config_Local.h"
#include "function_declarations.h"
#include "Due_Pins.h"



#define FAN_DEFAULT_SPEED 180 
#define FAN_TURN_ON_TEMPERATURE 28
#define FAN_MAX_SPEED_TEMPERATURE 40
#define ALLOW_SMART_MANUAL_OVERRIDE // allow override, but will increase temperature to avoid thermal run away, full manual control if not defined
#define MANUAL_START_SPEED 0


#define TEMPERATURE_SENSOR_1_CONNECTED    //comment out if not using specific sensors
#define TEMPERATURE_SENSOR_2_CONNECTED
#define TEMPERATURE_SENSOR_3_CONNECTED

#define MAX_OPERATING_TEMPERATURE 50    // max value sensor can return, if bigger assume sensor broken
#define MIN_OPERATING_TEMPERATURE 5

#define TEMP_UPDATE_DELAY_PERIOD 500

struct Temp_sensor{

  byte pin1 = TEMP1;
  byte pin2 = TEMP2;
  byte pin3 = TEMP3;

  bool enabled1 = true;
  bool enabled2 = true;
  bool enabled3 = true;

  byte dat1[5];         //data array for temp and humidity
  byte dat2[5];
  byte dat3[5];

  byte temp1 = 254;   //current temperature as read by sensor, set to something unlikely to start, (nb values not read as all high)
  byte temp2 = 254;
  byte temp3 = 254;

  byte humidity1 = 0;   //current humidity as read by sensor
  byte humidity2 = 0;
  byte humidity3 = 0;

  bool bad_connection1 = false;  // detect fault based on temp returned, if outside min/max range defined above, presume fault
  bool bad_connection2 = false;
  bool bad_connection3 = false;
  
  byte avg=0;
};


//byte fan_speed = FAN_DEFAULT_SPEED;

struct Fan_Struct {
  byte pin = FAN_PIN;                  // pin num
  byte manual_set_value = MANUAL_START_SPEED;
  byte target_speed = FAN_DEFAULT_SPEED;              // target value to approach, not necessicarily the current speed
  byte current_speed = 0;              // current value written to fan
  byte change_increment = 10;           // value to increment by to approach target
  uint16_t fan_change_interval = 50;           // milliseconds to wait between incrementing value
  byte fan_minimum = 100;                  // minimum value where the fans are on
  bool enabled  = false;                    // is fan enabled, if so, run functions
  bool manual = false;
};

int attach_timer_fan();

void fade_fans();          // interrupt to change the current value of the fans to approach the target value

bool fans_set_freq();      //interrupt to set the frequency the fans are adjusted




//class for both fan cantrol and temperature monitoring  
class Fans{

  private:

    void enable_temp1();
    void enable_temp2();
    void enable_temp3();
    
    void disable_temp1();
    void disable_temp2();
    void disable_temp3();
    
    void calculate_avg_temp();
    void calculate_fan_speed();
    bool check_for_bad_connections(); //when calculating, check sensors working
  
  public:
    
    Fans(){}
    
    //initialisations of individual items, set pins, test if working if applicable, etc
    void init_fans();
    void init_temp_sensors();            
    int set_fan_speed();
    void enable();
    void disable();   
    void enable_temp();
    void disable_temp();

    void update_temperatures();



};


#endif // Fans_H
