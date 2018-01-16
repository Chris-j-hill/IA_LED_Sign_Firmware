#ifndef Fans_CPP
#define Fans_CPP 

#include "Fans.h"
#include "Due.h"



//methods for fans and temperature sensors

// initialiser functions

int due_class::init_fans() {          //initialise fans and set to starting value
  pinMode(fan.fan_pin, OUTPUT);
  
  Sprintln(F("\t Set fans to value..."));
  int fail = writeFanSpeed(fan.fan_target_speed);

  if (fail != 0) {
    Sprintln(F("Failed to write initial fan speed"));
    return (-1);
  }
  else
    return (0);

}

int due_class::init_temp_sensors() {    //code to initialise temp sensors

  //not much to initialise since the pin is set to output and input to request and recieve data
  // just test the sensor responds

  int fail;

  fail = temp_read_sensor(temp.pin1);

  if (fail == -100) { //if the value is -100 -> error
  return(-1);
  }
  else {
    Sprint(F("Read temp1 as:"));
    Sprintln(fail);
  }
  return(0);
}

int due_class::writeFanSpeed(int newValue) {  //function to fade in fans from current value to new value

  // function will set new value, and attach interrupt if not attached already
  // when target value is reached, isr will set ISR_attached to false and isr will be detached at the end of the loop
}

int due_class::get_temperature(int pin) {  //return the temperature as read by the specified pin

  // call the poll_temperature_sensor to get it to return the data in its registers
  // then get the relavent value of dat[] array and return it


  poll_temperature_sensor(pin);      //update registers

  //display results from function
  #ifdef (DEBUG)
  if (pin == TEMP1){
  temp0 = temp.dat1 [0];
  temp1 = temp.dat1 [1];
  temp2 = temp.dat1 [2];
  temp3 = temp.dat1 [3];
}
  else if (pin == TEMP2){
   temp0 = temp.dat2 [0];
  temp1 = temp.dat2 [1];
  temp2 = temp.dat2 [2];
  temp3 = temp.dat2 [3];
}
  else if (pin == TEMP3){
 temp0 = temp.dat3 [0];
  temp1 = temp.dat3 [1];
  temp2 = temp.dat3 [2];
  temp3 = temp.dat3 [3];
}
 
  Sprint ("Current humdity =");
  Serial.print (temp0, DEC); // display the humidity-bit integer;
  Sprint ('.');
  Serial.print (temp1, DEC); // display the humidity decimal places;
  Sprintln ('%');

  Sprint ("Current temperature =");
  Serial.print (temp2, DEC); // display the temperature of integer bits;
  Sprint ('.');
  Serial.print (temp3, DEC); // display the temperature of decimal paces;
  Sprintln ('C');
  #endif

 // Sprintln(F("WARNING: function not fully implemented, only reading sensor 1")); 

}

int due_class::poll_temperature_sensor (int pin) {    //adapted from this: https://tkkrlab.nl/wiki/Arduino_KY-015_Temperature_and_humidity_sensor_module

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

  if (pin = TEMP1) {
    for (int i = 0; i < 4; i ++)           // receive temperature and humidity data, the parity bit is not considered
      temp.dat1[i] = read_temp_data_from_register (pin);               // data to global array

    
      Sprintln(F("Read temp sensor 1"));

  }

  else if (pin = TEMP2) {
    for (int i = 0; i < 4; i ++)           // receive temperature and humidity data, the parity bit is not considered
      temp.dat2[i] = read_temp_data_from_register (pin);
    
      Sprintln(F("Read temp sensor 2"));
  }

  else if (pin = TEMP3) {
    for (int i = 0; i < 4; i ++)           // receive temperature and humidity data, the parity bit is not considered
      temp.dat3[i] = read_temp_data_from_register (pin);

   
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

byte due_class::read_temp_data_from_register (int pin) {   // read the data back from the register as bits and convert to a byte, call this for every byte to read

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

#endif
