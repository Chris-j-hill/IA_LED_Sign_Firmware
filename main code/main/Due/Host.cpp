
#ifndef Host_CPP
#define Host_CPP

#include "Arduino.h"
#include "Host.h"
#include "Config_local.h"
#include "Fans.h"
#include "function_declarations.h"
#include "Led_strip.h"
#include "Encoder.h"


//access to structs
extern Temp_sensor temp_parameters;
extern Fan_Struct fan_parameters;
extern Timers timers;
extern Led_Strip_Struct led_strip_parameters;
extern Encoder_Struct encoder_parameters;
extern Button_Struct button_parameters;


Serial_Sub_Menu serial_sub_menu_items;


String space_colon_string PROGMEM = " : ";
String dash_space_string PROGMEM = "- ";
String tab_string PROGMEM = "\t";
String space_string PROGMEM = " ";

inline void space() {
  Serial.print(space_string);
};

inline void tab() {
  Serial.print(tab_string);
};

inline void space_colon() {
  Serial.print(space_colon_string);
};

inline void dash_space() {
  Serial.print(dash_space_string);
};

inline void print_command_name(String command_data) {
  Serial.print(command_data);
  space_colon();
};

inline void print_new_command_name(String command_data) {
  Serial.println(F("New Value"));
  print_command_name(command_data);
};


void Host::init_serial() {
  Serial.begin(HOST_SERIAL_SPEED);
}

void Host::check_serial() {   //to read incomming data

  if (Serial.available() > 0) {

    String rx = Serial.readString();
    rx.trim();  //trim off return carraige

    //set message printing mode
    if      (rx == serial_sub_menu_items.items[REPORT_FANS])          data_to_report = REPORT_FANS;
    else if (rx == serial_sub_menu_items.items[REPORT_LED_STRIP])     data_to_report = REPORT_LED_STRIP;
    else if (rx == serial_sub_menu_items.items[REPORT_TEMPS])         data_to_report = REPORT_TEMPS;
    else if (rx == serial_sub_menu_items.items[STOP_REPORT])          data_to_report = STOP_REPORT;
    else if (rx == serial_sub_menu_items.items[REPORT_LDRS])          data_to_report = REPORT_LDRS;
    else if (rx == serial_sub_menu_items.items[REPORT_MENU_TREE])     data_to_report = REPORT_MENU_TREE;
    else if (rx == serial_sub_menu_items.items[REPORT_ENCODER])       data_to_report = REPORT_ENCODER;
    else if (rx == serial_sub_menu_items.items[REPORT_BUTTON])        data_to_report = REPORT_BUTTON;


    else { //input might be to directly change value
      serial_sub_menu(rx);
    }
  }
}

void Host::serial_sub_menu(String rx) {

  //split input into strings based on these delimiters
  byte first_delimiter = rx.indexOf(' ');
  byte second_delimiter = rx.indexOf(' ', first_delimiter + 1);
  byte third_delimiter = rx.indexOf(' ', second_delimiter + 1);

  String data_set = rx.substring(0, first_delimiter);
  String command_mode = rx.substring(first_delimiter + 1, second_delimiter);
  String command_data = rx.substring(second_delimiter + 1, third_delimiter);
  String command_arg =  rx.substring(third_delimiter + 1); //value to write as a string

  int value = command_arg.toInt();  //value as an int


  //    Serial.print("data set: ");
  //    Serial.println(data_set);
  //
  //    Serial.print("Value: ");
  //    Serial.println(value);
  //    Serial.println(command_mode);
  //    Serial.println(command_data);
  //    Serial.println(command_arg);

  if (command_mode == "-h") {
    data_to_report = data_set_LUT(data_set);  //convert string to data to display
    print_help_options();
    data_to_report = STOP_REPORT;   // stop printing to keep options on screen
  }

  else if (command_mode == "-r") {
    byte temp = data_to_report;
    data_to_report = data_set_LUT(data_set);
    return_data(command_data);
    data_to_report = temp;    //return to whatever we were doing before
  }

  else if (command_mode == "-w") {
    byte temp = data_to_report;
    data_to_report = data_set_LUT(data_set);
    write_data(command_data, value);
    data_to_report = temp;    //return to whatever we were doing before
  }
}


byte Host::data_set_LUT(String data_set) {

  if (data_set == serial_sub_menu_items.data_elements[0][REPORT_FANS])
    return REPORT_FANS;
  else if (data_set == serial_sub_menu_items.data_elements[0][REPORT_TEMPS])
    return REPORT_TEMPS;
  else if (data_set == serial_sub_menu_items.data_elements[0][REPORT_LED_STRIP])
    return REPORT_LED_STRIP;
  else if (data_set == serial_sub_menu_items.data_elements[0][REPORT_MENU_TREE])
    return REPORT_MENU_TREE;
  else if (data_set == serial_sub_menu_items.data_elements[0][REPORT_LDRS])
    return REPORT_LDRS;
  else if (data_set == serial_sub_menu_items.data_elements[0][REPORT_ENCODER])
    return REPORT_ENCODER;
  else if (data_set == serial_sub_menu_items.data_elements[0][REPORT_BUTTON])
    return REPORT_BUTTON;

}

