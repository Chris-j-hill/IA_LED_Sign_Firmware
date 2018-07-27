#ifndef Fans_CPP
#define Fans_CPP

#include "Fans.h"
//#include "Due.h"
//#include "function_declarations.h"
#include "DueTimer.h"
//#include "Due_DHT11.h"
#include "Due_Libraries.h"

dht11 Temp1;    //objects for using dht library functions
dht11 Temp2;
dht11 Temp3;

Temp_sensor temp_parameters;
Fan_Struct fan_parameters;        //create fan struct
extern struct Timers timers;
extern 
#ifdef ENABLE_FANS
bool enable_fans = true;   //initialise on startup?
#else
bool enable_fans = false;
#endif

#ifdef ENABLE_TEMPERATURE_SENSORS
bool enable_temp_sensor = true;
#else
bool enable_temp_sensor = false;
#endif

#define FAN_TIMER Timer2
#define FAN_TIMER_INTERRUPT TC2_IRQn

int attach_timer_fan() {
  //attach fan interrupt
  if (!timers.fan_timer_attached & fan_parameters.enabled) {
    timers.fan_timer_attached = true;       //indicate the timer is attached

    FAN_TIMER.attachInterrupt(fade_fans);   // attach ISR
    bool fail = fans_set_freq();          // set the freq to based on the programmed interval

    if (fail) {
      timers.fan_timer_attached = false;
      fan_parameters.enabled = false;
      return (-1);    //stop code
    }

    FAN_TIMER.start();    
    NVIC_SetPriority (FAN_TIMER_INTERRUPT, FAN_PRIORITY);  //set priority of interrupt, see priority definitions for details and links

  }
}


void fade_fans() {         // interrupt to change the current value of the fans to approach the target value

  if (fan_parameters.enabled) { //only write values if enabled

    //if theres a difference, do something, ohterwise skip to end
    if (fan_parameters.target_speed != fan_parameters.current_speed) {
      if (fan_parameters.current_speed + fan_parameters.change_increment < fan_parameters.target_speed) { //if after adding increment is less than target, increase current value and write to pin
        fan_parameters.current_speed = fan_parameters.current_speed + fan_parameters.change_increment;
        analogWrite(fan_parameters.pin, fan_parameters.current_speed);
      }

      else if (fan_parameters.current_speed - fan_parameters.change_increment > fan_parameters.target_speed) { //if after subtracting incrementing is grater than target, decrease current value and write to pin
        fan_parameters.current_speed = fan_parameters.current_speed - fan_parameters.change_increment;
        analogWrite(fan_parameters.pin, fan_parameters.current_speed);
      }

      else {      //otherwise we must be close to target, set equal
        fan_parameters.current_speed = fan_parameters.target_speed;
        analogWrite(fan_parameters.pin, fan_parameters.current_speed);
      }
    }
  }
}

bool fans_set_freq() {     //interrupt to set the frequency the fans are adjusted
  FAN_TIMER.setPeriod(fan_parameters.fan_change_interval * 1000);
  //FAN_TIMER.start();   //<- not needed unless resetting freq after initial setup
  return (FAN_TIMER.getPeriod() != fan_parameters.fan_change_interval * 1000);
}



//methods for fans and temperature sensors

// initialiser functions

void Fans::init_fans() {          //initialise fans and set to starting value
  if (enable_fans)
    this -> enable();

  else
    this -> disable();
}

