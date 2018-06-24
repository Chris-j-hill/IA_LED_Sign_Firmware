#ifndef Host_H
#define Host_H

#define HOST_SERIAL_SPEED 115200


//class to manage all functions regarding communication with due host device (probably a pi)
class Host{

  private:

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
