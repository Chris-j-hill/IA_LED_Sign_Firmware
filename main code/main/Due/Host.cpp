
#ifndef Host_CPP
#define Host_CPP

#include "Arduino.h"
#include "Host.h"
#include "Config_local.h"
#include "Fans.h"
#include "function_declarations.h"

//access to structs
extern Temp_sensor temp_parameters;
extern Fan_Struct fan_parameters;
extern Timers timers;

void Host::init_serial() {
  Serial.begin(HOST_SERIAL_SPEED);
}

int request_data();
int set_text_string();    //if got new string, save to text_str variable

void Host::check_serial() {   //to read incomming data

  if (Serial.available()>0){
    
    String rx = Serial.readString();
    rx.trim();  //trim off return carraige

    //set message printing mode
    if (rx == "fans") data_to_report = REPORT_FANS;
    else if (rx == "strip") data_to_report = REPORT_LED_STRIP;
    else if (rx == "temp") data_to_report = REPORT_TEMPS;
    else if (rx == "stop") data_to_report = STOP_REPORT;
   
  }
  

}

void set_debug_mode();   //set the mode, what data to print
void transmit_modes();   //send back available modes and setting commands
void stop_debug_mode();

void Host::print_messages() {
  static byte previously_reporting = 0; // reset header counter if just changed printing messages
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

void Host::print_temps() {}
void Host::print_led_strip() {}
#endif // Host_CPP
