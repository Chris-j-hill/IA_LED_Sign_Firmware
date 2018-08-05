
#ifndef Coms_serial_H
#define Coms_serial_H


#include "Config_Local.h"
#include "Coms.h"

#define PING_WAIT_PERIOD 200


struct   Mega_Serial_Parameters {

  bool detected1 = false;
  bool detected2 = false;
  bool detected3 = false;
  bool detected4 = false;

};



class Coms_Serial: public Coms {

  private:
    void write_sensor_data_frame(byte address);
    void write_menu_frame(byte address);
    void write_pos_frame(byte address);
    void write_text_frame(byte address);
    inline void write_text_frame();   // send to all at once
    void write_frame(byte address, byte frame_type); //general function to send a given frame type to specified address, does not check for mega enabled or detected
    inline bool mega_status(byte address); // checks conditions to send frame or not
    
    void ping();
    void init_software_serial_to_megas();

    void disable_timer_interrupts();
    void enable_timer_interrupts();
    
  public:

    Coms_Serial() {}

    void init_serial();

    void send_menu_frame(byte menu);
    void send_text_frame(byte obj_num, int8_t address =-1);
    void send_pos_frame(byte obj_num);
    void send_text_calibration_data(byte obj_num);
    void send_specific_calibration_data(byte sensor_prefix, int address, bool more_bytes, uint16_t offset);  //function to send specific value

    
    void check_megas(); //check if megas sent anything

    void decode_serial_rx(String rx, byte address);
    void send_partial_text_frame(byte address, byte obj_num, byte frame_num);
    //void send_all_calibration_data(byte address);      legacy function, impractical to send all parameters, and unnecessary 
};


#endif  //Sign_coms_serial_H

