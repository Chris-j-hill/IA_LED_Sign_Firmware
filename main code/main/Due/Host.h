#ifndef Host_H
#define Host_H

#include "Arduino.h"

#define HOST_SERIAL_SPEED 250000

// NB numbering here must match index of string in items array in Serial_Sub_Menu struct
#define STOP_REPORT           0
#define REPORT_FANS           1
#define REPORT_TEMPS          2
#define REPORT_LED_STRIP      3
#define REPORT_MENU_TREE      4
#define REPORT_LDRS           5
#define REPORT_ENCODER        6
#define REPORT_BUTTON         7
#define REPORT_CURRENT_METER  8

#define REPORT_TEXT           9

#define REPORT_SERIAL         10
#define REPORT_SD_CARD        11

#define REPORT_LDR_CONFIG     12
//#define DUMP_FILE             13



#define HEADER_PRINT_INCREMENT  8     // make this a power of 2, auto overflow...
#define MESSAGE_DELAY_PERIOD    300   //delay this many ms between message prints
//class to manage all functions regarding communication with due host device (probably a pi)

#define MAX_NUM_MENU_OPTIONS 14
#define NUM_MENU_ITEMS 14

#define NUM_USB_COMMANDS 8
#define NATIVE_USB_TIMEOUT_PERIOD 50

struct Serial_Sub_Menu {
  String prepend_commands PROGMEM = "to read a value above, type -r followed by the command, to write a new value type -w and append the value(eg fans -w pin 10)";
  String top_level_commands PROGMEM = "for live feedback type the command above, append with -h for a list of specific variables to read/modify";

  // better to have large 2d arrays with wasted space than large unravelled loops in code

  //these are the terminal input commands to type
  String data_elements [NUM_MENU_ITEMS][MAX_NUM_MENU_OPTIONS] PROGMEM = {
    /*0 */   {"stop", "fans", "temp", "strip", "menu", "ldr", "encoder", "button", "current", "text", "serial", "card", "ldr_config", "dump"},
    /*1 */   {"pin", "manual_speed", "target_speed", "current_speed", "increment", "interval", "minimum", "enabled", "manual"},
    /*2 */   {"pin1", "pin2", "pin3", "enable1", "enable2", "enable3"},
    /*3 */   {"pin", "target_brightness", "current_brightness", "increment", "change_interval", "stable_interval", "minimum", "enabled", "fast_interval_on", "sinusoidal", "freq"},
    /*4 */   {""},  // <- menu handled differently
    /*5 */   {"pin1", "pin2", "enabled1", "enabled2", "large_diff"},
    /*6 */   {"pin1", "pin2", "pos", "enabled", "attached"},
    /*7 */   {"pin", "pressed", "interval", "enabled", "attached"},
    /*8 */   {"pin1", "pin2", "enabled1", "enabled2", "max_current"},
    /*9 */   {"pin1", "pin2", "enabled1", "enabled2", "max_current"},
    /*10*/   {"pin1", "pin2", "enabled1", "enabled2", "max_current"},
    /*11*/   {"pin1", "pin2", "enabled1", "enabled2", "network", "password"},
    /*12*/   {"max1", "min1", "max2", "min2"},
    /*13*/   {"max1", "min1", "max2", "min2"}
  };


