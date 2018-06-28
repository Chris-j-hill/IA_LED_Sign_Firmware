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

byte screen_brightness = 100;

LDR_Struct light_sensor_parameters;
Current_Meter_Struct current_meter_parameters;

int Light_Sensor::init_LDR() {

  if (enable_LDR) {
    this -> enable();
    this -> read_sensor(1); //check connection
  }
  else
    this -> disable();
  this -> read_sensor(2);
}

void Light_Sensor::read_sensors() {

  delayMicroseconds(250);
  this -> read_sensor(1);
  delayMicroseconds(250);
  this -> read_sensor(2);

  this -> avg_sensor_result();

}


byte Light_Sensor::calculate_target_brightness() {

  byte temp_brightness = map(light_sensor_parameters.avg_reading, 0, 255, 0, 100);
  if (current_meter_parameters.total > current_meter_parameters.max_current_limit) {
    temp_brightness = temp_brightness - 2;      //reduce by 2% every loop until under current limit

  }
  //require difference between target_brightness and current birghtness to be above threshold before update gets pushed to megas
  if (abs(screen_brightness - temp_brightness) > SCREEN_BRIGHTNESS_UPDATE_THRESHOLD)
    return temp_brightness;
  else
    return screen_brightness;

}



void Light_Sensor::enable() {
  if (!light_sensor_parameters.enabled1)
    this -> enable1();
  if (!light_sensor_parameters.enabled2)
    this -> enable2();

}

void Light_Sensor::disable() {

  this -> disable1();
  this -> disable2();

}

void Light_Sensor::enable1() {

  pinMode(light_sensor_parameters.pin1, INPUT);
  light_sensor_parameters.enabled1 = true;
}

void Light_Sensor::enable2() {

  pinMode(light_sensor_parameters.pin2, INPUT);
  light_sensor_parameters.enabled2 = true;
}

void Light_Sensor::disable1() {
  light_sensor_parameters.enabled1 = false;
}

void Light_Sensor::disable2() {
  light_sensor_parameters.enabled1 = false;
}

void Light_Sensor::read_sensor(byte sensor) {       // read the average value of the LDR's, if one fails to read correctly or large disparity fail returned

  int temp_reading = 0;
  if (sensor == 1 && light_sensor_parameters.enabled1) {

    //Serial.print("Read sensor 1: ");
    temp_reading = map(analogRead(light_sensor_parameters.pin1), 1, 1022, light_sensor_parameters.config_min1, (uint16_t)65025 / light_sensor_parameters.config_max1);
    light_sensor_parameters.reading1 = constrain(temp_reading, 0, 255); //constrain to limits, should retain infor of shorts or open circuit
    //Serial.println(light_sensor_parameters.reading1);

    if (light_sensor_parameters.reading1 == 0 || light_sensor_parameters.reading1 == 255) {
      Sprintln(F("From Light_Sensor.read_sensor: \t Problem reading light sensor 1, check wiring"));
      light_sensor_parameters.bad_connection1 = true;
    }
    else
      light_sensor_parameters.bad_connection1 = false;
  }

  else if (sensor == 2 && light_sensor_parameters.enabled2) {

    //Serial.print("Read sensor 2: ");
    temp_reading = map(analogRead(light_sensor_parameters.pin2), 1, 1022, light_sensor_parameters.config_min2, light_sensor_parameters.config_max2);
    light_sensor_parameters.reading2 = constrain(temp_reading, 0, 255);
    //Serial.println(light_sensor_parameters.reading2);

    if (light_sensor_parameters.reading2 == 0 || light_sensor_parameters.reading2 == 255) {
      Sprintln(F("From Light_Sensor.read_sensor: \t Problem reading light sensor 2, check wiring"));
      light_sensor_parameters.bad_connection2 = true;
    }
    else

      light_sensor_parameters.bad_connection2 = false;
  }
}


