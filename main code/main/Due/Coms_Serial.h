
#ifndef Coms_serial_H
#define Coms_serial_H


#include "Config_Local.h"
#include "Coms.h"

#define PING_WAIT_PERIOD 200



struct   Mega_Serial_Parameters{

  bool enabled1 = MEGA_1_ENABLED;
  bool enabled2 = MEGA_2_ENABLED;
  bool enabled3 = MEGA_3_ENABLED;
  bool enabled4 = MEGA_4_ENABLED;

  bool detected1 = false;
  bool detected2 = false;
  bool detected3 = false;
  bool detected4 = false;

  uint16_t baud_rate = 57600; //<- max stable baud rate = 57600
  
};



class Coms_Serial: public Coms {    

  private:
    void write_sensor_data_frame(byte address);
    void write_menu_frame(byte address);
    void write_pos_frame(byte address);
    void write_text_frame(byte address);
    void write_text_frame();   // send to all at once
    
    void check_pos_frame_queue();
    void check_sensor_date_frame_queue();
    void check_text_frame_queue();
    void check_menu_frame_queue();

    void send_long_text_frame(byte address);
    void send_short_text_frame(byte address);

    void ping();
    void init_software_serial_to_megas(int speed = 115200);
    
  public:

    Coms_Serial(){}

//    int init_serial();            // init the serial at 115200 baud rate
//    int init_serial(int speed);   // init the serial at a custom speed

    
    void init_serial();
    
    int Serial_write_frame(int address) ;                                  //function to actually send the frame to given address
    void write_frame(int address);
    void send_menu_frame(int menu, int encoder_pos);
    int send_all_text_frames();
    int send_text_frame(int address);
    void send_pos_frame();
    
    void check_queues();
    

};


#endif  //Sign_coms_serial_H