void Fans::init_temp_sensors() {    //code to initialise temp sensors

  if (enable_temp_sensor) {
    int current_temperature;

    if (temp_parameters.enabled1) {
      Temp1.init(temp_parameters.pin1); //attach sensor
      Temp1.scan(); //get values from this sensor
      current_temperature = Temp1.get_temperature();  //read value out from class variable
      if (current_temperature == 0) { //if the value is 0 -> error
        Sprintln(F("Error Reading temperature sensor 1"));
        temp_parameters.bad_connection1 = true;
      }
      else {
        Sprint(F("Read temperature sensor 1 as:"));
        Sprintln(current_temperature);
        temp_parameters.bad_connection3 = false;
      }
    }

    if (temp_parameters.enabled2) {
      Temp2.init(temp_parameters.pin2);
      Temp2.scan();
      current_temperature = Temp2.get_temperature();
      if (current_temperature == 0) {
        Sprintln(F("Error Reading temperature sensor 2"));
        temp_parameters.bad_connection2 = true;
      }
      else {
        Sprint(F("Read temperature sensor 2 as:"));
        Sprintln(current_temperature);
        temp_parameters.bad_connection3 = false;
      }
    }

    if (temp_parameters.enabled3) {
      Temp3.init(temp_parameters.pin3);
      Temp3.scan();
      current_temperature = Temp3.get_temperature();
      if (current_temperature == 0) {
        Sprintln(F("Error Reading temperature sensor 3"));
        temp_parameters.bad_connection3 = true;
      }
      else {
        Sprint(F("Read temperature sensor 3 as:"));
        Sprintln(current_temperature);
        temp_parameters.bad_connection3 = false;
      }
    }
  }
}

int Fans::set_fan_speed() {

  this -> calculate_avg_temp();

  this -> calculate_fan_speed();

}

void Fans::enable() {

  pinMode(fan_parameters.pin, OUTPUT);
  fan_parameters.enabled = true;
  if (!timers.fan_timer_attached) {
    attach_timer_fan();
  }
}

void Fans::disable() {
  fan_parameters.enabled = false;       //disable interrupt analogWrite
  pinMode(fan_parameters.pin, INPUT_PULLUP);   //disable pin
  fan_parameters.manual = false;        // once enabled again, revert to automatic control
}

void Fans::enable_temp() {
#if defined(TEMPERATURE_SENSOR_1_CONNECTED)
  if (!temp_parameters.bad_connection1 && !temp_parameters.enabled1)  //not enalbed and not broken connection
    this -> enable_temp1();
#endif

#if defined(TEMPERATURE_SENSOR_2_CONNECTED)
  if (!temp_parameters.bad_connection2 && !temp_parameters.enabled2)  //not enalbed and not broken connection
    this -> enable_temp2();
#endif

#if defined(TEMPERATURE_SENSOR_3_CONNECTED)
  if (!temp_parameters.bad_connection3 && !temp_parameters.enabled3)  //not enalbed and not broken connection
    this -> enable_temp3();
#endif
}


void Fans::enable_temp1() {
  temp_parameters.enabled1 = true;
}

void Fans::enable_temp2() {
  temp_parameters.enabled2 = true;
}

void Fans::enable_temp3() {
  temp_parameters.enabled3 = true;
}

void Fans::disable_temp() {
#if defined(TEMPERATURE_SENSOR_1_CONNECTED)
  if (!temp_parameters.bad_connection1 || temp_parameters.enabled1)  //not enabled and not broken connection
    this -> disable_temp1();
#endif

#if defined(TEMPERATURE_SENSOR_2_CONNECTED)
  if (!temp_parameters.bad_connection2 || temp_parameters.enabled2)  //not enabled and not broken connection
    this -> disable_temp2();
#endif

#if defined(TEMPERATURE_SENSOR_3_CONNECTED)
  if (!temp_parameters.bad_connection3 || temp_parameters.enabled3)  //not enabled and not broken connection
    this -> disable_temp3();
#endif
}

void Fans::disable_temp1() {
  pinMode(temp_parameters.pin1, INPUT_PULLUP);
  temp_parameters.enabled1 = false;
}

void Fans::disable_temp2() {
  pinMode(temp_parameters.pin2, INPUT_PULLUP);
  temp_parameters.enabled2 = false;
}

void Fans::disable_temp3() {
  pinMode(temp_parameters.pin3, INPUT_PULLUP);
  temp_parameters.enabled3 = false;
}


