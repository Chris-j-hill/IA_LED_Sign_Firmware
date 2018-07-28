
#ifndef Coms_serial_H
#define Coms_serial_H


#include "Config_Local.h"
#include "Coms.h"

#define PING_WAIT_PERIOD 200

#define MEGA_SERIAL_CONNECTION_TESTING  // define to fake a positive response from ping test on initialisation for megas
                                        // useful if megas not loaded with correct code (eg megas running serial passthrough)

struct   Mega_Serial_Parameters {

  bool enabled1 = MEGA_1_ENABLED;
  bool enabled2 = MEGA_2_ENABLED;
  bool enabled3 = MEGA_3_ENABLED;
  bool enabled4 = MEGA_4_ENABLED;

  bool detected1 = false;
  bool detected2 = false;
  bool detected3 = false;
  bool detected4 = false;

  uint16_t baud_rate = COMS_SPEED; //<- max stable baud rate = 57600

};



class Coms_Serial: public Coms {

  private:
    void write_sensor_data_frame(byte address);
    void write_menu_frame(byte address);
    void write_pos_frame(byte address);
    void write_text_frame(byte address);
    inline void write_text_frame();   // send to all at once


    void ping();
    void init_software_serial_to_megas(int speed = COMS_SPEED);
    
  public:

    Coms_Serial() {}

    void init_serial();

    void send_menu_frame(byte menu);
    void send_text_frame(byte obj_num);
    void send_pos_frame(byte obj_num);
    void send_text_calibration_data(byte obj_num);
    void send_specific_calibration_data(byte sensor_prefix, int address, bool more_bytes, int offset);  //function to send specific value

    
    void check_megas(); //check if megas sent anything


    //void send_all_calibration_data(byte address);      legacy function, impractical to send all parameters, and unnecessary 
};


#endif  //Sign_coms_serial_H