  //these are the help notes for above commands
  String data_descriptions[NUM_MENU_ITEMS][MAX_NUM_MENU_OPTIONS] PROGMEM = {
    /*0 */    {"stop printing data", "data related to fans", "data related to temperature sensors", "data related to led strip", "data related to the menu tree", "data related to the LDR's", "data related to the encoder wheel", "data related to encoder's button", "data related to current meters", "current text setting", "serial link settings to megas", "sd card settings", "to configure range of ldr readings", "dump specified file to specified serial"},
    /*1 */    {"fan attached to this digital pin number", "manually set the speed", "the target fan speed", "the current fan speed", "ISR increment magnitude", "interval period between ISR", "minimum rotating speed of the fan", "is the fan enabled", "is the fan accepting manual speed override"},
    /*2 */    {"sensor 1 attached to this digital pin number", "as above...", "as above...", "set to 1 to enable this sensor, 0 to disable", "as above...", "as above..."},
    /*3 */    {"led strip attached to this digital pin number", "the target brightness", "the current brightness", "ISR increment magnitude", "interval period between ISR when changing brightness", "interval period between ISR when stable", "minimum brightness of led strip", "is the led strip enabled", "can the interval period be set to fast", "set the led behaviour to sinusoidal pulsing", "pulse at this freq if in sinusoidal mode"},
    /*4 */    {""}, //<- menu handled differently
    /*5 */    {"LDR 1 attached to this digital pin number", "as above...", "set to 1 to enable this sensor, 0 to disable", "as above...", "if greater difference in readings than this assume one sensor covered and rely on higher returned value (8 bit range)"},
    /*6 */    {"pin 1 attached to this digital pin number, CLK or DT", "as above...", "encoder wheel current position", "set to 1 to enable the encoder, 0 to disable", "set to 1 to attach ISR, 0 to detatch"},
    /*7 */    {"button attached to this digital pin number", "has the button pressed, use to force response from other functions", "interval, in ms, between button presses, used to avoid bounce", "set to 1 to enable button, 0 to disable", "set to 1 to attach ISR, 0 to detatch"},
    /*8 */    {"Current meter 1 attached to this digital pin number", "as above...", "set to 1 to enable this sensor, 0 to disable", "as above...", "max allowable current, due will intervene if current draw higher"},
    /*9 */    {"fan attached to this digital pin number", "manually set the speed", "the target fan speed", "the current fan speed", "ISR fan increment magnitude", "interval period between ISR", "minimum rotating speed of the fan", "is the fan enabled", "is the fan accepting manual speed override"},
    /*10*/    {"fan attached to this digital pin number", "manually set the speed", "the target fan speed", "the current fan speed", "ISR fan increment magnitude", "interval period between ISR", "minimum rotating speed of the fan", "is the fan enabled", "is the fan accepting manual speed override"},
    /*11*/    {"SD card attached to this digital pin number", "as above...", "set to 1 to enable this sensor, 0 to disable", "as above...", "Extracted network name", "Extracted network password"},
    /*12*/    {"ldr 1 max value", "ldr 1 min value", "ldr 2 max value", "ldr 2 min value"},
    /*13*/    {"ldr 1 max value", "ldr 1 min value", "ldr 2 max value", "ldr 2 min value"}
  };

  byte active_elements_by_row[NUM_MENU_ITEMS] = {NUM_MENU_ITEMS, 9, 6, 11, 0, 5, 5, 5, 5, 0, 0, 6, 4, 0}; //number of columns in each row of the above two arrays

};



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
    void print_current_meters();
    void print_ldr_config();
    void print_sd_cards();
    void print_text(String command);



    void serial_sub_menu(String rx);
    void print_help_options();
    byte data_set_LUT(String data_set);
    void return_data(String command_data); //prints current value to screen
    void write_data(String command_data, int value); // updates specific data
    void read_write_LUT(byte index, char r_w, int value = 0);
    void position_to_menu_value();
    void print_menu_tree_options(int cur_menu = -1); //if not argument provided display all sub menus of current menu

    
  public:
    Host() {}
    void init_serial();
    void check_serial();    //to read incomming data
    void print_messages();
    
    void print_bits(uint32_t var, byte digits, byte units, bool carriage_return = false);
    inline void println_bits(uint32_t var, byte digits, byte units){
      print_bits(var, digits, units, true);
    }
};

// class for any data transfer over functions over native usb port
// use this port for automatic data transfer between pi and due
// allows this port to be open and communicating which not overloading the
// users serial monitor or preventing firmware updates
class HostNativeUSB {
  private:
    int type_ok(String rx_type);  //function to return a number corresponding to the location of the data about to be received

    String types[NUM_USB_COMMANDS] PROGMEM = {"disp_text", "command", "ip_addr", "network", "git_commit_msg", "git_commit_tag", "git_commit_author", "ping"};
    String data_out_keyword PROGMEM = "data_out";
    String request_keyword PROGMEM = "request";
    String data_in_keyword PROGMEM = "data_in";
  public:

    HostNativeUSB() {}
    void init_native_usb();
    void push_serial(int loc, String data);   //push data to pi, possibly login info or for non volatile storage?
    void get_serial();      //two step, first type, due confirms if recognised type, data read into specific location (might be a lot)
    void request_data(byte location);    //due requests
    void put_data_into_loc(String rx_string, byte loc, byte obj_num=0);
    void check_connection();
};
#endif //Host_H