void Fans::calculate_avg_temp() {

  int num_enabled = 0;
  int temp_avg = 0;
  temp_parameters.avg = 0;
  if (temp_parameters.enabled1 && !temp_parameters.bad_connection1) {
    num_enabled++;
    temp_avg += temp_parameters.temp1;
  }
  if (temp_parameters.enabled2 && !temp_parameters.bad_connection2) {
    num_enabled++;
    temp_avg += temp_parameters.temp2;
  }

  if (temp_parameters.enabled3 && !temp_parameters.bad_connection3) {
    num_enabled++;
    temp_avg += temp_parameters.temp3;
  }
  temp_parameters.avg = temp_avg / num_enabled;
  Sprint("Average temp reading:");
  Sprintln(temp_parameters.avg);
}


void Fans::calculate_fan_speed() {
  //if no sensors reporting, set to max speed for safety
  if (check_for_bad_connections())
    fan_parameters.target_speed = 255;

  // else allow manual or smart speed setting
  else {
#if defined(ALLOW_SMART_MANUAL_OVERRIDE)
    if (temp_parameters.avg <= FAN_TURN_ON_TEMPERATURE)
      fan_parameters.target_speed = 0;

    else if (temp_parameters.avg > FAN_TURN_ON_TEMPERATURE)
      fan_parameters.target_speed = map(temp_parameters.avg, (FAN_TURN_ON_TEMPERATURE - 1), FAN_MAX_SPEED_TEMPERATURE, fan_parameters.fan_minimum, 255);

    else
      fan_parameters.target_speed = fan_parameters.fan_minimum;

    if (fan_parameters.manual && fan_parameters.target_speed < fan_parameters.manual_set_value) { //smart override
      fan_parameters.target_speed = fan_parameters.manual_set_value;
    }

#else //full manual
    fan_parameters.target_speed = fan_parameters.manual_set_value;  //always this temperature, can turn in menu

#endif
  }
}


bool Fans::check_for_bad_connections() {
  //if all disabled
  byte bad_sensor_count = 0;

  if (!temp_parameters.enabled1)  //if disabled
    bad_sensor_count++;
  else if (temp_parameters.bad_connection1) // or enabled and not connected correctly
    bad_sensor_count++;

  if (!temp_parameters.enabled2)
    bad_sensor_count++;
  else if (temp_parameters.bad_connection2)
    bad_sensor_count++;

  if (!temp_parameters.enabled3)
    bad_sensor_count++;
  else if (temp_parameters.bad_connection3)
    bad_sensor_count++;

  if (bad_sensor_count == 3)  //if all temp sensors fail set to max
    return true;
  return false;   // one or more sensors ok

}


void Fans::update_temperatures() {
  static uint32_t last_temp_update_time = millis();
  byte newest_value;

  if (millis() > last_temp_update_time + TEMP_UPDATE_DELAY_PERIOD) {
    if (temp_parameters.enabled1) {
      Temp1.scan();
      newest_value = Temp1.get_temperature();
      if (newest_value != 0) {
        temp_parameters.temp1 = newest_value;
        temp_parameters.bad_connection1 = false;
      }
      else
        temp_parameters.bad_connection1 = true;
    }

    if (temp_parameters.enabled2) {
      Temp2.scan();
      newest_value = Temp2.get_temperature();
      if (newest_value != 0) {
        temp_parameters.temp2 = newest_value;
        temp_parameters.bad_connection2 = false;
      }
      else
        temp_parameters.bad_connection2 = true;
    }

    if (temp_parameters.enabled3) {
      Temp3.scan();
      newest_value = Temp3.get_temperature();

      if (newest_value != 0) {
        temp_parameters.temp3 = newest_value;
        temp_parameters.bad_connection3 = false;
      }
      else
        temp_parameters.bad_connection3 = true;
    }

  }
}
#endif