void Host::print_help_options() {

  Serial.println();
  Serial.println(F("Command \t   Description"));

  //loop through commands
  for (int i = 0; i < serial_sub_menu_items.active_elements_by_row[data_to_report]; i++) {
    Serial.print(serial_sub_menu_items.data_elements[data_to_report][i]);

    //tab out
    tab();
    if (serial_sub_menu_items.data_elements[data_to_report][i].length() < 8)
      tab();

    space_colon();

    Serial.println(serial_sub_menu_items.data_descriptions[data_to_report][i]);
  }

  Serial.println();
  if (data_to_report != STOP_REPORT)
  Serial.println(serial_sub_menu_items.prepend_commands);
  else
  Serial.println(serial_sub_menu_items.top_level_commands);
  Serial.println();
  data_to_report = STOP_REPORT;
}

void Host::write_data(String command_data, int value) {

  for (int i = 0; i < serial_sub_menu_items.active_elements_by_row[data_to_report]; i++) {
    if (command_data == serial_sub_menu_items.data_elements[data_to_report][i]) {
      Serial.println(F("Original Value"));
      print_command_name(command_data);
      read_write_LUT(i, 'r');

      read_write_LUT(i, 'w', value);

      print_new_command_name(command_data);
      read_write_LUT(i, 'r');
      break;
    }
  }
}

void Host::return_data(String command_data) {

  for (int i = 0; i < serial_sub_menu_items.active_elements_by_row[data_to_report]; i++) {
    if (command_data == serial_sub_menu_items.data_elements[data_to_report][i]) {
      print_command_name(command_data);
      read_write_LUT(i, 'r');
      ;
    }
  }
}

void Host::read_write_LUT(byte index, char r_w, int value) {

  String pin_error_msg PROGMEM = "Error: Cannot change pin value during operation, aborting assignment";

  switch (data_to_report) {
    case REPORT_FANS:
      switch (index) {
        case 0: (r_w == 'r')  ?  Serial.println(fan_parameters.pin)                          : Serial.println(pin_error_msg);                   break;
        case 1: (r_w == 'r')  ?  Serial.println(fan_parameters.manual_set_value)             : fan_parameters.manual_set_value = value;         break;
        case 2: (r_w == 'r')  ?  Serial.println(fan_parameters.target_speed)                 : fan_parameters.target_speed = value;             break;
        case 3: (r_w == 'r')  ?  Serial.println(fan_parameters.current_speed)                : fan_parameters.current_speed = value;            break;
        case 4: (r_w == 'r')  ?  Serial.println(fan_parameters.change_increment)             : fan_parameters.change_increment = value;         break;
        case 5: (r_w == 'r')  ?  Serial.println(fan_parameters.fan_change_interval)          : fan_parameters.fan_change_interval = value;      break;
        case 6: (r_w == 'r')  ?  Serial.println(fan_parameters.fan_minimum)                  : fan_parameters.fan_minimum = value;              break;
        case 7: (r_w == 'r')  ?  Serial.println(fan_parameters.enabled)                      : fan_parameters.enabled = value;                  break;
        case 8: (r_w == 'r')  ?  Serial.println(fan_parameters.manual)                       : fan_parameters.manual = value;                   break;
      }
      break;

    case REPORT_TEMPS:
      switch (index) {
        case 0: (r_w == 'r')  ?  Serial.println(temp_parameters.pin1)                        : Serial.println(pin_error_msg);                   break;
        case 1: (r_w == 'r')  ?  Serial.println(temp_parameters.pin2)                        : Serial.println(pin_error_msg);                   break;
        case 2: (r_w == 'r')  ?  Serial.println(temp_parameters.pin3)                        : Serial.println(pin_error_msg);                   break;
        case 3: (r_w == 'r')  ?  Serial.println(temp_parameters.enabled1)                    : temp_parameters.enabled1 = value;                break;
        case 4: (r_w == 'r')  ?  Serial.println(temp_parameters.enabled2)                    : temp_parameters.enabled2 = value;                break;
        case 5: (r_w == 'r')  ?  Serial.println(temp_parameters.enabled3)                    : temp_parameters.enabled3 = value;                break;
      }
      break;
  }

}

