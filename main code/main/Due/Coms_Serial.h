
#ifndef Sign_coms_serial_H
#define Sign_coms_serial_H

#include "Global_variables.h"
#include "Config_Local.h"
class Coms_Serial: public Coms {    

  private:
    

  public:

    Coms_Serial(){}

    int init_serial();            // init the serial at 115200 baud rate
    int init_serial(int speed);   // init the serial at a custom speed

 
     
    int get_serial();                     //function to interpret serial data recieved without a user prompt

    int serial_write_frame(int address) ;                                  //function to actually send the frame to given address
 
};


#endif  //Sign_coms_serial_H

