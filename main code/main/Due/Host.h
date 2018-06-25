#ifndef Host_H
#define Host_H

#define HOST_SERIAL_SPEED 115200

#define STOP_REPORT           0
#define REPORT_FANS           1
#define REPORT_TEMPS          2
#define REPORT_LED_STRIP      3

#define HEADER_PRINT_INCREMENT  8     // make this a power of 2, auto overflow
#define MEGGAGE_DELAY_PERIOD    200   //delay this many ms between message prints
//class to manage all functions regarding communication with due host device (probably a pi)
class Host{

  private:

  byte data_to_report = STOP_REPORT;    //default to not printing 
  byte header_print_counter =0;
  void print_fans();
  void print_temps();
  void print_led_strip();

 
  public:
  Host(){}
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