void Host::print_messages() {
  static uint16_t last_message_print_time;
  static byte previously_reporting = 0; // reset header counter if just changed printing messages

  if (millis() > last_message_print_time + MEGGAGE_DELAY_PERIOD) { // check if specified time delay elapsed

    last_message_print_time = millis();

    // increment counter for printing header info message
    if (previously_reporting != data_to_report)
      header_print_counter = 0;
    else
      header_print_counter += HEADER_PRINT_INCREMENT;

    previously_reporting = data_to_report;

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
      case REPORT_MENU_TREE:
        print_menu_tree();
        break;
      case REPORT_LDRS:
        print_ldrs();
        break;
      case REPORT_ENCODER:
        print_encoder();
        break;
      case REPORT_BUTTON:
        print_button();
        break;
      default: break;
    }

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
  tab(); tab();
  Serial.print(temp_parameters.avg);
  Serial.println();

}

void Host::print_temps() {

  if (header_print_counter == 0) {
    Serial.println();
    Serial.println(F("Pins \t     Sensors Enabled \t Good Connection \t Current Temp Readings \t Average Temp"));
  }

  Serial.print(temp_parameters.pin1);
  space();

  Serial.print(temp_parameters.pin2);
  space();

  Serial.print(temp_parameters.pin3);

  space(); space(); space(); space(); space();

  if (temp_parameters.enabled1)
    Serial.print(F("Y"));
  else
    Serial.print(F("N"));
  space();
  if (temp_parameters.enabled2)
    Serial.print(F("Y"));
  else
    Serial.print(F("N"));
  space();
  if (temp_parameters.enabled3)
    Serial.print(F("Y"));
  else
    Serial.print(F("N"));

  tab(); tab(); space();

  if (!temp_parameters.enabled1)
    dash_space();
  else {
    if (temp_parameters.bad_connection1)
      Serial.print(F("N"));
    else
      Serial.print(F("Y"));
    space();
  }

  if (!temp_parameters.enabled2)
    dash_space();
  else {
    if (temp_parameters.bad_connection2)
      Serial.print(F("N"));
    else
      Serial.print(F("Y"));
    space();
  }

  if (!temp_parameters.enabled3) {
    dash_space(); tab();
  }
  else {
    if (temp_parameters.bad_connection3)
      Serial.print(F("N"));
    else
      Serial.print(F("Y"));
    tab();
  }
  tab(); tab(); space();

  Serial.print(temp_parameters.temp1);
  space();

  Serial.print(temp_parameters.temp2);
  space();

  Serial.print(temp_parameters.temp3);
  tab(); tab(); space();

  Serial.print(temp_parameters.avg);
  Serial.println();


}

void Host::print_led_strip() {

  if (header_print_counter == 0) {
    Serial.println();
    Serial.println(F("Pin \tLED Strip Enabled \tCurrent Brightness \tTarget Brightness \tUse Fast ISR Interval"));
  }

  Serial.print(led_strip_parameters.pin);
  tab();

  if (led_strip_parameters.enabled)
    Serial.print(F("Yes"));
  else
    Serial.print(F("No"));
  tab(); tab(); tab();
  Serial.print(led_strip_parameters.current_brightness);
  tab(); tab(); tab();

  Serial.print(led_strip_parameters.target_brightness);
  tab(); tab(); tab();
  if (led_strip_parameters.fast_interval)
    Serial.println(F("Yes"));
  else
    Serial.println(F("No"));

}

void Host::print_encoder() {

  if (header_print_counter == 0) {
    Serial.println();
    Serial.println(F("Pins \tEncoder Enabled \tISR Attached \tPosition"));
  }

  Serial.print(encoder_parameters.pinA);
  space();

  Serial.print(encoder_parameters.pinB);
  tab();

  if (encoder_parameters.enabled)
    Serial.print(F("Yes"));
  else
    Serial.print(F("No"));

  tab(); tab; tab(); tab();

  if (encoder_parameters.is_attached)
    Serial.print(F("Yes"));
  else
    Serial.print(F("No"));

  tab(); tab; tab();

  Serial.println(encoder_parameters.position);


}

void Host::print_button() {
  static int function_called_last = button_parameters.last_button_pressed;

  if (header_print_counter == 0) {
    Serial.println();
    Serial.println(F("Pin \tButton Enabled \tISR Attached \tPressed"));
  }

  Serial.print(button_parameters.button_pin);
  tab();

  if (button_parameters.enabled)
    Serial.print(F("Yes"));
  else
    Serial.print(F("No "));

  tab; tab(); tab();

  if (button_parameters.is_attached)
    Serial.print(F("Yes"));
  else
    Serial.print(F("No "));

  tab(); tab; tab();

  if (button_parameters.last_button_pressed > function_called_last)
    Serial.print(F("Yes"));
  else
    Serial.print(F("No"));
  Serial.println();

  function_called_last = millis();
}


int button_pin = BUTTON_PIN;              // pin num
volatile bool button_pressed_ISR = false;     // has the button been pressed recently
bool button_pressed = false;
volatile int last_button_pressed = 0;     // when was the button last pressed
int button_press_interval = 300;          // minimum period for button to be de-pressed to register 2 independant pressed
bool enabled = false;
bool is_attached = false;

void Host::print_ldrs() {}
void Host::print_menu_tree() {}
#endif // Host_CPP
