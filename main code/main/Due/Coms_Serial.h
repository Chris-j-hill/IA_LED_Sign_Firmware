
#ifndef Coms_serial_H
#define Coms_serial_H


#include "Config_Local.h"
#include "Coms.h"

class Coms_Serial: public Coms {    

  private:
    

  public:

    Coms_Serial(){}

//    int init_serial();            // init the serial at 115200 baud rate
//    int init_serial(int speed);   // init the serial at a custom speed

    int init_software_serial_to_megas();
    int init_software_serial_to_megas(int speed);
     
    int get_serial();                     //function to interpret serial data recieved without a user prompt
    
    int Serial_write_frame(int address) ;                                  //function to actually send the frame to given address
    void write_frame(int address);
    void send_menu_frame(int menu, int encoder_pos){};
    int send_all_calibration_data(int address);
    int send_all_text_frames();
    int send_text_frame(int address);
    
    void write_sensor_data_frame(){};
    void write_menu_frame(){};
    void write_pos_frame(){};
    void write_text_frame(){};
    
    void check_pos_frame_queue(){};
    void check_sensor_date_frame_queue(){};
    void check_text_frame_queue(){};
    void check_menu_frame_queue(){}
};


#endif  //Sign_coms_serial_H

