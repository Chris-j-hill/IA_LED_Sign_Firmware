
#ifndef Coms_serial_H
#define Coms_serial_H


#include "Config_Local.h"
#include "Coms.h"

#define PING_WAIT_PERIOD 200



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
    void write_text_frame();   // send to all at once

    inline void check_pos_frame_queue();
    inline void check_sensor_date_frame_queue();
    inline void check_text_frame_queue();
    inline void check_menu_frame_queue();

    void send_long_text_frame(byte address);
    void send_short_text_frame(byte address);




    void ping();
    void init_software_serial_to_megas(int speed = COMS_SPEED);
    void Serial_write_frame(byte address) ;                                  //function to actually send the frame to given address

  public:

    Coms_Serial() {}

    void init_serial();

    void write_frame(int address);
    void send_menu_frame(byte menu);
    void send_all_text_frames(bool send_now = false);
    void send_all_sensor_data_frames(bool send_now = false);
    void send_text_frame(byte obj_num, byte address = 0);
    void send_pos_frame();

    void check_queues();

    void send_all_calibration_data(byte address);                          //function to send all data calibration
    bool send_specific_calibration_data(byte sensor_prefix, int address, bool more_bytes, int offset);  //function to send specific value

};


#endif  //Sign_coms_serial_H