void Light_Sensor::avg_sensor_result() {

  static byte ring_buffer_index = 0; //index in ring buffer
  static int ring_buffer_total = 0;
  static byte ring_buffer[RING_BUFFER_LENGTH] = {0}; //ring buffer used to calculate average
  byte this_avg_reading = 0;

  if (!light_sensor_parameters.bad_connection1 && !light_sensor_parameters.bad_connection2) { //if connections fine, get average
    if (abs(light_sensor_parameters.reading1 - light_sensor_parameters.reading2) >= light_sensor_parameters.large_disparity) { //if large disparity between sensor readings, uses the higher, possibly one covered

      Sprintln(F("From LDR_check_value: \t Large disparity in sensor readings, return higher"));
      Sprint(F("sensor 1: "));
      Sprint(light_sensor_parameters.reading1);
      Sprint(F("\t sensor 2: "));
      Sprintln(light_sensor_parameters.reading2);

      if (light_sensor_parameters.reading1 > light_sensor_parameters.reading2)
        this_avg_reading = light_sensor_parameters.reading1;

      else
        this_avg_reading = light_sensor_parameters.reading2;
    }
    else {
      this_avg_reading = (light_sensor_parameters.reading1 + light_sensor_parameters.reading2) >> 1;
      Sprintln(F("From Light_Sensor.avg_sensor_result: Both sensor readings good, return average"));
      Sprint(F("Average reading: "));
      Sprintln(this_avg_reading);
    }
  }

  else { // return good result if one one available
    if (!light_sensor_parameters.bad_connection1 && light_sensor_parameters.bad_connection2) {  //if one sensor is usable just return the value
      this_avg_reading = light_sensor_parameters.reading1;
      Sprintln(F("From Light_Sensor.avg_sensor_result: \t Return sensor 1 reading"));
    }

    else if (light_sensor_parameters.bad_connection1 && !light_sensor_parameters.bad_connection2) {
      this_avg_reading = light_sensor_parameters.reading2;
      Sprintln(F("From Light_Sensor.avg_sensor_result: \t Return sensor 2 reading"));
    }

    else {  //if neither usable set avg as highest value
      this_avg_reading = 255;
      Sprintln(F("From Light_Sensor.avg_sensor_result: \t Error reading both sensors, check connections"));
    }

  }

  ring_buffer_total -= ring_buffer[ring_buffer_index];    //subtract off oldest value
  ring_buffer_total += this_avg_reading;                  //add new value to sum
  ring_buffer[ring_buffer_index] += this_avg_reading;     //log new value in array
  light_sensor_parameters.avg_reading = ring_buffer_total >> RING_BUFFER_SHIFT; //divide by number of indexes for rolling avg

  ring_buffer_index = (ring_buffer_index + 1) % RING_BUFFER_LENGTH; //move index
}

