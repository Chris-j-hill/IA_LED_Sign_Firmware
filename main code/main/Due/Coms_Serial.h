
#ifndef Coms_serial_H
#define Coms_serial_H


#include "Config_Local.h"
#include "Coms.h"

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
    

  public:

    Coms_Serial(){}

//    int init_serial();            // init the serial at 115200 baud rate
//    int init_serial(int speed);   // init the serial at a custom speed

    int init_software_serial_to_megas();
    int init_software_serial_to_megas(int speed);
    
    int Serial_write_frame(int address) ;                                  //function to actually send the frame to given address
    void write_frame(int address);
    void send_menu_frame(int menu, int encoder_pos);
    int send_all_text_frames();
    int send_text_frame(int address);
    void send_pos_frame();
    
    void check_queues();
    

};


#endif  //Sign_coms_serial_H

