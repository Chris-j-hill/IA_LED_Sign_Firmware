
/*
 * Parent class used to genreate the frames, parse them, do error checking and any other functionallity
 * Actually transmitting the frames is done in the daughter class of the protocol used
 * 
 */

#ifndef Sign_coms_H
#define Sign_coms_H

#include "Global_variables.h"
#include "Config_Local.h"
class coms {

  private:
    

  public:

    coms(){}

    int init_software_serial_to_megas();            // init the software serial at 115200 baud rate
    int init_software_serial_to_megas(int speed);   // init the serial at a custom speed
    int init_software_serial_to_usb_port();            // init the serial at 115200 baud rate
    int init_software_serial_to_usb_port(int speed);   // init the serial at a custom speed
    int startup_handshake();      //startup sequence to ensure due boots first and transmission begins when all megas are ready
    int write_frame(int address);
    int send_disp_string_frame(int address);                             //complete function to send strings over i2c to display on specified screen
    int pack_disp_string_frame(int frame_type, int frame_offset);        //function to pack a frame of text to display
    int send_pos_frame(int address);                                     //function to send the xy coordinates along with a number of other related variables
    int pack_xy_coordinates() ;                                          //function to pack the 4 bytes to send the x and y positions of the text cursor
    int send_all_calibration_data(int address);                          //function to send all data calibration
    int send_specific_calibration_data(byte sensor_prefix, int address, bool more_bytes, int offset);  //function to send specific value
    int send_all_pos_on_interrupt();     // function to send pos data to all megas periodically based on timer interrupt
    int send_all_pos_now();
    int send_all_text();    // send the text frame to all megas
    void print_frame();      //print the currently stored frame to the serial monitor
   
    int get_text_colour_hue(int byte_number);                            //function to retrun the MSB or LSB of the current hue value to send over i2c


//  todo
    
    int generate_checksum();
    int error_check();
     int get_serial();                     //function to interpret serial data recieved without a user prompt

    int get_frame_code();
    int get_data();             //extract data from frame


};




#endif  //Sign_coms_H

