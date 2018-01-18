
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
};


#endif  //Sign_coms_serial_H

