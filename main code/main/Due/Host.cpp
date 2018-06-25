
#ifndef Host_CPP
#define Host_CPP

#include "Arduino.h"
#include "Host.h"
#include "Config_local.h"
#include "Fans.h"
#include "function_declarations.h"
#include "Led_strip.h"

//access to structs
extern Temp_sensor temp_parameters;
extern Fan_Struct fan_parameters;
extern Timers timers;
extern Led_Strip_Struct led_strip_parameters;

void Host::init_serial() {
  Serial.begin(HOST_SERIAL_SPEED);
}

int request_data();
int set_text_string();    //if got new string, save to text_str variable

void Host::check_serial() {   //to read incomming data

  if (Serial.available() > 0) {

    String rx = Serial.readString();
    rx.trim();  //trim off return carraige

    //set message printing mode
    if (rx == "fans") data_to_report = REPORT_FANS;
    else if (rx == "strip") data_to_report = REPORT_LED_STRIP;
    else if (rx == "temp") data_to_report = REPORT_TEMPS;
    else if (rx == "stop") data_to_report = STOP_REPORT;
    else if (rx == "ldr") led_strip_parameters.target_brightness += 128;
  }


}

void set_debug_mode();   //set the mode, what data to print
void transmit_modes();   //send back available modes and setting commands
void stop_debug_mode();

void Host::print_messages() {
  static uint16_t last_message_print_time;
  static byte previously_reporting = 0; // reset header counter if just changed printing messages

  if (millis() > last_message_print_time + MEGGAGE_DELAY_PERIOD) { // check if specified time delay elapsed

    last_message_print_time = millis();

    switch (data_to_report) {
      case STOP_REPORT:
        break;
      case REPORT_FANS:
        print_fans();
        break;
      case REPORT_TEMPS:
        print_temps();
        break;
      case REPORT_LED_STRIP:
        print_led_strip();
        break;

      default: break;
    }

    // increment counter for printing header info message
    if (previously_reporting != data_to_report)
      header_print_counter = 0;
    else
      header_print_counter += HEADER_PRINT_INCREMENT;

    previously_reporting = data_to_report;
  }
}




//_________Printing Functions__________

void Host::print_fans() {

  if (header_print_counter == 0) {
    Serial.println();
    Serial.println(F("Pin \t ISR attached \t State \t Current Speed \t Target Speed \t Average Temp"));
  }

  Serial.print(fan_parameters.pin);
  Serial.print(F("\t "));

  if (timers.fan_timer_attached)
    Serial.print(F("Yes"));
  else
    Serial.print(F("No"));

  Serial.print(F("\t\t "));

  if (fan_parameters.enabled)
    Serial.print(F("On"));

  else
    Serial.print(F("Off"));

  Serial.print(F("\t "));
  Serial.print(fan_parameters.current_speed);
  Serial.print(F("\t\t "));
  Serial.print(fan_parameters.target_speed);
  Serial.print(F("\t\t "));
  Serial.print(temp_parameters.avg);
  Serial.println();

}

void Host::print_temps() {

  if (header_print_counter == 0) {
    Serial.println();
    Serial.println(F("Pins \t     Sensors Enabled \t Good Connection \t Current Temp Readings \t Average Temp"));
  }

  Serial.print(temp_parameters.pin1);
  Serial.print(F(" "));

  Serial.print(temp_parameters.pin2);
  Serial.print(F(" "));

  Serial.print(temp_parameters.pin3);
  Serial.print(F("     "));

  if (temp_parameters.enabled1)
    Serial.print(F("Y "));
  else
    Serial.print(F("N "));

  if (temp_parameters.enabled2)
    Serial.print(F("Y "));
  else
    Serial.print(F("N "));

  if (temp_parameters.enabled3)
    Serial.print(F("Y\t"));
  else
    Serial.print(F("N\t"));

  Serial.print(F("\t "));

  if (!temp_parameters.enabled1)
    Serial.print(F("- "));
  else {
    if (temp_parameters.bad_connection1)
      Serial.print(F("N "));
    else
      Serial.print(F("Y "));
  }

  if (!temp_parameters.enabled2)
    Serial.print(F("- "));
  else {
    if (temp_parameters.bad_connection2)
      Serial.print(F("N "));
    else
      Serial.print(F("Y "));
  }

  if (!temp_parameters.enabled3)
    Serial.print(F("- "));
  else {
    if (temp_parameters.bad_connection3)
      Serial.print(F("N\t"));
    else
      Serial.print(F("Y\t"));
  }
  Serial.print(F("\t\t "));

  Serial.print(temp_parameters.temp1);
  Serial.print(F(" "));

  Serial.print(temp_parameters.temp2);
  Serial.print(F(" "));

  Serial.print(temp_parameters.temp3);
  Serial.print(F("\t\t"));

  Serial.print(temp_parameters.avg);
  Serial.println();


}
void Host::print_led_strip() {

  if (header_print_counter == 0) {
    Serial.println();
    Serial.println(F("Pin \tLED Strip Enabled \tCurrent Brightness \tTarget Brightness \tUse Fast ISR Interval"));
  }
  Serial.print(led_strip_parameters.pin);
  Serial.print(F("\t"));

  if (led_strip_parameters.enabled)
    Serial.print(F("Yes\t\t\t"));
  else
    Serial.print(F("No\t\t\t"));

  Serial.print(led_strip_parameters.current_brightness);
  Serial.print(F("\t\t\t"));

  Serial.print(led_strip_parameters.target_brightness);
  Serial.print(F("\t\t\t"));
  if (led_strip_parameters.fast_interval)
    Serial.println(F("Yes"));
  else
    Serial.println(F("No"));

}
#endif // Host_CPP
