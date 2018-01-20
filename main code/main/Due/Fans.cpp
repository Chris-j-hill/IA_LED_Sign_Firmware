#ifndef Fans_CPP
#define Fans_CPP 

#include "Fans.h"
//#include "Due.h"
//#include "function_declarations.h"
#include "DueTimer.h"



Temp_sensor temp_parameters;
Fan_Struct fan_parameters;        //create fan struct
extern struct Timers timers;

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

bool fans_enabled = false; //to check if initialised sucessfully
bool temp_sensor_enabled = false;





int attach_timer_fan(){
  //attach fan interrupt
    if (!timers.fan_timer_attached & fans_enabled){
      timers.fan_timer_attached = true;       //indicate the timer is attached
      
      Timer2.attachInterrupt(fade_fans);   // attach ISR
      int fail = fans_set_freq();          // set the freq to based on the programmed interval
      
      if (fail !=0){
        Sprintln("Failed to attach fan timer");
        timers.fan_timer_attached = false;       
        return(-1);     //stop code   
      }
           
      Timer2.start();
      Sprintln("Attached fan timer");      
    } 
}


void fade_fans(){          // interrupt to change the current value of the fans to approach the target value

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

int fans_set_freq(){}      //interrupt to set the frequency the fans are adjusted



//methods for fans and temperature sensors

// initialiser functions

int Fans::init_fans() {          //initialise fans and set to starting value
  if (enable_fans)
    this -> enable();
  
  else
    this -> disable();
}

int Fans::init_temp_sensors() {    //code to initialise temp sensors

  //not much to initialise since the pin is not specifically set to output or input
  // just test the sensor responds

  int current_temperature;

  current_temperature = get_temperature(temp_parameters.pin1);
  if (current_temperature == -100) { //if the value is -100 -> error
    Sprint(F("Error Reading temperature sensor 1"));
  }
  else {
    Sprint(F("Read temperature sensor 1 as:"));
    Sprintln(current_temperature);
  }

    current_temperature = get_temperature(temp_parameters.pin2);
  if (current_temperature == -100) { 
    Sprint(F("Error Reading temperature sensor 3"));
  }
  else {
    Sprint(F("Read temperature sensor 2 as:"));
    Sprintln(current_temperature);
  }

    current_temperature = get_temperature(temp_parameters.pin3);
  if (current_temperature == -100) { 
    Sprint(F("Error Reading temperature sensor 3"));
  }
  else {
    Sprint(F("Read temperature sensor 3 as:"));
    Sprintln(current_temperature);
  }
  return(0);
}

int Fans::set_fan_speed() { 

  if (temp_parameters.enabled1)
    this -> get_temperature(temp_parameters.pin1);
  if (temp_parameters.enabled2)
    this -> get_temperature(temp_parameters.pin2);
  if (temp_parameters.enabled3)
    this -> get_temperature(temp_parameters.pin3);

  this -> calculate_avg_temp();
  
  this -> calculate_fan_speed();

}

int Fans::get_temperature(int pin) {  //return the temperature as read by the specified pin

  // call the poll_temperature_sensor to get it to return the data in its registers
  // then get the relavent value of dat[] array and return it
  byte temp_byte0;
  byte temp_byte1;
  byte temp_byte2;
  byte temp_byte3;

  
  poll_temperature_sensor(pin);

  if (pin == temp_parameters.pin1 && temp_parameters.enabled1){
    if (temp_parameters.dat1 [3] > MAX_OPERATING_TEMPERATURE || temp_parameters.dat1 [3] < MIN_OPERATING_TEMPERATURE){
      temp_parameters.bad_connection1 = true;
      return(-1);
    }
    temp_byte0 = temp_parameters.dat1 [0];
    temp_byte1 = temp_parameters.dat1 [1];
    temp_byte2 = temp_parameters.dat1 [2];
    temp_byte3 = temp_parameters.dat1 [3];
}
  else if (pin == temp_parameters.pin2 && temp_parameters.enabled2){
    if (temp_parameters.dat2 [3] > MAX_OPERATING_TEMPERATURE || temp_parameters.dat2 [3] < MIN_OPERATING_TEMPERATURE){
      temp_parameters.bad_connection2 = true;
      return(-1);
    }
    temp_byte0 = temp_parameters.dat2 [0];
    temp_byte1 = temp_parameters.dat2 [1];
    temp_byte2 = temp_parameters.dat2 [2];
    temp_byte3 = temp_parameters.dat2 [3];
}
  else if (pin == temp_parameters.pin3 && temp_parameters.enabled3){
    if (temp_parameters.dat3 [3] > MAX_OPERATING_TEMPERATURE || temp_parameters.dat3 [3] < MIN_OPERATING_TEMPERATURE){
      temp_parameters.bad_connection3 = true;
      return(-1);
    }
    temp_byte0 = temp_parameters.dat3 [0];
    temp_byte1 = temp_parameters.dat3 [1];
    temp_byte2 = temp_parameters.dat3 [2];
    temp_byte3 = temp_parameters.dat3 [3];
}
  else {
  Sprintln(F("Error reading temperature sensor"));
  return (-1);
}
  //display results from function
#ifdef DEBUG
  Sprint ("Current humdity =");
  Serial.print (temp_byte0, DEC); // display the humidity-bit integer;
  Sprint ('.');
  Serial.print (temp_byte1, DEC); // display the humidity decimal places;
  Sprintln ('%');

  Sprint ("Current temperature =");
  Serial.print (temp_byte2, DEC); // display the temperature of integer bits;
  Sprint ('.');
  Serial.print (temp_byte3, DEC); // display the temperature of decimal paces;
  Sprintln ('C');
#endif

return(temp_byte0);

}

int Fans::poll_temperature_sensor (int pin) {    //adapted from this: https://tkkrlab.nl/wiki/Arduino_KY-015_Temperature_and_humidity_sensor_module

  pinMode(pin, OUTPUT);      // confirm that the pin is an output
  digitalWrite (pin, LOW);   // bus down, send start signal, drive line to ground
  delay (30);                   // delay greater than 18ms, so DHT11 start signal can be detected
  digitalWrite (pin, HIGH);  // drive bus to logic high again
  delayMicroseconds (40);       // Wait for DHT11 response


  pinMode (pin, INPUT);               // set pin to recieve data
  while (digitalRead (pin) == HIGH);  // wait for sensor
  delayMicroseconds (80);                // DHT11 response, pulled the bus 80us
  if (digitalRead (pin) == LOW);
  delayMicroseconds (80);                // DHT11 80us after the bus pulled to start sending data

  if (pin = temp_parameters.pin1) {
    for (int i = 0; i < 4; i ++)           // receive temperature and humidity data, the parity bit is not considered
      temp_parameters.dat1[i] = read_temp_data_from_register (pin);               // data to global array 
      Sprintln(F("Read temp sensor 1"));

  }

  else if (pin = temp_parameters.pin2) {
    for (int i = 0; i < 4; i ++)           // receive temperature and humidity data, the parity bit is not considered
      temp_parameters.dat2[i] = read_temp_data_from_register (pin);  
      Sprintln(F("Read temp sensor 2"));
  }

  else if (pin = temp_parameters.pin3) {
    for (int i = 0; i < 4; i ++)           // receive temperature and humidity data, the parity bit is not considered
      temp_parameters.dat3[i] = read_temp_data_from_register (pin);
      Sprintln(F("Read temp sensor 3"));
  }

  else {

    Sprintln(F("Pin not defined as a temperature sensor, define as input to avoid possible damage"));
    pinMode(pin, INPUT);
    return (-1);
  }


  pinMode (pin, OUTPUT);
  digitalWrite (pin, HIGH);            // send data once after releasing the bus, wait for the host to open the next Start signal
  return (0);
}

byte Fans::read_temp_data_from_register (int pin) {   // read the data back from the register as bits and convert to a byte, call this for every byte to read

  byte data;
  for (int i = 0; i < 8; i ++) {
    if (digitalRead (pin) == LOW) {
      while (digitalRead (pin) == LOW);    // wait for 50us
      delayMicroseconds (30);                 // determine the duration of the high level to determine the data is '0 'or '1'
      if (digitalRead (pin) == HIGH)
        data |= (1 << (7 - i));               // high front and low in the post
      while (digitalRead (pin) == HIGH);   // data '1 ', wait for the next one receiver
    }
  }
  return data;   //return one byte of data
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
  fan_parameters.manual = false;    // once enabled again, revert to automatic control
}

void Fans::enable_temp(){
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


void Fans::enable_temp1(){
  temp_parameters.enabled1 = true;  
}
 
void Fans::enable_temp2(){
  temp_parameters.enabled2 = true;  
}
  
void Fans::enable_temp3(){  
  temp_parameters.enabled3 = true;  
}

void Fans::disable_temp(){
#if defined(TEMPERATURE_SENSOR_1_CONNECTED)
  if (!temp_parameters.bad_connection1 || temp_parameters.enabled1)  //not enalbed and not broken connection
    this -> disable_temp1();
#endif

#if defined(TEMPERATURE_SENSOR_2_CONNECTED)
  if (!temp_parameters.bad_connection2 || temp_parameters.enabled2)  //not enalbed and not broken connection
    this -> disable_temp2();
#endif

#if defined(TEMPERATURE_SENSOR_3_CONNECTED)
  if (!temp_parameters.bad_connection3 || temp_parameters.enabled3)  //not enalbed and not broken connection
    this -> disable_temp3();
#endif
}
  
void Fans::disable_temp1(){
  pinMode(temp_parameters.pin1, INPUT_PULLUP);
  temp_parameters.enabled1 = false;
}

void Fans::disable_temp2(){
  pinMode(temp_parameters.pin2, INPUT_PULLUP);
  temp_parameters.enabled2 = false;
}

void Fans::disable_temp3(){
  pinMode(temp_parameters.pin3, INPUT_PULLUP);
  temp_parameters.enabled3 = false;
}


void Fans::calculate_avg_temp(){
  
  int num_enabled = 0;
  temp_parameters.avg = 0;
  if (temp_parameters.enabled1 && !temp_parameters.bad_connection1){
    num_enabled++;
    temp_parameters.avg += temp_parameters.temp1;
  }
  if (temp_parameters.enabled2 && !temp_parameters.bad_connection2){
    num_enabled++;
    temp_parameters.avg += temp_parameters.temp2;
  }

  if (temp_parameters.enabled3 && !temp_parameters.bad_connection3){
    num_enabled++;
    temp_parameters.avg += temp_parameters.temp3;
  }
  temp_parameters.avg /= num_enabled; 
  
  }
void Fans::calculate_fan_speed(){

#if defined(ALLOW_SMART_MANUAL_OVERRIDE)
    if (temp_parameters.avg <= FAN_TURN_ON_TEMPERATURE)
      fan_parameters.target_speed =0;
      
    else if(temp_parameters.avg > FAN_TURN_ON_TEMPERATURE)
      fan_parameters.target_speed = map(temp_parameters.avg, FAN_TURN_ON_TEMPERATURE, FAN_MAX_SPEED_TEMPERATURE, 0, 100);
      
    else  
      fan_parameters.target_speed = 100;
    
    if (fan_parameters.manual && fan_parameters.target_speed<fan_parameters.manual_set_value){ //smart override
      fan_parameters.target_speed = fan_parameters.manual_set_value;
    }

#elif defined(ALLOW_FULL_MANUAL_OVERRIDE) //full manual
    fan_parameters.target_speed = fan_parameters.manual_set_value;  //always this temperature, can turn in menu
     
#endif
    
}

#endif