void Light_Sensor::get_readings() {
  static int previous_ldr_read_time = millis();

  if (millis() > LDR_UPDATE_PERIOD + previous_ldr_read_time){
    previous_ldr_read_time = millis();
    read_sensors();
  }
}
void Light_Sensor::LDR_calibration() {        // code to set ambient brightness, max and min values
  //
  //  // since the lrd likely will never read near the rail voltage when in light or darkness
  //  // ambient, max and min will need to be calibrated. this could be calculated roughly too, based on
  //  // approximate series resistor values, but will vary depending on component tollerances
  //  // TO DO: 1) add screen interface not just serial feedback
  //  //        2) configure sensors individually not just the average, in case using non identical components
  //
  //    char accept_val = 'w';    //variable to detect if serial input given, if val == x -> quit
  //    //                                          id val == s -> skip current
  //    int reading1;             // variables to store readings from ldr's
  //    int reading2;
  //    // configure ambient
  //    Sprintln (F("LDR configuration tool:"));
  //
  //    // set ambient brightness value
  //    Sprint(F("Set the ambient brightness, make sure neither ldr's covered"));
  //    while (Serial.available() == 0) { //wait until some serial data arrives
  //      // give prompt on value read from pins
  //      delay(500);
  //      Sprint(F("LDR 1 value: "));
  //      Sprint(analogRead(LDR.pin1));
  //      Sprint(F("LDR 2 value: "));
  //      Sprintln(analogRead(LDR.pin2));
  //    }
  //
  //    accept_val = Serial.read();    //read one byte
  //    Serial.flush();                  //empty buffer
  //    if (accept_val == 'x')
  //      return (0);                      // if user quits, return 0
  //    if (accept_val != 's') {      // if not skipping this stage
  //      reading1 = analogRead(LDR.pin1);
  //      reading2 = analogRead(LDR.pin2);
  //      LDR.ambient = (reading1 + reading2) / 2; //set average as ambient
  //      if (abs(reading1 - reading2) >= LDR.large_disparity) //if large disparity, give warning
  //        Sprintln(F("Warning, large disparity on readings"));
  //    }
  //
  //
  //    //set maximum brightness value
  //
  //    Sprint(F("Set the maximum brightness, make sure neither ldr's covered, shine light on ldr 1"));
  //    while (Serial.available() == 0) { //wait until some serial data arrives
  //      // give prompt on value read from pin, help to identify pin
  //      delay(500);
  //      Sprint(F("LDR 1 value: "));
  //      Sprintln(analogRead(LDR.pin1));
  //    }
  //
  //    accept_val = Serial.read();
  //    Serial.flush();
  //    if (accept_val == 'x')
  //      return (0);                      // if user quits, return 0
  //
  //    if (accept_val != 's') {
  //
  //      LDR.max_reading1 = analogRead(LDR.pin1);
  //
  //      Sprint(F("Set the maximum brightness, make sure neither ldr's covered, shine light on ldr 2"));
  //      while (Serial.available() == 0) { //wait until some serial data arrives
  //        // give prompt on value read from pin, help to identify pin
  //        delay(500);
  //        Sprint(F("LDR 2 value: "));
  //        Sprintln(analogRead(LDR.pin2));
  //      }
  //
  //      accept_val = Serial.read();
  //      Serial.flush();
  //      if (accept_val == 'x')
  //        return (0);                      // if user quits, return 0
  //      LDR.max_reading2 = analogRead(LDR.pin2);
  //      if (abs(LDR.max_reading1 - LDR.max_reading2) >= LDR.large_disparity)
  //        Sprintln(F("Warning, large disparity on readings"));
  //    }
  //
  //    //set minimum brightness value
  //    Sprint(F("Set the minimum brightness, cover ldr 1"));
  //    while (Serial.available() == 0) { //wait until some serial data arrives
  //      // give prompt on value read from pin, help to identify pin
  //      delay(500);
  //      Sprint(F("LDR 1 value: "));
  //      Sprintln(analogRead(LDR.pin1));
  //    }
  //    accept_val = Serial.read();
  //    Serial.flush();
  //    if (accept_val == 'x')
  //      return (0);                      // if user quits, return 0
  //    if (accept_val != 's') {
  //      LDR.min_reading1 = analogRead(LDR.pin1);
  //      Sprint(F("Set the maximum brightness, cover ldr 2"));
  //      while (Serial.available() == 0) { //wait until some serial data arrives
  //        // give prompt on value read from pin, help to identify pin
  //        delay(500);
  //        Sprint(F("LDR 2 value: "));
  //        Sprintln(analogRead(LDR.pin2));
  //      }
  //      accept_val = Serial.read();
  //      Serial.flush();
  //      if (accept_val == 'x')
  //        return (0);                      // if user quits, return 0
  //      LDR.min_reading2 = analogRead(LDR.pin2);
  //      if (abs(LDR.min_reading1 - LDR.min_reading2) >= LDR.large_disparity)
  //        Sprintln(F("Warning, large disparity on readings"));
  //    }
  //  }

}


#endif  // Current_Control_CPP
