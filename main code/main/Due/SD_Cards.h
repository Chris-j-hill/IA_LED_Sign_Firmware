#ifndef SD_Cards_H
#define SD_Cards_H

#include "Due_Libraries.h"


class card {

  private:
    int copy_sd_data(const char *ext_file, const char *int_file, const char *ext_dir, const char *int_dir);
    int extract_network_data();
    int buffer_in_header();         // return 1 if the colon is in the header, otherwise 0
    int buffer_is_network();         // check if network stored in buffer
    int buffer_is_password();        // check if password stored in buffer
    int buffer_is_default();        // check if default string stored in buffer
    int string_length();      // calculate the length of the relevent string in file
    int remove_card_1();
    int log_temp_to_sd();               // log data to the sd card data related to temps over time

  public:

    card() {}

    int init_sd_cards();
 


    int read_current_meter(int pin);  // read the current sensor
    float reading_to_amps(int value);   // convert 10 bit analog reading to amps

    int LDR_calibration();        //calibrate maximum minimum and room brightness lights
    int LDR_check_value();        //read the average value of the LDR's, if one fails to read correctly or large disparity, return other, if both fail return fail

    int set_LDR_enable(int sensor);   // toggle, ueful if you dint care about ambient brightness, or sensor not working
    int set_LDR_disable(int sensor);

    int temp_check_temp(int sensor);   // get the temp vaule from the specified sensor register
    int temp_check_humidity(int sensor);         // likewise, might be useful, probably not

    int set_temp_sensor_enable(int sensor);   //code to enable sensors, might be useful to toggle them, maybe autotoggle if not working?
    int set_temp_sensor_disable(int sensor);


    int temp_read_sensor (int pin);        // read temperature based on specified pin, return value in degrees C
    int read_temp_sensor (int pin);       // read temp sensor registers, used to update stored value to most recent
    byte read_temp_data (int pin);        // convert incoming bits from temp sensor to a byte

    
    int get_serial();                     //function to interpret serial data recieved without a user prompt


    // i2c functions
    int send_disp_string_frame(int address);                             //complete function to send strings over i2c to display on specified screen
    int pack_disp_string_frame(int frame_type, int frame_offset);        //function to pack a frame of text to display
    int wire_write_frame(int address) ;                                  //function to actually send the frame to given address
    int send_pos_frame(int address);                                     //function to send the xy coordinates along with a number of other related variables
    int pack_xy_coordinates() ;                                          //function to pack the 4 bytes to send the x and y positions of the text cursor
    int send_all_calibration_data(int address);                          //function to send all data calibration
    int send_specific_calibration_data(byte sensor_prefix, int address, bool more_bytes, int offset);  //function to send specific value
    int get_text_colour_hue(int byte_number);                            //function to retrun the MSB or LSB of the current hue value to send over i2c

    int send_all_pos();     // function to send pos data to all megas periodically based on timer interrupt
    int send_all_pos_now();
    int send_all_text();    // send the text frame to all megas
    int print_frame();      //print the currently stored frame to the serial monitor
};


#endif  // SD_Cards_H
