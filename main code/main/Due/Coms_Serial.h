
#ifndef Coms_serial_H
#define Coms_serial_H


#include "Config_Local.h"
#include "Coms.h"

#define PING_WAIT_PERIOD 200

#define MAX_NUM_TEXT_FRAME_RETRASMIT 4
#define MAX_NUM_POS_FRAME_RETRASMIT 1
#define MAX_NUM_SENSOR_DATA_FRAME_RETRASMIT 1
#define MAX_NUM_MENU_FRAME_RETRASMIT 1

#define MULTI_FRAME_DELAY_PERIOD 40   //when sending several frames sequentially, delay so mega can process it, seems to be around 30ms from simples tests, so give a bit overhead

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
    void configure_serial_1(uint32_t baud);
    void configure_serial_2(uint32_t baud);
    void configure_serial_3(uint32_t baud);
    void configure_serial_4(uint32_t baud);

    void disable_timer_interrupts();
    void enable_timer_interrupts();

    void write_frame_history(byte address, byte frame_type, byte loc);
    void write_all_frame_history(byte address);  //write a bunch of frames to this address, hopefully one is right

    void check_mega_1();  //functions to actually check ports for request from mega
    void check_mega_2();
    void check_mega_3();
    void check_mega_4();

  public:

    Coms_Serial() {}

    void init_serial();

    void send_menu_frame(byte cur_menu);
    void send_text_frame(byte obj_num, int8_t address = -1);
    void send_pos_frame(byte obj_num);
    void send_text_calibration_data(byte obj_num);
    void send_specific_calibration_data(byte sensor_prefix, int address, bool more_bytes, uint16_t offset);  //function to send specific value

    void check_megas(); //check if megas sent anything

    void decode_serial_rx(byte *char_array, byte address);
    void send_partial_text_frame(byte address, byte obj_num, byte frame_num);

};


#endif  //Sign_coms_serial_H

