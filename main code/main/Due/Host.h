#ifndef Host_H
#define Host_H

#include "Arduino.h"

#define HOST_SERIAL_SPEED 115200

// NB numbering here must match index of string in items array in Serial_Sub_Menu struct
#define STOP_REPORT           0
#define REPORT_FANS           1
#define REPORT_TEMPS          2
#define REPORT_LED_STRIP      3
#define REPORT_MENU_TREE      4
#define REPORT_LDRS           5
#define REPORT_ENCODER        6
#define REPORT_BUTTON         7

#define HEADER_PRINT_INCREMENT  8     // make this a power of 2, auto overflow
#define MEGGAGE_DELAY_PERIOD    200   //delay this many ms between message prints
//class to manage all functions regarding communication with due host device (probably a pi)

#define NUM_FAN_MENU_OPTIONS 11
#define NUM_MENU_ITEMS 8

struct Serial_Sub_Menu {
  String prepend_commands PROGMEM = "to read a value above, type -r followed by the command, to write a new value type -w and append the value(eg fans -w pin 10)";
  String top_level_commands PROGMEM = "for live feedback type the command above, append with -h for a list of specific variables to read/modify";
  
  //String fans [NUM_FAN_MENU_OPTIONS] PROGMEM = {"pin", "manual_speed", "target_speed", "current_speed", "increment", "interval", "minimum", "enabled", "manual"};

       // better to have large 2d array with wasted space than large unravelled loops in code
  String data_elements [NUM_MENU_ITEMS][NUM_FAN_MENU_OPTIONS] PROGMEM = {
   {"stop", "fans", "temp", "strip", "menu", "ldr", "encoder", "button"},
   {"pin", "manual_speed", "target_speed", "current_speed", "increment", "interval", "minimum", "enabled", "manual"},
   {"pin1", "pin2", "pin3", "enable1", "enable2", "enable3"},
   {"pin", "target_brightness", "current_brightness", "increment", "change_interval", "stable_interval", "minimum", "enabled","fast_interval_on", "sinusoidal", "freq"},
   {"pin", "manual_speed", "target_speed", "current_speed", "increment", "interval", "minimum", "enabled", "manual"},
   {"pin", "manual_speed", "target_speed", "current_speed", "increment", "interval", "minimum", "enabled", "manual"},
   {"pin", "manual_speed", "target_speed", "current_speed", "increment", "interval", "minimum", "enabled", "manual"},
   {"pin", "manual_speed", "target_speed", "current_speed", "increment", "interval", "minimum", "enabled", "manual"}};

                                                
  String data_descriptions[NUM_MENU_ITEMS][NUM_FAN_MENU_OPTIONS] PROGMEM = {
    {"stop printing data", "data related to fans", "data related to temperature sensors", "data related to led strip", "data related to the menu tree", "data related to the LDR's", "data related to the encoder wheel", "data related to encoder's button"},
    {"fan attached to this digital pin number", "manually set the speed", "the target fan speed", "the current fan speed", "ISR increment magnitude", "interval period between ISR", "minimum rotating speed of the fan", "is the fan enabled", "is the fan accepting manual speed override"},
    {"sensor 1 attached to this digital pin number", "as above...", "as above...", "set to 1 to enable this sensor, 0 to disable", "as above...", "as above..."},
    {"led strip attached to this digital pin number", "the target brightness", "the current brightness", "ISR increment magnitude", "interval period between ISR when changing brightness", "interval period between ISR when stable", "minimum brightness of led strip", "is the led strip enabled", "can the interval period be set to fast", "set the led behaviour to sinusoidal pulsing", "pulse at this freq if in sinusoidal mode"},
    {"fan attached to this digital pin number", "manually set the speed", "the target fan speed", "the current fan speed", "ISR fan increment magnitude", "interval period between ISR", "minimum rotating speed of the fan", "is the fan enabled", "is the fan accepting manual speed override"},
    {"fan attached to this digital pin number", "manually set the speed", "the target fan speed", "the current fan speed", "ISR fan increment magnitude", "interval period between ISR", "minimum rotating speed of the fan", "is the fan enabled", "is the fan accepting manual speed override"},
    {"fan attached to this digital pin number", "manually set the speed", "the target fan speed", "the current fan speed", "ISR fan increment magnitude", "interval period between ISR", "minimum rotating speed of the fan", "is the fan enabled", "is the fan accepting manual speed override"},
    {"fan attached to this digital pin number", "manually set the speed", "the target fan speed", "the current fan speed", "ISR fan increment magnitude", "interval period between ISR", "minimum rotating speed of the fan", "is the fan enabled", "is the fan accepting manual speed override"}};

  byte active_elements_by_row[NUM_MENU_ITEMS] = {8,9,6,11,0,0,0,0};

  String items[NUM_MENU_ITEMS] = {"stop", "fans", "temp", "strip", "menu", "ldr", "encoder", "button"};

};
//
//struct sub_menu_fans{
//
//  String fans[1] PROGMEM = {"pin\t\t","manual_speed\t","target_speed\t", "current_speed\t", "increment\t","interval\t","minimum\t","enabled\t","manual\t"};
//
//  }

class Host {

  private:

    byte data_to_report = STOP_REPORT;    //default to not printing
    byte header_print_counter = 0;
    void print_fans();
    void print_temps();
    void print_led_strip();
    void print_encoder();
    void print_button();
    void print_ldrs();
    void print_menu_tree();
    void serial_sub_menu(String rx);
    void print_help_options();
    byte data_set_LUT(String data_set);
    void return_data(String command_data); //prints current value to screen
    void write_data(String command_data, int value); // updates specific data
    void read_write_LUT(byte index, char r_w, int value = 0);

  public:
    Host() {}
    void init_serial();
    int request_data();
    int set_text_string();    //if got new string, save to text_str variable

    void check_serial();    //to read incomming data

    void set_debug_mode();   //set the mode, what data to print
    void transmit_modes();   //send back available modes and setting commands
    void stop_debug_mode();  //stop debug prints

    void print_messages();

};


#endif //Host_H
