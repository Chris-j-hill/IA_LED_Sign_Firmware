
#ifndef Coms_serial_H
#define Coms_serial_H


#include "Config_Local.h"
#include "Coms.h"

#define PING_WAIT_PERIOD 200

//#define PACK_FRAME_NUM_DATA(a, b) (((a<<5) & (b<<1)) & 0b11101110) //ensure parity bits are zero in case not used

#define MAX_NUM_TEXT_FRAME_RETRASMIT 4
#define MAX_NUM_POS_FRAME_RETRASMIT 1
#define MAX_NUM_SENSOR_DATA_FRAME_RETRASMIT 1
#define MAX_NUM_MENU_FRAME_RETRASMIT 1


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
    void write_frame(byte address, byte frame_type, byte *buf = 0, byte frame_length = 0); //general function to send a given frame type to specified address, does not check for mega enabled or detected
    inline bool mega_status(byte address); // checks conditions to send frame or not

    void ping();
    void init_software_serial_to_megas();

    void disable_timer_interrupts();
    void enable_timer_interrupts();

    void write_frame_history(byte address, byte frame_type, byte loc);
    void write_all_frame_history(byte address);  //write a bunch of frames to this address, hopefully one is right
    bool error_sanity_check(byte frame_type, byte frame_num, byte obj_num);


  public:

    Coms_Serial() {}

    void init_serial();

    void send_menu_frame(byte cur_menu);
    void send_text_frame(byte obj_num, int8_t address = -1);
    void send_pos_frame(byte obj_num);
    void send_text_calibration_data(byte obj_num);
    void send_specific_calibration_data(byte sensor_prefix, int address, bool more_bytes, uint16_t offset);  //function to send specific value


    void check_megas(); //check if megas sent anything

    void decode_serial_rx(String rx, byte address);
    void send_partial_text_frame(byte address, byte obj_num, byte frame_num);
    //void send_all_calibration_data(byte address);      legacy function, impractical to send all parameters, and unnecessary

};


#endif  //Sign_coms_serial_H

